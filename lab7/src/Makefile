CXX = nvcc
CXXFLAGS = -O3

all: convolution im2col cuDNN


convolution: Q1/convolution.cu
	$(CXX) $(CXXFLAGS) -o $@ $< -w

im2col: Q2/im2col.cu
	$(CXX) $(CXXFLAGS) -o $@ $< -w 

cuDNN: Q3/cuDNN.cu
	$(CXX) $(CXXFLAGS) -o $@ $< -lcudnn -w

clean:
	rm convolution im2col cuDNN