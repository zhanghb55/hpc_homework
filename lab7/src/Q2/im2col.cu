#include <iostream>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
using namespace std;
#define IDX2C(i,j,ld) (((i)*(ld))+(j))

__global__ void load(float * mat,int channel_id, int channel_count, float * unroll,
                    int height_stride,int width_stride,
                    int mat_height,int mat_width,
                    int filter_height,int filter_width,
                    int res_height,int res_width)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int j = blockIdx.y * blockDim.y + threadIdx.y;
    if(i < res_height && j < res_width){
        for(int x = 0;x < filter_height;x++){
            for(int y = 0;y < filter_width;y++){
                unroll[IDX2C(IDX2C(i,j,res_width),IDX2C(x,y,filter_width) + channel_id * filter_height * filter_width ,channel_count * filter_height * filter_width)] = mat[IDX2C(i * height_stride + x,j * width_stride + y,mat_width)];
            }
        }
    }
    __syncthreads();
}

__global__ void MatMul(float * A, float * B ,float * C,int m,int n,int k)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int j = blockIdx.y * blockDim.y + threadIdx.y;
    float sum = 0;
    if (i < m && j < k){
        for(int x = 0;x < n;x++){
            sum += A[i * n + x] * B[x * k + j];
        }
        C[i * k + j] = sum;
    }
}

int main()
{
    timeval t1, t2;
    int x,y;    
    cout << "Input threadsPerBlock.x:";
    cin >> x;
    cout << "Input threadsPerBlock.y:";
    cin >> y;
    dim3 threadsPerBlock(x,y);
    int height, width;
    cout << "Input problem size:";
    cin >> height;
    width = height;
    int filter_height = 3;
    int filter_width = 3;
    
    int channel = 3;
    int stride;
    cout << "Input stride:";
    cin >> stride;
    int padding = ((((height - filter_height) / stride + 1) * stride - (height - filter_height)) % stride ) / 2;

    float* Mat[channel];
    float* filter[channel];

    int Mat_size = sizeof(float) * (height + 2 * padding) * (width + 2 * padding);
    int res_size = sizeof(float) * ((height - filter_height + 2 * padding) / stride + 1) * ((width - filter_width + 2 * padding) / stride + 1);
    int filter_size = sizeof(float) * filter_height * filter_width;

    for(int k = 0;k < channel;k++){
        Mat[k] = (float*) malloc(Mat_size);
        memset(Mat[k],0,sizeof(Mat[k]));
        for(int i = padding;i < height + padding;i++){
            for(int j = padding;j < width + padding;j++){
                Mat[k][IDX2C(i,j,width + 2 * padding)] = IDX2C(i,j,width + 2 * padding);
            }
        }
    }
    for(int i = 0;i < channel;i++){
        filter[i] = (float*)malloc(filter_size);
        for(int j = 0;j < filter_height * filter_width;j++){
            filter[i][j] = j + 1;
        }
    }
    float * res;
    res = (float*)malloc(res_size);

    gettimeofday(&t1, NULL);
    float * d_Mat[channel], *d_filter[channel], *d_res;

    for(int i = 0;i < channel;i++){
        cudaMalloc(&d_Mat[i], Mat_size);
        cudaMemcpy(d_Mat[i],Mat[i],Mat_size,cudaMemcpyHostToDevice);
    }

    for(int i = 0;i < channel;i++){
        cudaMalloc(&d_filter[i],filter_size);
        cudaMemcpy(d_filter[i],filter[i],filter_size,cudaMemcpyHostToDevice);
    }
    cudaMalloc(&d_res, res_size);
    
    float * unroll;
    cudaMalloc(&unroll,res_size * filter_height * filter_width * channel );

    int res_height = (height - filter_height + 2 * padding) / stride + 1;
    int res_width = (width - filter_width + 2 * padding) / stride + 1;
    dim3 numBlocks((res_height % threadsPerBlock.x) ? res_height / threadsPerBlock.x + 1 : res_height / threadsPerBlock.x ,(res_width % threadsPerBlock.y) ? res_width / threadsPerBlock.y + 1 : res_width / threadsPerBlock.y);
        
    for(int i = 0;i < channel;i++){
        load<<<numBlocks, threadsPerBlock>>>(d_Mat[i],i,channel, unroll,stride,stride,height + 2 * padding,width + 2 * padding,filter_height,filter_width,res_height,res_width);
    }

    float * W;
    cudaMalloc(&W,filter_size * channel);

    for(int i = 0;i < channel;i++){
        cudaMemcpy(W + i * (filter_height * filter_width),filter[i],filter_size,cudaMemcpyHostToDevice);
    }

    dim3 numBlocks1((res_height * res_width % threadsPerBlock.x) ? res_height * res_width / threadsPerBlock.x + 1 :res_height * res_width / threadsPerBlock.x ,(filter_height * filter_width % threadsPerBlock.y) ? filter_height * filter_width / threadsPerBlock.y + 1 : filter_height * filter_width / threadsPerBlock.y);

    MatMul<<<numBlocks1,threadsPerBlock>>>(unroll,W,d_res,res_height*res_width,filter_height * filter_width * channel, 1);
    gettimeofday(&t2, NULL);
    printf("convolution time is:%ldμs\n",t2.tv_sec*1000000 + t2.tv_usec - t1.tv_sec*1000000 - t1.tv_usec);

    cudaMemcpy(res, d_res, res_size, cudaMemcpyDeviceToHost);


/*
    for(int x = 0;x < channel;x++){
        cout << "Input channel " << x << " after padding:" << endl;
        for(int i = 0;i < height + 2 * padding;i++){
            for(int j = 0;j < width + 2 * padding;j++){
                cout << Mat[x][i * (width + 2 * padding) + j] << " ";
            }
            cout << endl;
        }
    }

    cout << endl;

    for(int x = 0;x < channel;x++){
        cout << "Filter channel " << x << ":" << endl;
        for(int i = 0;i < filter_height;i++){
            for(int j = 0;j < filter_width;j++){
                cout << filter[x][i * filter_width + j] << ' ';
            }
            cout << endl;
        }
    }
    cout << endl;
    cout << "Res:" << endl;
    for(int i = 0;i < ((height - filter_height + 2 * padding) / stride + 1);i++){
        for(int j = 0;j < ((width - filter_width + 2 * padding) / stride + 1);j++){
            cout << res[i * ((width - filter_width + 2 * padding) / stride + 1) + j] << ' ';
        }
        cout << endl;
    }
*/
    for(int i = 0;i < channel;i++){
        cudaFree(d_Mat[i]);
    }

    for(int i = 0;i < channel;i++){
        cudaFree(d_filter[i]);
    }
    cudaFree(d_res);
    for(int k = 0;k < channel;k++){
        free(Mat[k]);
    }
    for(int i = 0;i < channel;i++){
        free(filter[i]);
    }
    free(res);
}