#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <cuda.h>
#include <sys/time.h>
#include <cudnn.h>
 
#define IDX2C(i,j,ld) (((i)*(ld))+(j))
using namespace std;
#define CUDA_CALL(f) { \
  cudaError_t err = (f); \
  if (err != cudaSuccess) { \
    cout \
        << "    Error occurred: " << err << endl; \
    exit(1); \
  } \
}
 
#define CUDNN_CALL(f) { \
  cudnnStatus_t err = (f); \
  if (err != CUDNN_STATUS_SUCCESS) { \
    cout \
        << "    Error occurred: " << err << endl; \
    exit(1); \
  } \
}
 
__global__ void assignment(float *px) {
  int tid = threadIdx.x + blockIdx.x * blockDim.x;
  px[tid] = tid;
}
 
void print(const float *data, int n, int c, int h, int w) {
  vector<float> buffer(1 << 20);
  CUDA_CALL(cudaMemcpy(
        buffer.data(), data,
        n * c * h * w * sizeof(float),
        cudaMemcpyDeviceToHost));
  int a = 0;
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < c; ++j) {
      cout << "n=" << i << ", c=" << j << ":" << endl;
      for (int k = 0; k < h; ++k) {
        for (int l = 0; l < w; ++l) {
          cout << setw(4) << right << buffer[a];
          ++a;
        }
        cout << endl;
      }
    }
  }
  cout << endl;
}
 
int main() {
    timeval t1, t2;
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

    cudnnHandle_t cudnn;
    CUDNN_CALL(cudnnCreate(&cudnn));

    float* Mat;
    float* filter;
    int Mat_size = sizeof(float) * (height + 2 * padding) * (width + 2 * padding);
    int res_size = sizeof(float) * ((height - filter_height + 2 * padding) / stride + 1) * ((width - filter_width + 2 * padding) / stride + 1);
    int filter_size = sizeof(float) * filter_height * filter_width;


    int res_height = (height - filter_height + 2 * padding) / stride + 1;
    int res_width = (width - filter_width + 2 * padding) / stride + 1;


    Mat = (float*) malloc(Mat_size * channel);
    memset(Mat,0,sizeof(Mat));
    for(int k = 0;k < channel;k++){
      for(int i = padding;i < height + padding;i++){
          for(int j = padding;j < width + padding;j++){
              Mat[k * (height + 2 * padding) * (width + 2 * padding) + IDX2C(i,j,width + 2 * padding)] = IDX2C(i,j,width + 2 * padding);
          }
      }
    }

    filter = (float*)malloc(filter_size * channel);
    for(int i = 0;i < channel;i++){
        for(int j = 0;j < filter_height * filter_width;j++){
            filter[i * filter_height * filter_width + j] = j + 1;
        }
    }
    gettimeofday(&t1, NULL);
    const int in_n = 1;
    const int in_c = 3;
    const int in_h = height;
    const int in_w = width;

    cudnnTensorDescriptor_t in_desc;
    CUDNN_CALL(cudnnCreateTensorDescriptor(&in_desc));
    CUDNN_CALL(cudnnSetTensor4dDescriptor(
          in_desc, CUDNN_TENSOR_NCHW, CUDNN_DATA_FLOAT,
          in_n, in_c, in_h, in_w));

    float *in_data;
    CUDA_CALL(cudaMalloc(
          &in_data, in_n * in_c * in_h * in_w * sizeof(float)));

    // filter
    const int filt_k = 1;
    const int filt_c = 3;
    const int filt_h = filter_height;
    const int filt_w = filter_width;


    cudnnFilterDescriptor_t filt_desc;
    CUDNN_CALL(cudnnCreateFilterDescriptor(&filt_desc));
    CUDNN_CALL(cudnnSetFilter4dDescriptor(
          filt_desc, CUDNN_DATA_FLOAT, CUDNN_TENSOR_NCHW,
          filt_k, filt_c, filt_h, filt_w));
    
    float *filt_data;
    CUDA_CALL(cudaMalloc(
        &filt_data, filt_k * filt_c * filt_h * filt_w * sizeof(float)));

    const int pad_h = padding;
    const int pad_w = padding;
    const int str_h = stride;
    const int str_w = stride;
    const int dil_h = 1;
    const int dil_w = 1;

    float * res;
    res = (float*)malloc(res_size);
    cudnnConvolutionDescriptor_t conv_desc;
    CUDNN_CALL(cudnnCreateConvolutionDescriptor(&conv_desc));
    CUDNN_CALL(cudnnSetConvolution2dDescriptor(
          conv_desc,
          pad_h, pad_w, str_h, str_w, dil_h, dil_w,
          CUDNN_CROSS_CORRELATION, CUDNN_DATA_FLOAT));

    // output
    int out_n;
    int out_c;
    int out_h;
    int out_w;
      
    CUDNN_CALL(cudnnGetConvolution2dForwardOutputDim(
          conv_desc, in_desc, filt_desc,
          &out_n, &out_c, &out_h, &out_w));


    cudnnTensorDescriptor_t out_desc;
    CUDNN_CALL(cudnnCreateTensorDescriptor(&out_desc));
    CUDNN_CALL(cudnnSetTensor4dDescriptor(
          out_desc, CUDNN_TENSOR_NCHW, CUDNN_DATA_FLOAT,
          out_n, out_c, out_h, out_w));

    float *out_data;
    CUDA_CALL(cudaMalloc(
          &out_data, out_n * out_c * out_h * out_w * sizeof(float)));


    cudnnConvolutionFwdAlgo_t algo;
    CUDNN_CALL(cudnnGetConvolutionForwardAlgorithm(
          cudnn,
          in_desc, filt_desc, conv_desc, out_desc,
          CUDNN_CONVOLUTION_FWD_PREFER_FASTEST, 0, &algo));

    cout << "Convolution algorithm: " << algo << endl;
    cout << endl;


    size_t ws_size;
    CUDNN_CALL(cudnnGetConvolutionForwardWorkspaceSize(
          cudnn, in_desc, filt_desc, conv_desc, out_desc, algo, &ws_size));

    float *ws_data;
    CUDA_CALL(cudaMalloc(&ws_data, ws_size));

    cout << "Workspace size: " << ws_size << endl;
    cout << endl;


    float alpha = 1.f;
    float beta = 0.f;


    assignment<<<in_w * in_h, in_n * in_c>>>(in_data);
    cudaMemcpy(filt_data,filter,filt_k * filt_c * filt_h * filt_w * sizeof(float),cudaMemcpyHostToDevice);
    
    
    CUDNN_CALL(cudnnConvolutionForward(
        cudnn,
        &alpha, in_desc, in_data, filt_desc, filt_data,
        conv_desc, algo, ws_data, ws_size,
        &beta, out_desc, out_data));
    
    gettimeofday(&t2, NULL);
    printf("cuDNN time is:%ld μs\n",t2.tv_sec*1000000 + t2.tv_usec - t1.tv_sec*1000000 - t1.tv_usec);
/*
     cout << "in_data:" << endl;
     print(in_data, in_n, in_c, in_h, in_w);
      
     cout << "filt_data:" << endl;
     print(filt_data, filt_k, filt_c, filt_h, filt_w);
      
    cout << "res:" << endl;

    cudaMemcpy(res,out_data,res_size,cudaMemcpyDeviceToHost);

    for(int i = 0;i < ((height - filter_height + 2 * padding) / stride + 1);i++){
      for(int j = 0;j < ((width - filter_width + 2 * padding) / stride + 1);j++){
          cout << res[i * ((width - filter_width + 2 * padding) / stride + 1) + j] << ' ';
      }
      cout << endl;
    }*/

    CUDA_CALL(cudaFree(ws_data));
    CUDA_CALL(cudaFree(out_data));
    CUDNN_CALL(cudnnDestroyTensorDescriptor(out_desc));
    CUDNN_CALL(cudnnDestroyConvolutionDescriptor(conv_desc));
    CUDA_CALL(cudaFree(filt_data));
    CUDNN_CALL(cudnnDestroyFilterDescriptor(filt_desc));
    CUDA_CALL(cudaFree(in_data));
    CUDNN_CALL(cudnnDestroyTensorDescriptor(in_desc));
    CUDNN_CALL(cudnnDestroy(cudnn));
    return 0;
}