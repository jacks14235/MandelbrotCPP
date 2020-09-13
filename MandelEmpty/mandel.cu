#include <cuda_runtime.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "CImg.h"
#include "device_launch_parameters.h"
#include <chrono> 

using namespace std::chrono;

using namespace cimg_library;

__global__ void mandel(double *real, double *imag, int *out, int n, int maxIt)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x;

	double mReal = real[i];
	double mImag = imag[i];
	for (int iter = 0; iter < maxIt; iter++) {
		double mReal2 = mReal * mReal;
		double mImag2 = mImag * mImag;
		double nReal = mReal;
		mReal = mReal2 - mImag2 + real[i];
		mImag = 2 * nReal * mImag + imag[i];
		if (mReal2 + mImag2 > 4) {
			out[i] = iter;
			//charOut[i] = black;
			return;
		}
	}
	out[i] = maxIt;
	//charOut[i] = white;
	
}


int* makeImage(const int xRes, const int yRes, double xStart, double xEnd, double  yStart, double yEnd, int maxIt) {
	double *real;
	double *imag;
	int *out;
	//unsigned char **charOut;

	cudaMallocManaged(&real, xRes * yRes * sizeof(double));
	cudaMallocManaged(&imag, xRes * yRes * sizeof(double));
	cudaMallocManaged(&out, xRes * yRes * sizeof(int));

	double xStep = (xEnd - xStart) / (double)xRes;
	double yStep = (yEnd - yStart) / (double)yRes;
	//std::cout << xStep;
	//std::cout << yStep;
	int c = 0;
	for (int i = 0; i < xRes; i++) {
		for (int j = 0; j < yRes; j++) {
			real[c] = xStart + i * xStep;
			imag[c] = yStart + j * yStep;
			out[i] = 0;
			c++;
		}
	}
	auto start = high_resolution_clock::now();

	mandel << <xRes * yRes / 1024, 1024 >> > (real, imag, out, xRes * yRes, maxIt);

	cudaDeviceSynchronize();
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	std::cout << duration.count();
	std::cout << "\n\n\n";

	cudaFree(real);
	cudaFree(imag);
	return out;
}