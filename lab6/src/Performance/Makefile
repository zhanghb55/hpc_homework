CXX = nvcc
CXXFLAGS = -O3 -Wall

all: CUDA_MatMul omp_CUDA_MatMul CUBLAS_MatMul


CUDA_MatMul: Q1/CUDA_MatMul.cu
	$(CXX) $(CXXFLAGS) -o $@ $< 

omp_CUDA_MatMul: Q2/omp_CUDA_MatMul.cu
	$(CXX) $(CXXFLAGS) -o $@ $< -Xcompiler -fopenmp

CUBLAS_MatMul: Q3/CUBLAS_MatMul.cu
	$(CXX) $(CXXFLAGS) -o $@ $< -lcublas