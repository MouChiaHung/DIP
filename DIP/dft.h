#ifndef _DFT_
#define _DFT_
#include <fstream>
#include <string>
#include <memory>
#include <iostream>
#include <filesystem>
#include <stdio.h>
//#include <complex.h>
#include <complex>
#include <math.h>
#define PI 3.14159265
//#define PI 3.1416
//using namespace std;
using std::complex;

//#define PRINT_DFT

class DFT {
public:
	double n;
	double m;
	complex<double> *matrix; 
	DFT(void);
	DFT(double n, double m, complex<double>* M);
	~DFT(void);
	bool clear(void);
	bool dft2(complex<double>** dst, double* src, double w, double h);
	bool dft2(complex<double>** dst, double* src, double w, double h, double window);
	bool spectrum(double*& dst, double* w, double* h, bool isNormalize);
	bool spectrum(double*& dst, complex<double>* matrix, double* w, double* h, bool isNormalize);
	bool idft2(double*& dst, double* w, double* h, bool output_real);
	bool idft2(double*& dst, double* w, double* h, double window, bool output_real);
	bool fftshift(double*& buf, int w, int h);
	bool fftshift(uint8_t*& buf, int w, int h);
	//bool convolution(double*& dst, double* src, double* kernel, int width, int height, int window);
	//bool convolutionZeroPadding(double*& dst, double* src, double* kernel, int width, int height, int window);
	template<typename T> T mod(const T& a, const T& b);
};
#endif