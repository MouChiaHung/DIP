#ifndef _DCT_
#define _DCT_
#include <fstream>
#include <string>
#include <memory>
#include <iostream>
#include <filesystem>
#include <stdio.h>
using namespace std;
#include <math.h>
#define PI 3.14159265

#define PRINT_DCT 0
#define PRINT_DCT_GROUP 0

class DCT {
public:
	double n;
	double m;
	double* matrix; 
	DCT(void);
	DCT(double n, double m, double* M);
	~DCT(void);
	bool clear(void);
	bool dct2(double** dst, double* src, double w, double h);
	bool dct2(double** dst, double* src, double w, double h, double window);
	bool dct2(double** dst, double* src, double w, double h, double window, double group);
	bool idct2(double*& dst, double* w, double* h);
	bool idct2(double*& dst, double* matrix, double* w, double* h);
	bool idct2(double*& dst, double* w, double* h, double group);
	bool idct2(double*& dst, double* w, double* h, double window, double group);
	bool idct2(double*& dst, double* matrix, double* w, double* h, double window, double group);
	bool spectrum(double** dst, double* w, double* h, bool isNormalize);
	bool spectrum(double** dst, double* matrix, double* w, double* h, bool isNormalize);
};
#endif