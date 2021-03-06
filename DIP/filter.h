#ifndef _FILTER_
#define _FILTER_
#include <fstream>
#include <string>
#include <memory>
#include <iostream>
#include <filesystem>
#include <stdio.h>
#include <complex>
#include <math.h>
using namespace std;
using std::complex;

//#define PRINT_FILTER

class Filter {
public:

	Filter(void);
	~Filter(void);
	bool cast(uint8_t*& dst, double* src, int width, int height);
	bool normalize(uint8_t*& dst, double* src, int width, int height, bool isAllPositive);
	bool correlation(double*& dst, double* src, double* kernel, int width, int height, int window);
	bool convolution(double*& dst, double* src, double* kernel, int width, int height, int window);
	bool convolutionZeroPadding(double*& dst, double* src, double* kernel, int width, int height, int window);
	bool gaussian(double*& dst, double* src, int width, int height, string file);
	bool sobel(double*& dst, double* src, int width, int height, string file);
	bool gradient(double*& magnitude, double*& orientation, double* src, int width, int height, string file);
	bool laplacian(double*& magnitude, double* src, int width, int height, string file);
};
#endif