	/******************************************************************************
	**                      INCLUDE
	*******************************************************************************/
#include <fstream>
#include <string>
#include <memory>
#include <cstdarg>
#include <iostream>
#include <filesystem>
#include "stdlib.h"
#include "conio.h"
#include "tiff.h"
#include "dft.h"
#include "filter.h"
#include <time.h>  
#include "pgm.h"

using namespace std;
	/******************************************************************************
	**                      DEFINE
	*******************************************************************************/
#define Blue 1
#define Green 2
#define Cyan 3
#define Red 4
#define Purple 5
#define Yellow_Dark 6
#define White_Default 7
#define Gray 8
#define Bright_Blue 9
#define Brigth_Green 10
#define Bright_Cyan 11
#define Bright_Red 12
#define Pink 13
#define Yellow 14
#define Bright_White 15

	/******************************************************************************
	**                      GLOBAL VARIABLE
	*******************************************************************************/

	/******************************************************************************
	**                      DECLARATION
	*******************************************************************************/
bool print_bit_plane(uint8_t* src, int w, int h);	

	/******************************************************************************
	**                      DECLARATION OF FUNCTION
	*******************************************************************************/

	/******************************************************************************
	**                      IMPLEMENTATION FOR UTIL
	*******************************************************************************/
#define WHEREARG  __func__
#define LOG(...) debug_print_impl(WHEREARG, ##__VA_ARGS__)
#define LOGV(...) debug_print_verbose_impl(WHEREARG, ##__VA_ARGS__)
#define LOGD(...) debug_print_debug_impl(WHEREARG, ##__VA_ARGS__)
#define LOGE(...) debug_print_error_impl(WHEREARG, ##__VA_ARGS__)
#ifdef NO_LOG
#define printf(...) debug_print_impl(WHEREARG, ##__VA_ARGS__)
#endif
	void debug_print_impl(const char* func, _Printf_format_string_ char const* const _Format, ...) {
#ifdef NO_LOG
		return;
#endif 
		setbuf(stdout, NULL);
		va_list args;
		va_start(args, _Format);
		char db_buf[128];
		sprintf(db_buf, "[%s]", func);
		int db_len = 0;
		db_len = strlen(db_buf);
		vsprintf(db_buf + db_len, _Format, args);
		printf(db_buf);
		va_end(args);

#ifdef EXPORT_LOG
		FILE* file = fopen("log.txt", "a+");
		fputs(db_buf, file);
		fclose(file);
#endif
	}

	void debug_print_verbose_impl(const char* func, _Printf_format_string_ char const* const _Format, ...) {
#ifdef NO_LOG
		//return;
#endif 
		setbuf(stdout, NULL);
		va_list args;
		va_start(args, _Format);
		char db_buf[128] = "";
		sprintf(db_buf, "");
		int db_len = 0;
		db_len = strlen(db_buf);
		vsprintf(db_buf + db_len, _Format, args);
		printf(db_buf);
		va_end(args);

#ifdef EXPORT_LOG
		FILE* file = fopen("log.txt", "a+");
		fputs(db_buf, file);
		fclose(file);
#endif
	}

	void debug_print_debug_impl(const char* func, _Printf_format_string_ char const* const _Format, ...) {
		setbuf(stdout, NULL);
		va_list args;
		va_start(args, _Format);
		char db_buf[128];
		sprintf(db_buf, "[%s]", func);
		int db_len = 0;
		db_len = strlen(db_buf);
		vsprintf(db_buf + db_len, _Format, args);
		std::cout << (db_buf) << std::endl;
		va_end(args);

#ifdef EXPORT_LOG
		FILE* file = fopen("log.txt", "a+");
		fputs(db_buf, file);
		fclose(file);
#endif
	}

	void debug_print_error_impl(const char* func, _Printf_format_string_ char const* const _Format, ...) {
#ifdef NO_LOG
		return;
#endif 
		setbuf(stdout, NULL);
		va_list args;
		va_start(args, _Format);
		char db_buf[128];
		sprintf(db_buf, "[%s]", func);
		int db_len = 0;
		db_len = strlen(db_buf);
		vsprintf(db_buf + db_len, _Format, args);
		printf(db_buf);
		va_end(args);

#ifdef EXPORT_LOG
		FILE* file = fopen("log.txt", "a+");
		fputs(db_buf, file);
		fclose(file);
#endif
	}

ostream& Foo(ostream &stream) {
	stream << "-*-*-*-*- Hello Digital Image Processing -*-*-*-*-\n" << endl;
	return stream;
}

ostream& Qoo(ostream &stream) {
	stream << "\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-" << endl;
	return stream;
}

void byte_swap_uint8_data(uint16_t* data, int w) {
	if (w < 2) return;
	if (w%2 != 0) {
		cout << "byte swap window is odd" << endl;
	}

	for (int i=0; i<w/2; i++) {
		data[i] += data[w-1-i];
		data[w-1-i] = data[i] - data[w-1-i];
		data[i] = data[i] - data[w-1-i];
	}
}

bool print_bit_plane(uint8_t* src, int w, int h) {
	int len_plane = w*h;
	uint8_t* dst_0 =(uint8_t*)malloc(len_plane);
	uint8_t* dst_1 =(uint8_t*)malloc(len_plane);
	uint8_t* dst_2 =(uint8_t*)malloc(len_plane);
	uint8_t* dst_3 =(uint8_t*)malloc(len_plane);
	uint8_t* dst_4 =(uint8_t*)malloc(len_plane);
	uint8_t* dst_5 =(uint8_t*)malloc(len_plane);
	uint8_t* dst_6 =(uint8_t*)malloc(len_plane);
	uint8_t* dst_7 =(uint8_t*)malloc(len_plane);
	if (dst_0 != NULL) memset(dst_0, 0, len_plane);
	if (dst_1 != NULL) memset(dst_1, 0, len_plane);
	if (dst_2 != NULL) memset(dst_2, 0, len_plane);
	if (dst_3 != NULL) memset(dst_3, 0, len_plane);
	if (dst_4 != NULL) memset(dst_4, 0, len_plane);
	if (dst_5 != NULL) memset(dst_5, 0, len_plane);
	if (dst_6 != NULL) memset(dst_6, 0, len_plane);
	if (dst_7 != NULL) memset(dst_7, 0, len_plane);
	for (int p=0; p<8; p++) {
		if (p==0)     
			memcpy(dst_0, src+p*len_plane, len_plane);
		else if (p==1)
			memcpy(dst_1, src+p*len_plane, len_plane);
		else if (p==2)
			memcpy(dst_2, src+p*len_plane, len_plane);
		else if (p==3)
			memcpy(dst_3, src+p*len_plane, len_plane);
		else if (p==4)
			memcpy(dst_4, src+p*len_plane, len_plane);
		else if (p==5)
			memcpy(dst_5, src+p*len_plane, len_plane);
		else if (p==6)
			memcpy(dst_6, src+p*len_plane, len_plane);
		else if (p==7)
			memcpy(dst_7, src+p*len_plane, len_plane);
	}

#if 1
	ofstream fos_0;
	try {
	string file_name_raw = "C:\\src\\amo\\DIP\\Debug\\bprint0.raw";
	remove(file_name_raw.c_str());
	fos_0.open(file_name_raw, fstream::in | fstream::out | fstream::trunc | fstream::binary);
	if (fos_0.fail()) {
		printf("%s\n", strerror(errno));
		return false;
	}
	else fos_0.clear();
	fos_0.write((char*)dst_0, len_plane);
	fos_0.close();
	}
	catch (exception ex) {
		return false;
	}
	ofstream fos_1;
	try {
	string file_name_raw = "C:\\src\\amo\\DIP\\Debug\\bprint1.raw";
	remove(file_name_raw.c_str());
	fos_1.open(file_name_raw, fstream::in | fstream::out | fstream::trunc | fstream::binary);
	if (fos_1.fail()) {
		printf("%s\n", strerror(errno));
		return false;
	}
	else fos_1.clear();
	fos_1.write((char*)dst_1, len_plane);
	fos_1.close();
	}
	catch (exception ex) {
		return false;
	}
	ofstream fos_2;
	try {
	string file_name_raw = "C:\\src\\amo\\DIP\\Debug\\bprint2.raw";
	remove(file_name_raw.c_str());
	fos_2.open(file_name_raw, fstream::in | fstream::out | fstream::trunc | fstream::binary);
	if (fos_2.fail()) {
		printf("%s\n", strerror(errno));
		return false;
	}
	else fos_2.clear();
	fos_2.write((char*)dst_2, len_plane);
	fos_2.close();
	}
	catch (exception ex) {
		return false;
	}
	ofstream fos_3;
	try {
	string file_name_raw = "C:\\src\\amo\\DIP\\Debug\\bprint3.raw";
	remove(file_name_raw.c_str());
	fos_3.open(file_name_raw, fstream::in | fstream::out | fstream::trunc | fstream::binary);
	if (fos_3.fail()) {
		printf("%s\n", strerror(errno));
		return false;
	}
	else fos_3.clear();
	fos_3.write((char*)dst_3, len_plane);
	fos_3.close();
	}
	catch (exception ex) {
		return false;
	}
	ofstream fos_4;
	try {
	string file_name_raw = "C:\\src\\amo\\DIP\\Debug\\bprint4.raw";
	remove(file_name_raw.c_str());
	fos_4.open(file_name_raw, fstream::in | fstream::out | fstream::trunc | fstream::binary);
	if (fos_4.fail()) {
		printf("%s\n", strerror(errno));
		return false;
	}
	else fos_4.clear();
	fos_4.write((char*)dst_4, len_plane);
	fos_4.close();
	}
	catch (exception ex) {
		return false;
	}
	ofstream fos_5;
	try {
	string file_name_raw = "C:\\src\\amo\\DIP\\Debug\\bprint5.raw";
	remove(file_name_raw.c_str());
	fos_5.open(file_name_raw, fstream::in | fstream::out | fstream::trunc | fstream::binary);
	if (fos_5.fail()) {
		printf("%s\n", strerror(errno));
		return false;
	}
	else fos_5.clear();
	fos_5.write((char*)dst_5, len_plane);
	fos_5.close();
	}
	catch (exception ex) {
		return false;
	}
	ofstream fos_6;
	try {
	string file_name_raw = "C:\\src\\amo\\DIP\\Debug\\bprint6.raw";
	remove(file_name_raw.c_str());
	fos_6.open(file_name_raw, fstream::in | fstream::out | fstream::trunc | fstream::binary);
	if (fos_6.fail()) {
		printf("%s\n", strerror(errno));
		return false;
	}
	else fos_6.clear();
	fos_6.write((char*)dst_6, len_plane);
	fos_6.close();
	}
	catch (exception ex) {
		return false;
	}
	ofstream fos_7;
	try {
	string file_name_raw = "C:\\src\\amo\\DIP\\Debug\\bprint7.raw";
	remove(file_name_raw.c_str());
	fos_7.open(file_name_raw, fstream::in | fstream::out | fstream::trunc | fstream::binary);
	if (fos_7.fail()) {
		printf("%s\n", strerror(errno));
		return false;
	}
	else fos_7.clear();
	fos_7.write((char*)dst_7, len_plane);
	fos_7.close();
	return true;
	}
	catch (exception ex) {
		return false;
	}
#endif

#if 1
	if (dst_0) free(dst_0);
	if (dst_1) free(dst_1);
	if (dst_2) free(dst_2);
	if (dst_3) free(dst_3);
	if (dst_4) free(dst_4);
	if (dst_5) free(dst_5);
	if (dst_6) free(dst_6);
	if (dst_7) free(dst_7);
#endif
}

int main()
{
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	printf("start time:%s", asctime(timeinfo) );
#if 0 //test
#if 0 //previous test
	complex<double> c1(2, 1);
	printf("c1:%f+i%f\n", real(c1), imag(c1));
	complex<double> c2(1, 4);
	printf("c2:%f+i%f\n", real(c2), imag(c2));
	//complex<double> c3(real(c1)+real(c2), imag(c1)+imag(c2));
	complex<double> c3 = c1*c2;
	printf("c3:%f+i%f\n", real(c3), imag(c3));
	uint8_t tmp[9] = {0x0};
	for (int i=9-1; i>=0 ; i--) *(tmp+i) = 0xff-i;
	uint16_t tmp2[9];
	for (int i=0; i<9; i++) tmp2[i] = tmp[i];
	byte_swap_uint8_data(tmp2, 9);

	//uint64_t d1 = 0x12345678;
	double d1 = -10.5;
	uint8_t* pc = (uint8_t*)&d1;
	printf("sizeof(double):%d\n", sizeof(double));
	printf("sizeof(d1):%d\n", sizeof(d1));
	printf("d1:%lf\n", d1);
	for (int i=0; i<sizeof(d1); i++)
		printf("[%x]", *(pc+i));
	printf("\n");
#endif
#if 1 //DFT test
	DFT dft;
	int width = 4;
	int height = 3;
	double mat[] = { 1, 1, 3, 2, 3, 4, 123, 154, 55, 2, 22, 233 };
	dft.dft2(mat, 4, 3);
	double* image = NULL;
	double* spec = NULL;
	double w;
	double h;
	dft.spectrum(spec, &w, &h, true);
	dft.fftshift(spec, (int)w, (int)h);
	dft.idft2(image, &w, &h);

	double* dst_1 = NULL;
	double src_1[3*2] = {1,2,3,4,5,6};
	double kernel_1[2*2] = {1,1,1,1};
	//dft.convolutionPadding(dst_1, src_1, kernel_1, 3, 2, 2);

	double* dst2 = NULL;
	double src2[5*6] = {25,100,75,49,130,
						50,80,0,70,100,
						5,10,20,30,0,
						60,50,12,24,32,
						37,53,55,21,90,
						140,17,0,23,222
						};
	double kernel2[3*3] = {1,0,1,
						   0,1,0,
						   0,0,1};
	dft.convolution(dst2, src2, kernel2, 5, 6, 3);
	dft.convolutionZeroPadding(dst2, src2, kernel2, 5, 6, 3);

	double* dst3 = NULL;
	double src3[3*3] = {1,2,3,4,5,6,7,8,9};
	double kernel3[3*3] = {-1,-2,-1,0,0,0,1,2,1};
	//dft.convolution(dst3, src3, kernel3, 3, 3, 3);
	//dft.convolutionZeroPadding(dst3, src3, kernel3, 3, 3, 3);

	return 0;
#endif

	return 0;
#endif

#if 1 //pgm test
	uint8_t* data_pgm = NULL;
	int size_pgm = 0;
	PGM pgm;
	pgm.read("C:\\src\\amo\\DIP\\Debug\\2object.pgm", data_pgm, &size_pgm);
	//pgm.read("C:\\src\\amo\\DIP\\Debug\\lena.pgm", data_pgm, &size_pgm);
	//pgm.read("C:\\src\\amo\\DIP\\Debug\\edge.pgm", data_pgm, &size_pgm);
	//pgm.read("C:\\src\\amo\\DIP\\Debug\\cell.pgm", data_pgm, &size_pgm);
	//pgm.read("C:\\src\\amo\\DIP\\Debug\\virus.pgm", data_pgm, &size_pgm);
	//pgm.read("C:\\src\\amo\\DIP\\Debug\\fingerAir.pgm", data_pgm, &size_pgm);
	//pgm.read("C:\\src\\amo\\DIP\\Debug\\finger.pgm", data_pgm, &size_pgm);
	//pgm.read("C:\\src\\amo\\DIP\\Debug\\Finger2030A.pgm", data_pgm, &size_pgm);
	//pgm.read("C:\\src\\amo\\DIP\\Debug\\finger_on_corner_1 .pgm", data_pgm, &size_pgm);
	//pgm.read("C:\\src\\amo\\DIP\\Debug\\finger_on_corner_2.pgm", data_pgm, &size_pgm);

	//pgm.read("C:\\src\\amo\\DIP\\Debug\\GND_no_charge\\1605654090835.pgm", data_pgm, &size_pgm);
	//pgm.read("C:\\src\\amo\\DIP\\Debug\\GND_no_charge\\1605654093272.pgm", data_pgm, &size_pgm);
	//pgm.read("C:\\src\\amo\\DIP\\Debug\\GND_no_charge\\1605654094949.pgm", data_pgm, &size_pgm);
	//pgm.read("C:\\src\\amo\\DIP\\Debug\\GND_no_charge\\1605654096608.pgm", data_pgm, &size_pgm);
	//pgm.read("C:\\src\\amo\\DIP\\Debug\\GND_no_charge\\1605654098065.pgm", data_pgm, &size_pgm);
	//pgm.read("C:\\src\\amo\\DIP\\Debug\\No_GND_no_charge\\1605654214424.pgm", data_pgm, &size_pgm);
	//pgm.read("C:\\src\\amo\\DIP\\Debug\\No_GND_no_charge\\1605654215487.pgm", data_pgm, &size_pgm);
	//pgm.read("C:\\src\\amo\\DIP\\Debug\\No_GND_no_charge\\1605654216505.pgm", data_pgm, &size_pgm);
	//pgm.read("C:\\src\\amo\\DIP\\Debug\\No_GND_no_charge\\1605654217487.pgm", data_pgm, &size_pgm);
	//pgm.read("C:\\src\\amo\\DIP\\Debug\\No_GND_no_charge\\1605654218469.pgm", data_pgm, &size_pgm);

	//pgm.read("C:\\src\\amo\\DIP\\Debug\\files\\gnd_charger_laptop.pgm", data_pgm, &size_pgm);
	//pgm.read("C:\\src\\amo\\DIP\\Debug\\files\\finger_charger_laptop.pgm", data_pgm, &size_pgm);

#if 0 //filtering
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	printf("filtering() start time:%s", asctime(timeinfo) );
#if 0
	double kernel[3*3] = { 0.1, 0.1, 0.1,
						   0.1, 0.1, 0.1,
						   0.1, 0.1, 0.1,};
#elif 1 //Low pass
	double kernel[3*3] = { 1/16.0, 1/8.0, 1/16.0,
						   1/8.0 , 1/4.0, 1/8.0,
						   1/16.0, 1/8.0, 1/16.0};
#elif 0 //Identity
	double kernel[3*3] = { 0.0, 0.0, 0.0,
						   0.0, 1.0, 0.0,
						   0.0, 0.0, 0.0 };
#elif 0 //High pass A
	double kernel[3*3] = { 0.0-1/16.0, 0.0-1/8.0, 0.0-1/16.0,
						   0.0-1/8.0 , 1.0-1/4.0, 0.0-1/8.0,
						   0.0-1/16.0, 0.0-1/8.0, 0.0-1/16.0};
#elif 0 //High pass B
	double kernel[3*3] = { 0.0-1/9.0, 0.0-1/9.0, 0.0-1/9.0,
						   0.0-1/9.0, 1.0-1/9.0, 0.0-1/9.0,
						   0.0-1/9.0, 0.0-1/9.0, 0.0-1/9.0};
#else 
	double kernel[3*3] = { 0.0, 0.1, 0,
						   0.2, 0.4, 0.2,
						   0.0, 0.1, 0};
#endif
//Laplacian
	double kernel_Gaussian[3*3] = { 1/16.0, 1/8.0, 1/16.0,
									1/8.0 , 1/4.0, 1/8.0,
									1/16.0, 1/8.0, 1/16.0};

	double kernel_Laplacian_A[3*3] = {-1.0, -1.0, -1.0,
									  -1.0,  8.0, -1.0,
									  -1.0, -1.0, -1.0};

	double kernel_Laplacian_B[3*3] = {0.0,-1.0, 0.0,
									 -1.0, 4.0,-1.0,
									  0.0,-1.0, 0.0};
	
	
	double kernel_LoG[5*5] = {0.0,0.0,  1.0,0.0,0.0,
							  0.0,1.0,  2.0,1.0,0.0,
							  1.0,2.0,-16.0,2.0,1.0,	
							  0.0,1.0,  2.0,1.0,0.0,
							  0.0,0.0,  1.0,0.0,0.0};
	double kernel_Gaussian_times_Laplacian[3*3] = { -1.0/16.0,  -1.0/8.0, -1.0/16.0,
													-1.0/8.0,   8.0/4.0,  -1.0/8.0,
													-1.0/16.0,  -1.0/8.0, -1.0/16.0};

	/*
	//A
	[   0.00][   0.38][   0.00]
	[   0.38][   1.25][   0.38]
	[   0.00][   0.38][   0.00]
	//B
	[   0.00][   0.12][   0.00]
	[   0.12][   0.50][   0.12]
	[   0.00][   0.12][   0.00]
	*/
	double *kernel_Gaussian_conv_Laplacian = NULL;

	Filter filter;
	filter.convolution(kernel_Gaussian_conv_Laplacian, kernel_Gaussian, kernel_Laplacian_B, 3, 3, 3);

	//pgm.filtering(kernel, 3);
	pgm.filtering(kernel_Laplacian_B, 3);
	//pgm.filtering(kernel_Gaussian_conv_Laplacian, 3);
	//pgm.filtering(kernel_LoG, 5);
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	printf("filtering() end   time:%s", asctime(timeinfo) );
#endif

#if 1
	if (pgm.raw == NULL) {
		cout << "pgm has no raw!" << endl;
		return 0;
	} 

	Filter filter;
	double* edged = NULL;
	double* lap = NULL;
	double* mag = NULL;
	double* ori = NULL;

	double* src = new double[sizeof(double)*pgm.width*pgm.height];
	for (int j=0; j<pgm.height; j++) {
		for (int i=0; i<pgm.width; i++) {
			src[i+j*pgm.width] = pgm.raw[i+j*pgm.width];
		}
	}

	double* src_gaussianed = new double[sizeof(double)*pgm.width*pgm.height];
	filter.gaussian(src_gaussianed, src, pgm.width, pgm.height, pgm.file);
	//filter.sobel(edged, src, pgm.width, pgm.height, pgm.file);
	filter.gradient(mag, ori, src, pgm.width, pgm.height, pgm.file);
	filter.laplacian(lap, src, pgm.width, pgm.height, pgm.file);

#endif

#if 0 //dft and idft
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	printf("dft_idft() start time:%s", asctime(timeinfo) );
	pgm.dft_idft(80, 80);
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	printf("dft_idft() end   time:%s", asctime(timeinfo) );
#endif

#if 0 //box filter
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	printf("low_pass_eff() start time:%s", asctime(timeinfo) );
	pgm.low_pass_eff(80, 80, 16);
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	printf("low_pass_eff() end   time:%s", asctime(timeinfo) );
	return 0;
#endif
#else //tiff test
	//ostream& (*pf)(ostream&) = Foo; 
	//cout << pf;

	uint8_t* data = NULL;
	int size = 0;
	Tiff t;
	//t.read_type2("C:\\src\\amo\\DIP\\Debug\\barbara.tif", data, &size);
	t.read_type2("C:\\src\\amo\\DIP\\Debug\\finger.tif", data, &size);
	
#if 1 //current topics	
	//t.dft_idft(100, 100);

#if 0
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	printf("low_pass() start time:%s", asctime(timeinfo) );
	t.low_pass(100,100, 20);
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	printf("low_pass() end   time:%s", asctime(timeinfo) );
#endif	

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	printf("low_pass_eff() start time:%s", asctime(timeinfo) );
	t.low_pass_eff(250, 250, 50);
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	printf("low_pass_eff() end   time:%s", asctime(timeinfo) );
#endif

#if 0 //previous topics

	uint8_t* data_bright = new uint8_t[t.image.width*t.image.height];
	t.bright(0x55, data_bright, t.image.width*256);

	uint8_t* data_inverse = new uint8_t[t.image.width*t.image.height];
	t.inverse(data_inverse, 512*256);
#if 0 //double inverse test
	uint8_t* ptmp = t.raw;
	memcpy(t.raw, data_inverse, t.image.width*t.image.height);
	t.inverse(data_inverse, 512*256);
	t.raw = ptmp;
#endif


	uint8_t* planes = new uint8_t[t.image.width*t.image.height*8];
	if (planes!= NULL) memset(planes, 0, t.image.width*t.image.height*8);
	t.bit_plane_slicing(planes);
	print_bit_plane(planes, t.image.width, t.image.height);
	t.bit_plane_reconstruct(planes);

	uint8_t* ht_dots = new uint8_t[t.image.width*t.image.height];
	if (ht_dots != NULL) memset(ht_dots, 0, t.image.width*t.image.height);
	t.halftone_dots(ht_dots, 8);

	uint8_t* bufFloydSteinbergDithering = new uint8_t[t.image.width*t.image.height];
	if (bufFloydSteinbergDithering != NULL) memset(bufFloydSteinbergDithering, 0, t.image.width*t.image.height);
	t.FloydSteinbergDithering(bufFloydSteinbergDithering);

	if (data_bright) delete[] data_bright; 
	if (data_inverse) delete[] data_inverse;
	if (planes) delete[] planes;
	if (ht_dots) delete[] ht_dots;
	if (bufFloydSteinbergDithering) delete[] bufFloydSteinbergDithering;
#endif
#endif
	//pf = Qoo;
	//cout << pf;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	printf("end time:%s", asctime(timeinfo) );
	system("pause");
}
