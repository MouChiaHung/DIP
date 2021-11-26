#ifndef _PGM_
#define _PGM_
#include <fstream>
#include <string>
#include <memory>
#include <cstdarg>
#include <iostream>
#include <filesystem>
#include "stdlib.h"
//#define PRINT_DFT_IDFT
#define FINGER true

typedef enum {
	NO_Centering_NO_Crop = 0,
	ONLY_Crop,
	BOTH_Centering_Crop
} Centering_Crop;

#include "dft.h"
using namespace std;
class PGM {
protected:

private:
	
public:
	PGM(); 
	~PGM();
	bool read(std::string file_name, uint8_t*& data, int* size);
	bool write(char* filename, char* data, int w, int h, int bitsOfPixel);
	bool dft(complex<double>** dst, int n_, int m_);
	bool dft(complex<double>** dst, int n_, int m_, int window);
	bool dft(complex<double>** dst, int n_, int m_, int window, bool dft_by_raw_filtered);
	bool dft(complex<double>** dst, int n_, int m_, int window, bool dft_by_raw_filtered, bool do_hist);
	bool idft(complex<double>* src, int n_, int m_, bool output_real);
	bool dft_idft(int n_, int m_);
	bool low_pass_eff(int n_, int m_, int window);
	bool filtering_by_DFT(double**dst, double* kernel, complex<double>* kernel_DFT, int w_kernel, int h_kernel, int window, bool dft_by_raw_filtered, bool do_inverse_transform);
	bool filtering_by_DFT(double**dst, double* kernel, complex<double>* kernel_DFT, int w_kernel, int h_kernel, int window, bool dft_by_raw_filtered, bool do_inverse_transform, double nsr);
	bool filtering_by_DCT(double**dst, double* kernel, double* kernel_DCT, int w_kernel, int h_kernel, bool dct_by_raw_filtered, bool do_even_extension, bool go_fast);
	bool filtering(double* kernel, int window, int need_centering_crop);
	bool filtering(double* kernel, int window);
	bool dct(double** dst, int n_, int m_);
	bool dct(double** dst, int n_, int m_, int window);
	bool dct(double** dst, int n_, int m_, int window, int group, bool dct_by_raw_filtered);
	bool idct(double* src, int n_, int m_);
	bool idct(double* src, int n_, int m_, int group);
	bool idct(double* src, int n_, int m_, int window, int group);
	bool hist(double** dst, bool update_raw);
	std::string file;
	int type;
	int width;
	int height;
	int depth;
	int size;
	void* raw;
	//For DFT and IDFT
	double* real;
	double* imag;
	double* spec;
	complex<double> *I;	
	double width_I;
	double height_I;
	uint8_t* raw_IDFT;
	//For filtering
	double* raw_filtered;
	int w_filtered;
	int h_filtered;
	//For DCT and IDCT
	double* Y;
	double width_Y;
	double height_Y;
	uint8_t* raw_IDCT;
};

#endif
