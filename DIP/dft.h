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
using namespace std;
using std::complex;

//#define PRINT_DFT

class DFT {
public:
	double n;
	double m;
	complex<double> *matrix; 
	DFT(void);
	~DFT(void);
	bool dft2(double* src, double w, double h);
	bool spectrum(double*& dst, double* w, double* h, bool isNormalize);
	bool idft2(double*& dst, double* w, double* h);
	bool fftshift(double*& buf, int w, int h);
	bool clear(void);
};
#endif