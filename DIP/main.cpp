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
#include "dct.h"
#include "filter.h"
#include <time.h>  
#include "pgm.h"

using namespace std;
	/******************************************************************************
	**                      DEFINE
	*******************************************************************************/
#define LOG(xx_fmt, ...) debug_print(__func__, xx_fmt,  ##__VA_ARGS__)

#define USE_PGM  1
#define USE_TIFF 0

#define RUN_UNIT_TEST     0
#define RUN_HIST          1
#define RUN_FILTERING     0
#define RUN_GRADIENT      0
#define RUN_DFT_AND_IDFT  0
#define RUN_BOX_FILTER    0
#define RUN_FILTER_BY_DFT 0
#define RUN_FILTER_BY_DCT 1
#define RUN_DCT_AND_IDCT  0

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
static void debug_print(const char* func, char const* const xx_fmt, ...) {
	char buf_print[256] = "";
	int  pos_print = 0;
	va_list args;
	va_start(args, xx_fmt);
	//sprintf(buf_print, "[%s][%s]", "MAIN", func);
	sprintf(buf_print, "");
	pos_print = strlen(buf_print);
	vsprintf(buf_print+pos_print, xx_fmt, args);
	printf(buf_print);
	va_end(args);
}

	/******************************************************************************
	**                      IMPLEMENTATION FOR UTIL
	*******************************************************************************/

ostream& Foo(ostream &stream) {
	stream << "-*-*-*-*- Hello Digital Image Processing -*-*-*-*-\n" << std::endl;
	return stream;
}

ostream& Qoo(ostream &stream) {
	stream << "\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-" << std::endl;
	return stream;
}

void byte_swap_uint8_data(uint16_t* data, int w) {
	if (w < 2) return;
	if (w%2 != 0) {
		std::cout << "byte swap window is odd" << std::endl;
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
		LOG("%s\n", strerror(errno));
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
		LOG("%s\n", strerror(errno));
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
		LOG("%s\n", strerror(errno));
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
		LOG("%s\n", strerror(errno));
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
		LOG("%s\n", strerror(errno));
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
		LOG("%s\n", strerror(errno));
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
		LOG("%s\n", strerror(errno));
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
		LOG("%s\n", strerror(errno));
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
	LOG("start time:%s", asctime(timeinfo) );


	//Filter f_log2;
	//int tlog = 0;
	//f_log2.log2(&tlog, 256);
	//f_log2.nextPowerToFloorLog2(&tlog, 360);
	//return 0;

#if RUN_UNIT_TEST //test

#if 0 //mat-multiply test
	double dst[2*3];
	double matA[2*2] = {1, 2, 3, 4};
	double matB[2*3] = {5, 6, 7, 8, 9, 10};
	Filter f_mm;
	if (!f_mm.mat_multiply(dst, matA, matB, 3, 2, 2, 2, 3, 2)) {
		LOG("Failed to mat_multiply\n");
		return 0;
	}
	LOG("mat multiply result:\n");
	for (int j=0;j<2; j++) {
		for (int i=0; i<3; i++) {
			LOG("[%.0f]", dst[i+j*3]);
			if ((i+1)%3 == 0)
				LOG("\n");
		}
	}
#endif

#if 0 //dctmtx test
	double ctm[10*10];
	DCT dct_ctm;
	dct_ctm.dctmtx(ctm, 10);
	LOG("dctmtx:\n");
	for (int j=0;j<10; j++) {
		for (int i=0; i<10; i++) {
			if (ctm[i+j*10] >= 0)
				LOG("[ ");
			else 
				LOG("[");
			LOG("%.2f]", ctm[i+j*10]);
			if ((i+1)%10 == 0)
				LOG("\n");
		}
	}

#endif

#if 0 //mat-transpose test
	double mat_to_transpose[2*3] = {5, 6, 7, 8, 9, 10};
	double mat_transposed[3*2];
	Filter f_mat_transpose;
	if (!f_mat_transpose.mat_transpose(mat_transposed, mat_to_transpose, 3, 2)) {
		LOG("Failed to mat_transpose\n");
		return 0;
	}

	LOG("mat_to_be_transpose:\n");
	int w = 3;
	int h = 2;
	for (int j=0;j<h; j++) {
		for (int i=0; i<w; i++) {
			if (mat_to_transpose[i+j*w] >= 0)
				LOG("[");
			else 
				LOG("[");
			LOG("%.2f]", mat_to_transpose[i+j*w]);
			if ((i+1)%w == 0)
				LOG("\n");
		}
	}
    LOG("mat_transposed:\n");
	w = 2;
	h = 3;
	for (int j=0;j<h; j++) {
		for (int i=0; i<w; i++) {
			if (mat_transposed[i+j*w] >= 0)
				LOG("[");
			else 
				LOG("[");
			LOG("%.2f]", mat_transposed[i+j*w]);
			if ((i+1)%w == 0)
				LOG("\n");
		}
	}
#endif

#if 1 //fast dct and idct test
	int w = 4;
	int h = 4;
	double im[4*4];
	double IM[4*4];
	double im_idct[4*4];
	for (int j=0; j<h; j++)
		for (int i=0; i<w; i++)
			im[i+j*w] = (i+1.0)+((int)(j+1.0))%2*10.0;
	LOG("im:\n");
	for (int j=0;j<h; j++) {
		for (int i=0; i<w; i++) {
			if (im[i+j*w] >= 0)
				LOG("[");
			else 
				LOG("[");
			if (im[i+j*w] < 10)
				LOG(" ");
			LOG("%.0f]", im[i+j*w]);
			if ((i+1)%w == 0)
				LOG("\n");
		}
	}

	DCT dct_fast;
	if (!dct_fast.dct2(IM,im,w)) {
		LOG("Failed to fast dct2\n");
		return 0;
	}
	LOG("IM:\n");
	for (int j=0;j<h; j++) {
		for (int i=0; i<w; i++) {
			if (IM[i+j*w] >= 0)
				LOG("[ ");
			else 
				LOG("[");
			if (IM[i+j*w] < 10)
				LOG(" ");
			LOG("%.2f]", IM[i+j*w]);
			if ((i+1)%w == 0)
				LOG("\n");
		}
	}

	if (!dct_fast.idct2(im_idct,IM,w)) {
		LOG("Failed to fast idct2\n");
		return 0;
	}
	LOG("IM:\n");
	for (int j=0;j<h; j++) {
		for (int i=0; i<w; i++) {
			if (im_idct[i+j*w] >= 0)
				LOG("[ ");
			else 
				LOG("[");
			if (im_idct[i+j*w] < 10)
				LOG(" ");
			LOG("%.2f]", im_idct[i+j*w]);
			if ((i+1)%w == 0)
				LOG("\n");
		}
	}

#endif	
	
#if 0 //atomic test
	complex<double> c1(2, 1);
	LOG("c1:%f+i%f\n", real(c1), imag(c1));
	complex<double> c2(1, 4);
	LOG("c2:%f+i%f\n", real(c2), imag(c2));
	//complex<double> c3(real(c1)+real(c2), imag(c1)+imag(c2));
	complex<double> c3 = c1*c2;
	LOG("c3:%f+i%f\n", real(c3), imag(c3));
	uint8_t tmp[9] = {0x0};
	for (int i=9-1; i>=0 ; i--) *(tmp+i) = 0xff-i;
	uint16_t tmp2[9];
	for (int i=0; i<9; i++) tmp2[i] = tmp[i];
	byte_swap_uint8_data(tmp2, 9);

	//uint64_t d1 = 0x12345678;
	double d1 = -10.5;
	uint8_t* pc = (uint8_t*)&d1;
	LOG("sizeof(double):%d\n", sizeof(double));
	LOG("sizeof(d1):%d\n", sizeof(d1));
	LOG("d1:%lf\n", d1);
	for (int i=0; i<sizeof(d1); i++)
		LOG("[%x]", *(pc+i));
	LOG("\n");
#endif

#if 0 //normalization

	Filter f;
	double data[7] = {10,11,234,45,65,456,0};
	double* norm = NULL;
	f.normalize(norm, data, 7, 1, 0.0, 1.0);
	return 0;
#endif 

#if 0 //Cutoff and zeroPadding test
	
	int w = 8;
	int h = 4;
	double *im = (double*)malloc(sizeof(double)*w*h);

	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			im[i+j*w] = ((double)i+(double)j*(double)w);
		}
	}

	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			LOG("[%.1f]", im[i+j*w]);
			if (!((i+1)%w)) LOG("\n");
		}
	}

	LOG("Cut off...\n");
	Filter f;
	double* im_cutoff = NULL;
	int w_cutoff = 0;
	int h_cutoff = 0;
	f.cutoff(im_cutoff, im, w, h, 1, 1, &w_cutoff, &h_cutoff);
	
	for (int j=0; j<h_cutoff; j++) {
		for (int i=0; i<w_cutoff; i++) {
			LOG("[%.1f]", im_cutoff[i+j*w_cutoff]);
			if (!((i+1)%w_cutoff)) LOG("\n");
		}
	}

	LOG("Zero padding...\n");
	double* im_padded = NULL;
	int w_padded = 0;
	int h_padded = 0;
	f.zeroPadding(im_padded, im_cutoff, w_cutoff, h_cutoff, 1, 1, &w_padded, &h_padded);
	
	for (int j=0; j<h_padded; j++) {
		for (int i=0; i<w_padded; i++) {
			LOG("[%.1f]", im_padded[i+j*w_padded]);
			if (!((i+1)%w_padded)) LOG("\n");
		}
	}



	if (im)
		free(im);
	if (im_cutoff)
		free(im_cutoff);
	if (im_padded)
		free(im_padded);
	return 0;
#endif

#if 0 //Rotate test
	
	int w = 8;
	int h = 4;
	uint8_t *im = (uint8_t*)malloc(sizeof(uint8_t)*w*h);
	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			im[i+j*w] = (i+j*w);
		}
	}

	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			LOG("[%2x]", im[i+j*w]);
			if (!((i+1)%w)) LOG("\n");
		}
	}

	LOG("Rotate...\n");
	Filter f;
	uint8_t* im_rot = NULL;
	int w_rot = 0;
	int h_rot = 0;
	f.rotate90(im_rot, im, w, h, &w_rot, &h_rot);
	
	for (int j=0; j<h_rot; j++) {
		for (int i=0; i<w_rot; i++) {
			LOG("[%2x]", im_rot[i+j*w_rot]);
			if (!((i+1)%w_rot)) LOG("\n");
		}
	}

	if (im)
		free(im);
	if (im_rot)
		free(im_rot);
	return 0;
#endif

#if 1 //Mirror-padding
	int w = 3;
	int h = 3;
	double *im = (double*)malloc(sizeof(double)*w*h);
	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			im[i+j*w] = floor(((i+1)+j*w)%(9)+0.6);
		}
	}

	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			LOG("[%.f]", im[i+j*w]);
			if (!((i+1)%w)) LOG("\n");
		}
	}

	double* dst = NULL;
	int w_dst = 0;
	int h_dst = 0;
	Filter f_mirror_padding;
	if (!f_mirror_padding.mirrorPadding(&dst, im, &w_dst, &h_dst, w, h)) {
		return 0;
	}

	for (int j=0; j<h_dst; j++) {
		for (int i=0; i<w_dst; i++) {
			LOG("[%.f]", dst[i+j*w_dst]);
			if (!((i+1)%w_dst)) LOG("\n");
		}
	}
#endif

#if 0 //Copy
	int w = 3;
	int h = 3;
	double *im = (double*)malloc(sizeof(double)*w*h);
	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			im[i+j*w] = floor(((i+1)+j*w)%(9)+0.6);
		}
	}

	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			LOG("[%.f]", im[i+j*w]);
			if (!((i+1)%w)) LOG("\n");
		}
	}

	LOG("Copy...\n");
	Filter f;
	int w_dst = w * 2;
	int h_dst = h * 2;

	int rf_dst = 0;
	int rt_dst = 0;
	int rs_dst = 0;
	int cf_dst = 0;
	int ct_dst = 0;
	int cs_dst = 0;

	int rf_src = 0;
	int rt_src = 0;
	int rs_src = 0;
	int cf_src = 0;
	int ct_src = 0;
	int cs_src = 0;
	double *dst = (double*)malloc(sizeof(double)*w_dst*h_dst);
	if (!dst) return 0;
	memset(dst, 0, sizeof(double)*w_dst*h_dst);

	//Copy [u, v]
    rf_dst = 0;
	rt_dst = w_dst-1;
	rs_dst = 1;
	cf_dst = 0;
	ct_dst = h_dst-1;
	cs_dst = 1;

	rf_src = 0;
	rt_src = w-1;
	rs_src = 1;
	cf_src = 0;
	ct_src = h-1;
	cs_src = 1;

	LOG("Copy src[%d:%d:%d][%d:%d:%d] to dst[%d:%d:%d][%d:%d:%d]\n"
		, rf_src, rs_src, rt_src, cf_src, cs_src, ct_src
		, rf_dst, rs_dst, rt_dst, cf_dst, cs_dst, ct_dst);
	f.copy(dst, w_dst, rf_dst, rt_dst, rs_dst, cf_dst, ct_dst, cs_dst, im, w, rf_src, rt_src, rs_src, cf_src, ct_src, cs_src);
	
	for (int j=0; j<h_dst; j++) {
		for (int i=0; i<w_dst; i++) {
			LOG("[%.f]", dst[i+j*w_dst]);
			if (!((i+1)%w_dst)) LOG("\n");
		}
	}

	//Copy [-u, v]
    rf_dst = w_dst-1;
	rt_dst = 0;
	rs_dst = -1;
	cf_dst = 0;
	ct_dst = h_dst-1;
	cs_dst = 1;

	rf_src = 0;
	rt_src = w-1;
	rs_src = 1;
	cf_src = 0;
	ct_src = h-1;
	cs_src = 1;

	LOG("Copy src[%d:%d:%d][%d:%d:%d] to dst[%d:%d:%d][%d:%d:%d]\n"
		, rf_src, rs_src, rt_src, cf_src, cs_src, ct_src
		, rf_dst, rs_dst, rt_dst, cf_dst, cs_dst, ct_dst);
	f.copy(dst, w_dst, rf_dst, rt_dst, rs_dst, cf_dst, ct_dst, cs_dst, im, w, rf_src, rt_src, rs_src, cf_src, ct_src, cs_src);
	
	for (int j=0; j<h_dst; j++) {
		for (int i=0; i<w_dst; i++) {
			LOG("[%.f]", dst[i+j*w_dst]);
			if (!((i+1)%w_dst)) LOG("\n");
		}
	}

	//Copy [u, -v]
    rf_dst = 0;
	rt_dst = w_dst-1;
	rs_dst = 1;
	cf_dst = h_dst-1;
	ct_dst = 0;
	cs_dst = -1;

	rf_src = 0;
	rt_src = w-1;
	rs_src = 1;
	cf_src = 0;
	ct_src = h-1;
	cs_src = 1;

	LOG("Copy src[%d:%d:%d][%d:%d:%d] to dst[%d:%d:%d][%d:%d:%d]\n"
		, rf_src, rs_src, rt_src, cf_src, cs_src, ct_src
		, rf_dst, rs_dst, rt_dst, cf_dst, cs_dst, ct_dst);
	f.copy(dst, w_dst, rf_dst, rt_dst, rs_dst, cf_dst, ct_dst, cs_dst, im, w, rf_src, rt_src, rs_src, cf_src, ct_src, cs_src);
	
	for (int j=0; j<h_dst; j++) {
		for (int i=0; i<w_dst; i++) {
			LOG("[%.f]", dst[i+j*w_dst]);
			if (!((i+1)%w_dst)) LOG("\n");
		}
	}

	//Copy [-u, -v]
    rf_dst = w_dst-1;
	rt_dst = 0;
	rs_dst = -1;
	cf_dst = h_dst-1;
	ct_dst = 0;
	cs_dst = -1;

	rf_src = 0;
	rt_src = w-1;
	rs_src = 1;
	cf_src = 0;
	ct_src = h-1;
	cs_src = 1;

	LOG("Copy src[%d:%d:%d][%d:%d:%d] to dst[%d:%d:%d][%d:%d:%d]\n"
		, rf_src, rs_src, rt_src, cf_src, cs_src, ct_src
		, rf_dst, rs_dst, rt_dst, cf_dst, cs_dst, ct_dst);
	f.copy(dst, w_dst, rf_dst, rt_dst, rs_dst, cf_dst, ct_dst, cs_dst, im, w, rf_src, rt_src, rs_src, cf_src, ct_src, cs_src);
	
	for (int j=0; j<h_dst; j++) {
		for (int i=0; i<w_dst; i++) {
			LOG("[%.f]", dst[i+j*w_dst]);
			if (!((i+1)%w_dst)) LOG("\n");
		}
	}

	LOG("Copy center around from big to small...\n");
	int w_dst_cent = w * 1;
	int h_dst_cent = h * 1;
	double *dst_cent = (double*)malloc(sizeof(double)*w_dst_cent*h_dst_cent);
	if (!dst_cent) 
		return 0;
	memset(dst_cent, 0, sizeof(double)*w_dst_cent*h_dst_cent);
	LOG("Copy center around from [%d]x[%d] to [%d]x[%d]...\n", w_dst, h_dst, w_dst_cent,h_dst_cent);
	f.copy_center_around(dst_cent, dst, w_dst_cent, h_dst_cent, w_dst, h_dst);
	for (int j=0; j<h_dst_cent; j++) {
		for (int i=0; i<w_dst_cent; i++) {
			LOG("[%.f]", dst_cent[i+j*w_dst_cent]);
			if (!((i+1)%w_dst_cent)) LOG("\n");
		}
	}

	LOG("Copy center around from small to big...\n");
	int w_dst_cent2 = w * 2 + 1;
	int h_dst_cent2 = h * 2 + 1;
	double* dst_cent2 = (double*)malloc(sizeof(double)*w_dst_cent2*h_dst_cent2);
	if (!dst_cent2) 
		return 0;
	memset(dst_cent2, 0, sizeof(double)*w_dst_cent2*h_dst_cent2);
	LOG("Copy center around from [%d]x[%d] to [%d]x[%d]...\n",w_dst_cent,h_dst_cent,w_dst_cent2,h_dst_cent2);
	f.copy_center_around(dst_cent2, dst_cent, w_dst_cent2, h_dst_cent2, w_dst_cent, h_dst_cent);
	for (int j=0; j<h_dst_cent2; j++) {
		for (int i=0; i<w_dst_cent2; i++) {
			LOG("[%.f]", dst_cent2[i+j*w_dst_cent2]);
			if (!((i+1)%w_dst_cent2)) LOG("\n");
		}
	}

	if (im)
		free(im);
	if (dst)
		free(dst);
	if (dst_cent)
		free(dst_cent);
	if (dst_cent2)
		free(dst_cent2);	
	return 0;
#endif 

#if 0 //Histogram equalization
	int w = 4;
	int h = 4;
	double *im = (double*)malloc(sizeof(double)*w*h);
	if (!im)
		return 0;
	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			im[i+j*w] = floor(((i+1)+j*w)%(8)+0.6);
		}
	}

	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			LOG("[");
			if (im[i+j*w] < 100) LOG(" ");
			if (im[i+j*w] < 10) LOG(" ");
			LOG("%.f]", im[i+j*w]);
			if (!((i+1)%w)) LOG("\n");
		}
	}

	LOG("Hitogram equalization...\n");
	Filter f;
	f.histogram_equalization(im, w*h);

	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			LOG("[");
			if (im[i+j*w] < 100) LOG(" ");
			if (im[i+j*w] < 10) LOG(" ");
			LOG("%.f]", im[i+j*w]);
			
			if (!((i+1)%w)) LOG("\n");
		}
	}

	if (im)
		free(im);

#endif

#if 0 //Element-wise complex multiply
	int w = 2;
	int h = 2;
	double *im_re = (double*)malloc(sizeof(double)*w*h);
	double *im_im = (double*)malloc(sizeof(double)*w*h);
	double *h_re = (double*)malloc(sizeof(double)*w*h);
	double *h_im = (double*)malloc(sizeof(double)*w*h);
	if (!im_re || !im_im)
		return 0;
	LOG("Real of image...\n");
	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			im_re[i+j*w] = floor(((i+1)+j*w)%(8)+0.6);
		}
	}
	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			LOG("[");
			if (im_re[i+j*w] < 100) LOG(" ");
			if (im_re[i+j*w] < 10) LOG(" ");
			LOG("%.f]", im_re[i+j*w]);
			if (!((i+1)%w)) LOG("\n");
		}
	}
	LOG("Imaginary of image...\n");
	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			im_im[i+j*w] = floor(((i+1)+j*w)%(8)+0.6);
		}
	}
	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			LOG("[");
			if (im_im[i+j*w] < 100) LOG(" ");
			if (im_im[i+j*w] < 10) LOG(" ");
			LOG("%.f]", im_im[i+j*w]);
			if (!((i+1)%w)) LOG("\n");
		}
	}
	LOG("Real of kernel...\n");
	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			h_re[i+j*w] = floor(((i+1)+j*w)%(8)+0.6);
		}
	}
	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			LOG("[");
			if (h_re[i+j*w] < 100) LOG(" ");
			if (h_re[i+j*w] < 10) LOG(" ");
			LOG("%.f]", h_re[i+j*w]);
			if (!((i+1)%w)) LOG("\n");
		}
	}
	LOG("Imaginary of kernel...\n");
	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			h_im[i+j*w] = floor(((j+1)+j*w)%(8)+0.6);
		}
	}
	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			LOG("[");
			if (h_im[i+j*w] < 100) LOG(" ");
			if (h_im[i+j*w] < 10) LOG(" ");
			LOG("%.f]", h_im[i+j*w]);
			if (!((i+1)%w)) LOG("\n");
		}
	}

	LOG("Element-wise complex multiply...\n");
	Filter f;
	//double *y_re = (double*)malloc(sizeof(double)*w*h);
	//double *y_im = (double*)malloc(sizeof(double)*w*h);
	double *y_re = NULL;
	double *y_im = NULL;
	f.element_wise_complex_multiply(&y_re, &y_im, im_re, im_im, h_re, h_im, w, h);
	
	LOG("Real of output...\n");
	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			LOG("[");
			if (y_re[i+j*w] < 100) LOG(" ");
			if (y_re[i+j*w] < 10) LOG(" ");
			LOG("%.f]", y_re[i+j*w]);
			if (!((i+1)%w)) LOG("\n");
		}
	}
	LOG("Imaginary of output...\n");
	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			LOG("[");
			if (y_im[i+j*w] < 100) LOG(" ");
			if (y_im[i+j*w] < 10) LOG(" ");
			LOG("%.f]", y_im[i+j*w]);
			if (!((i+1)%w)) LOG("\n");
		}
	}

	LOG("Check...\n");
	complex<double>* ans = (complex<double>*)malloc(sizeof(complex<double>)*w*h);
	if (!ans)
		return false;
	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			ans[i+j*w] = complex<double>(im_re[i+j*w],im_im[i+j*w])*complex<double>(h_re[i+j*w],h_im[i+j*w]);
		}
	}
	LOG("Check real...\n");
	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			LOG("[");
			if (ans[i+j*w]._Val[0] < 100) LOG(" ");
			if (ans[i+j*w]._Val[0] < 10) LOG(" ");
			LOG("%.f]", ans[i+j*w]._Val[0]);
			if (!((i+1)%w)) LOG("\n");
		}
	}
	LOG("Check imaginary...\n");
	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			LOG("[");
			if (ans[i+j*w]._Val[1] < 100) LOG(" ");
			if (ans[i+j*w]._Val[1] < 10) LOG(" ");
			LOG("%.f]", ans[i+j*w]._Val[1]);
			if (!((i+1)%w)) LOG("\n");
		}
	}


	if (y_re)
		free(y_re);
	if (y_im)
		free(y_im);
	if (im_re)
		free(im_re);
	if (im_im)
		free(im_im);
	if (h_re)
		free(h_re);
	if (h_im)
		free(h_im);
	return 0;
#endif


#if 0 //DFT test
	DFT dft;
	int width = 4;
	int height = 3;
	double mat[] = { 1, 1, 3, 2, 3, 4, 123, 154, 55, 2, 22, 233 };
	dft.dft2(NULL, mat, 4, 3);
	double* image = NULL;
	double* spec = NULL;
	double w;
	double h;
	dft.spectrum(spec, &w, &h, true);
	dft.fftshift(spec, (int)w, (int)h);
	dft.idft2(dft.matrix, &w, &h, true);
#endif

#if 0 //DCT test
	DCT dct;
#if 0
	int width    = 4;
	int height   = 4;
	double mat[] = {
					1,2,3,4, 
					5,6,7,8,
					9,10,9,8,
					7,6,5,4
					};
#else
	int width    = 8;
	int height   = 8;
	double mat[] = {
					16, 11, 10, 16, 24, 40, 51, 61,
					12, 12, 14, 19, 26, 58, 60, 55,
					14, 13, 16, 24, 40, 57, 69, 56,
					14, 17, 22, 29, 51, 87, 80, 82,
					18, 22, 37, 56, 68, 109, 103, 77,
					24, 35, 55, 64, 81, 104, 113, 92,
					49, 64, 78, 87, 103, 121, 120, 101,
					72, 92, 95, 98, 112, 100, 103, 99
					};
#endif
	double* X = NULL;
	LOG("--- source ---\n");
	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			LOG("[");
			if (mat[i+j*(int)width] < 100)  LOG(" ");
			if (mat[i+j*(int)width] < 10)   LOG(" ");
			LOG("%.1f]", mat[i+j*(int)width]);
		}
		LOG("\n");
	}

	LOG("\n******** Run DCT by all ********\n");
	dct.dct2(&X, mat, width, height);
	double* image = NULL;
	double* spec = NULL;
	double w;
	double h;
	//dct.spectrum(&spec, &w, &h, true);
	dct.idct2(image, &w, &h);

	LOG("\n******** Run DCT by group ********\n");
	dct.dct2(&X, mat, width, height, width, 4);
	double* image_run_by_group = NULL;
	double* spec_run_by_group = NULL;
	double w_run_by_group;
	double h_run_by_group;
	//dct.spectrum(&spec_run_by_group, &w_run_by_group, &h_run_by_group, true);
	dct.idct2(image_run_by_group, &w_run_by_group, &h_run_by_group, 4);
	return 0;
#endif

#if 0 //Conv test
	Filter filter_conv;
	double* dst_1 = NULL;
	double src_1[3*2] = {1,2,3,4,5,6};
	double kernel_1[2*2] = {1,1,1,1};
	filter_conv.convolutionZeroPadding(dst_1, src_1, kernel_1, 3, 2, 2);

	double* dst2 = NULL;
	double src2[5*5] = {//25,100,75,49,130,
						50,80,0,70,100,
						5,10,20,30,0,
						60,50,12,24,32,
						37,53,55,21,90,
						140,17,0,23,222
						};
	double kernel2[3*3] = {1,0,1,
						   0,1,0,
						   0,0,1};
	filter_conv.convolution(dst2, src2, kernel2, 5, 5, 3);
	filter_conv.convolutionZeroPadding(dst2, src2, kernel2, 5, 5, 3);

	double* dst3 = NULL;
	double src3[3*3] = {1,2,3,4,5,6,7,8,9};
	double kernel3[3*3] = {-1,-2,-1,0,0,0,1,2,1};
	//dft_conv.convolution(dst3, src3, kernel3, 3, 3, 3);
	//dft_conv.convolutionZeroPadding(dst3, src3, kernel3, 3, 3, 3);
	system("pause");
	return 0;
#endif

	return 0;
#endif
#if USE_PGM //pgm test
	std::cout << "\
*************************************\n\
*                PGM                *\n\
*************************************\n";
	uint8_t* data_pgm = NULL;
	int size_pgm = 0;
	PGM pgm;
	//pgm.read("C:\\src\\amo\\DIP\\Debug\\2object.pgm", data_pgm, &size_pgm);
	pgm.read("C:\\src\\amo\\DIP\\Debug\\lena.pgm", data_pgm, &size_pgm);
	//pgm.read("C:\\src\\amo\\DIP\\Debug\\edge.pgm", data_pgm, &size_pgm);
	//pgm.read("C:\\src\\amo\\DIP\\Debug\\cell.pgm", data_pgm, &size_pgm);
	//pgm.read("C:\\src\\amo\\DIP\\Debug\\virus.pgm", data_pgm, &size_pgm);

#if RUN_HIST
	std::cout << "\
*******************************************\n\
*                HITOGRAM EQUALIZATION     *\n\
*******************************************\n";
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	LOG("beg time:%s", asctime(timeinfo));

	pgm.hist(NULL, true);

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	LOG("end time:%s", asctime(timeinfo));
#endif


#if RUN_FILTERING
	std::cout << "\
*************************************\n\
*                FILTERING          *\n\
*************************************\n";
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	LOG("filtering() start time:%s", asctime(timeinfo) );
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

	Filter filter_diy;
	filter_diy.convolution(kernel_Gaussian_conv_Laplacian, kernel_Gaussian, kernel_Laplacian_B, 3, 3, 3);

	//pgm.filtering(kernel, 3);
	//pgm.filtering(kernel_Laplacian_B, 3);
	pgm.filtering(kernel_Gaussian_conv_Laplacian, 3);
	//pgm.filtering(kernel_LoG, 5);
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	LOG("filtering() end time:%s", asctime(timeinfo) );
#endif
#if RUN_GRADIENT
	std::cout << "\
*************************************\n\
*                GRADIENT           *\n\
*************************************\n";
	if (pgm.raw == NULL) {
		std::cout << "pgm has no raw!" << std::endl;
		return 0;
	} 

	Filter filter_gradient;
	double* edged = NULL;
	double* lap = NULL;
	double* mag = NULL;
	double* ori = NULL;

	double* src = new double[sizeof(double)*pgm.width*pgm.height];
	for (int j=0; j<pgm.height; j++) {
		for (int i=0; i<pgm.width; i++) {
			if (pgm.depth == 2) {
				src[i+j*pgm.width] = ((uint16_t*)pgm.raw)[(i+j*pgm.width)];
			}
			else if (pgm.depth == 1) {
				src[i+j*pgm.width] = ((uint8_t*)pgm.raw)[(i+j*pgm.width)];
			} 
		}
	}

	double* src_gaussianed = new double[sizeof(double)*pgm.width*pgm.height];
	filter_gradient.gaussian(src_gaussianed, src, pgm.width, pgm.height, pgm.file);
	filter_gradient.sobel(edged, src, pgm.width, pgm.height, pgm.file);
	filter_gradient.gradient(mag, ori, src, pgm.width, pgm.height, pgm.file);
	filter_gradient.laplacian(lap, src, pgm.width, pgm.height, pgm.file);

#endif
#if RUN_DFT_AND_IDFT
	std::cout << "\
*************************************\n\
*                DFT then IDFT      *\n\
*************************************\n";
#if 1
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	LOG("dft start time:%s", asctime(timeinfo) );
	pgm.dft(NULL, pgm.width, pgm.height, pgm.width, false, false);
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	LOG("dft end time:%s", asctime(timeinfo) );
	return 0;
#else
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	LOG("dft_idft() start time:%s", asctime(timeinfo) );
	pgm.dft_idft(79, 79);
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	LOG("dft_idft() end time:%s", asctime(timeinfo) );
#endif
#endif
#if RUN_BOX_FILTER
	std::cout << "\
*************************************\n\
*                BOX FILTER         *\n\
*************************************\n";
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	LOG("low_pass_eff() start time:%s", asctime(timeinfo) );
	pgm.low_pass_eff(79, 79, 16);
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	LOG("low_pass_eff() end time:%s", asctime(timeinfo) );
#endif

#if RUN_FILTER_BY_DFT
	std::cout << "\
*************************************\n\
*                FILTERING BY DFT   *\n\
*************************************\n";
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	LOG("start time:%s", asctime(timeinfo));
	Filter filter;
	int width_h        = 179; //odd size for no worry of conv aliasing due to centering
	int height_h       = 79;
	int dft_window = height_h/2;

	double* kernel = NULL;
	uint8_t* kernel_GrayScale = (uint8_t*)malloc(sizeof(uint8_t)*width_h*height_h);
	int width_kernel  = 79; //odd size for no worry of conv aliasing due to centering
	int height_kernel = 79;
	bool is_H_shifted_to_center = true;
	bool is_H_from_normalized   = true;
	
	int width_circle  = 16;
	int height_circle = 16;
	int radius_circle = 8;
	if (is_H_shifted_to_center) {
		width_circle = width_h;
		height_circle = height_h;
	}
	if(!filter.circle(&kernel_GrayScale, &kernel, width_circle, height_circle, radius_circle
		, dft_window, width_h, height_h, NULL, NULL, is_H_from_normalized, false, false)) {
		LOG("filter.circle failed\n");
		return 0;
	}

	double* kernel_crop = (double*)malloc(sizeof(double)*width_kernel*height_kernel);
	if (kernel_crop == NULL) {
		return 0;
	}

	//[Spatial domain]convolve whole image with h
	if (width_kernel != width_h || height_kernel != height_h) {
		Filter f_copier;
		
		if (!f_copier.copy_center_around(kernel_crop, kernel, width_kernel, height_kernel, width_h, height_h)){
			LOG("filter.copy_center_around failed\n");
			return 0;
		}
		if (!pgm.filtering(kernel_crop, width_kernel, NO_Centering_NO_Crop)) {
			LOG("pgm.filtering failed\n");
			return 0;
		}
	}
	else {
		if (!pgm.filtering(kernel, width_h, NO_Centering_NO_Crop)) {
			LOG("pgm.filtering failed\n");
			return 0;
		}
	}

	//[Frequency domain]element-multiply whole image by H
	dft_window = width_h/2;
	//dft_window = height_h/2;
	if (!pgm.filtering_by_DFT(NULL, kernel, NULL, width_h, height_h, dft_window, true, true, 1.0/100.0)) {
			LOG("pgm.filtering_by_DFT failed\n");
			return false;
	}

#endif

#if RUN_FILTER_BY_DCT
	std::cout << "\
*************************************\n\
*                FILTERING BY DCT   *\n\
*************************************\n";
	time(&rawtime);
	struct tm *timeinfo_start = localtime(&rawtime);
	double* kernel = NULL;
	Filter filter;
	int width_h       = 180;
	int height_h      = 80;
	int radius_circle = 6;
	int dct_window    = width_h/2;
	bool do_norm2one  = true;
	if(!filter.circle(NULL, &kernel, width_h, height_h, radius_circle
		, dct_window, width_h, height_h, NULL, NULL, do_norm2one, false, true)) {
		LOG("filter.circle failed\n");
		return 0;
	}

	//[Spatial domain]convolve whole image with h
	int width_kernel  = 79; //odd size for no worry of conv aliasing due to centering
	int height_kernel = 79;
	int width_kernel_even_ext  = 0; 
	int height_kernel_even_ext = 0;
	double* kernel_even_ext = NULL;
	double* kernel_crop = (double*)malloc(sizeof(double)*width_kernel*height_kernel);
	if (kernel_crop == NULL) {
		return 0;
	}
	Filter f_copier;
	DFT dft_shifter;

	if (!f_copier.mirrorPadding(&kernel_even_ext, kernel, &width_kernel_even_ext, &height_kernel_even_ext, width_h, height_h)){
		LOG("filter.mirrorPadding failed\n");
		return 0;
	}

	if (!dft_shifter.fftshift(kernel_even_ext, width_kernel_even_ext, height_kernel_even_ext)){
		LOG("dft.fftshift failed\n");
		return 0;
	}
		
	if (!f_copier.copy_center_around(kernel_crop, kernel_even_ext, width_kernel, height_kernel, width_kernel_even_ext, height_kernel_even_ext)){
		LOG("filter.copy_center_around failed\n");
		return 0;
	}
	if (!pgm.filtering(kernel_crop, width_kernel, NO_Centering_NO_Crop)) {
		LOG("pgm.filtering failed\n");
		return 0;
	}

	//[Prepare inverse kernel]
	bool do_inverse = true;
	if (do_inverse) {
		if(!filter.circle(NULL, &kernel, width_h, height_h, radius_circle
			, dct_window, width_h, height_h, NULL, NULL, do_norm2one, true, true)) {
			LOG("filter.circle failed\n");
			return 0;
		}
	}

	//[Frequency domain]element-multiply whole image by H
	bool go_fast = true;
	bool dct_by_raw_filtered = true;
	if (go_fast) {
		int N = 79;
		double* kr       = (double*)malloc(sizeof(double)*N*N);
		double* raw_db   = (double*)malloc(sizeof(double)*pgm.width*pgm.height);
		double* raw_crop = (double*)malloc(sizeof(double)*N*N);
		if (!kr || !raw_db || !raw_crop)
			return false;

		if (!f_copier.copy(kr, kernel, N, N, width_h, height_h)){
			LOG("filter.copy_center_around failed for kernel\n");
			return 0;
		}
		if (kernel) {
			free(kernel);
		}
		kernel = kr;
		width_h  = N;
		height_h = N;

		if (dct_by_raw_filtered){
			if (pgm.raw_filtered == NULL)
				return false;
			if (!f_copier.copy_center_around(raw_crop, pgm.raw_filtered, N, N, pgm.w_filtered, pgm.h_filtered)){
				LOG("filter.copy_center_around failed for pgm raw filtered\n");
				return 0;
			}
			if (pgm.raw_filtered)
				free(pgm.raw_filtered);
			pgm.raw_filtered = (double*)malloc(sizeof(double)*N*N);
			for (int i=0; i<N*N; i++) {
				pgm.raw_filtered[i] = raw_crop[i];
			}
			pgm.w_filtered  = N;
			pgm.h_filtered = N;

		}
		else {
			for (int i=0; i<pgm.width*pgm.height; i++) {
				if (pgm.depth == 2)
					raw_db[i] = ((uint16_t*)pgm.raw)[i];
				else if  (pgm.depth == 1) {
					raw_db[i] = ((uint8_t*)pgm.raw)[i];
				}
				else {
					return false;
				}
			}
			if (!f_copier.copy_center_around(raw_crop, raw_db, N, N, pgm.width, pgm.height)){
				LOG("filter.copy_center_around failed for pgm raw\n");
				return 0;
			}
			if (raw_crop == NULL)
				return false;
			if (pgm.raw) {
				free(pgm.raw);
				pgm.raw = NULL;
			}
			pgm.raw    = malloc(sizeof(uint8_t)*N*N);
			pgm.depth  = 1;
			pgm.width  = N;
			pgm.height = N;
			for (int i=0; i<N*N; i++) {
				((uint8_t*)pgm.raw)[i] = raw_crop[i];
			}
		} 
		if (raw_db)
			free(raw_db);
		if (raw_crop)
			free(raw_crop);
	}
	if (!pgm.filtering_by_DCT(NULL, kernel, NULL, width_h, height_h, dct_by_raw_filtered, false, go_fast)) {
			LOG("pgm.filtering_by_DCT failed\n");
			return false;
	}

	//free resource
	if (kernel)
		free(kernel);
	if (kernel_crop)
		free(kernel_crop);
#endif

#if RUN_DCT_AND_IDCT
	std::cout << "\
*************************************\n\
*                DCT then IDCT      *\n\
*************************************\n";
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	LOG("dct start time:%s", asctime(timeinfo) );
	int w_dct = 64;
	int h_dct = 64;
	int window_dct = w_dct;
	int group_dct = 8;
	//pgm.dct(NULL,128,128);
#if RUN_DECONV_BY_DCT
	pgm.dct(NULL, w_dct, h_dct, window_dct, group_dct, true);
#else
	pgm.dct(NULL, w_dct, h_dct, window_dct, group_dct, false);
#endif
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	LOG("dct end time:%s", asctime(timeinfo) );

	//pgm.idct(NULL, 128, 128);
	pgm.idct(NULL, w_dct, h_dct, window_dct, group_dct);


#endif

#elif USE_TIFF //tiff test
	std::cout << "\
*************************************\n\
*                TIFF               *\n\
*************************************\n";
	//ostream& (*pf)(ostream&) = Foo; 
	//std::cout << pf;

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
	LOG("low_pass() start time:%s", asctime(timeinfo) );
	t.low_pass(100,100, 20);
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	LOG("low_pass() end time:%s", asctime(timeinfo) );
#endif	

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	LOG("low_pass_eff() start time:%s", asctime(timeinfo) );
	t.low_pass_eff(250, 250, 50);
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	LOG("low_pass_eff() end time:%s", asctime(timeinfo) );
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

#else
	std::cout << "\
*************************************\n\
*                USE NOTHING         *\n\
*************************************\n";
#endif
	//pf = Qoo;
	//std::cout << pf;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	LOG("end time:%s", asctime(timeinfo) );
	system("pause");
}
