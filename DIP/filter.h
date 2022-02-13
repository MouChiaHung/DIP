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
#include <time.h>  
using namespace std;
using std::complex;

//#define PRINT_FILTER

typedef struct ImageKV {
	int    key;
	double val;
} ImageKV;

class Filter {
public:

	Filter(void);
	~Filter(void);
	bool log2(int* dst, int src);
	bool nextPowerToFloorLog2(int* dst, int src);
	bool mat_transpose(double* dst, double* src, int w_src, int h_src);
	bool mat_multiply(double* dst, double* matA, double* matB, int w_dst, int h_dst, int w_matA, int h_matA, int w_matB, int h_matB);
	bool mat_multiply_by_matB_Transpose(double* dst, double* matA, double* matB, int w_dst, int h_dst, int w_matA, int h_matA, int w_matB, int h_matB);
	bool mat_multiply_by_matA_Transpose(double* dst, double* matA, double* matB, int w_dst, int h_dst, int w_matA, int h_matA, int w_matB, int h_matB);
	bool element_wise_multiply(double* dst, double* img, double* ker, int w, int h);
	bool element_wise_complex_multiply(complex<double>* dst, complex<double>* img, complex<double>* ker, int w, int h);
	bool element_wise_complex_multiply(double** dst_re, double** dst_im
										 , double*  img_re, double*  img_im
										 , double*  ker_re, double*  ker_im
										 , int w, int h);
	bool cast(uint8_t*& dst, double* src, int width, int height);
	bool mirrorPadding(double** pdst, double* src, int* w_dst, int* h_dst, int w_src, int h_src);
	bool zeroPadding(double*& dst, double* src, int w, int h, int w_append, int h_append, int* w_dst, int* h_dst);
	bool cutoff(double*& dst, double* src, int w, int h, int w_remove, int h_remove, int* w_dst, int* h_dst);
	bool rotate90(uint8_t*& dst, uint8_t* src, int w, int h, int* w_dst, int* h_dst);
	bool copy(double* dst, double* src, int w_dst, int h_dst, int w_src, int h_src);
	bool copy(double* dst, int width_dst, int rf_dst, int rt_dst, int rs_dst, int cf_dst, int ct_dst, int cs_dst
			, double* src, int width_src, int rf_src, int rt_src, int rs_src, int cf_src, int ct_src, int cs_src);
	bool copy_center_around(double* dst, double* src, int w_dst, int h_dst, int w_src, int h_src);
	bool histogram_equalization(double* src, int length);
	bool normalize(uint8_t*& dst, double* src, int width, int height, bool isAllPositive); //double to uint8
	bool normalize(double*& dst, double* src, int width, int height, double alpha, double beta); //double to double
	bool normalize(double*& dst, double* src, int width, int height, double alpha, double beta, bool do_divide_by_sum);
	bool correlation(double*& dst, double* src, double* kernel, int width, int height, int window);
	bool convolution(double*& dst, double* src, double* kernel, int width, int height, int window); //go with odd dimension
	bool convolutionZeroPadding(double*& dst, double* src, double* kernel, int width, int height, int window);
	bool deconvolution_wiener(uint8_t** reconstructed, complex<double>* I, complex<double>* H, int width, int height, int window, double nsr, std::string file, bool is_H_shifted_to_center);
	bool deconvolution_dct(uint8_t** reconstructed, double* I, double* H, int width, int height, int window, int group, double nsr, string file);
	bool gaussian(double*& dst, double* src, int width, int height, std::string file);
	bool sobel(double*& dst, double* src, int width, int height, std::string file);
	bool gradient(double*& magnitude, double*& orientation, double* src, int width, int height, std::string file);
	bool laplacian(double*& magnitude, double* src, int width, int height, std::string file);
	bool circle(uint8_t** dst, double** kernel, int w_circle, int h_circle, int r_circle, int window, int w_img, int h_img, complex<double>** H, double** Y, bool is_norm_kernel, bool is_inverse_psf, bool do_DCT);
	bool rectangle(uint8_t** dst, int w_rect, int h_rect, int r_rect, int w_img, int h_img, complex<double>** H, bool dft_by_normalized);
	bool cos_x(uint8_t** dst, int w_func, int h_func, int dft_window, int w_img, int h_img, double freq, double phi, complex<double>** H, bool dft_by_normalized);
};
#endif
