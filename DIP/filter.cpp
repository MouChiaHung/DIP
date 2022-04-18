	/******************************************************************************
	**                      INCLUDE
	*******************************************************************************/
#include "filter.h"
#include "dft.h"
#include <cstdarg>
#include "dct.h"

	/******************************************************************************
	**                      DEFINE
	*******************************************************************************/
//extern void debug_print(const char* func, char const* const xx_fmt, ...);
#define PI 3.14159265
#define LOG(xx_fmt, ...) debug_print(__func__, xx_fmt,  ##__VA_ARGS__)

	/******************************************************************************
	**                      GLOBAL VARIABLE
	*******************************************************************************/

	/******************************************************************************
	**                      DECLARATION
	*******************************************************************************/

	/******************************************************************************
	**                      IMPLEMENTATION
	*******************************************************************************/
static void debug_print(const char* func, char const* const xx_fmt, ...) {
	char buf_print[256] = "";
	int  pos_print = 0;
	va_list args;
	va_start(args, xx_fmt);
	sprintf(buf_print, "[%s][%s]", "FILTER", func);
	pos_print = strlen(buf_print);
	vsprintf(buf_print+pos_print, xx_fmt, args);
	printf(buf_print);
	va_end(args);
}

Filter::Filter() {

}

Filter::~Filter() {

}

bool Filter::nextPowerToFloorLog2(int* dst, int src) {
	if (src < 0)
		return false;
	int ret = 0;
	while (src >>= 1) {
		ret++;
	}
	*dst = 1<<(ret+1);
	return true;
}

bool Filter::log2(int* dst, int src) {
	if (src < 0)
		return false;
	int ret = 0;
	while (src >>= 1) {
		ret++;
	}
	*dst = ret;
	return true;
}

bool Filter::mat_transpose(double* dst, double* src, int w_src, int h_src) {
	if (dst == NULL || src == NULL) {
		return false;
	}
	memset(dst, 0, sizeof(double)*w_src*h_src);
	int ii = 0;
	int jj = 0;
	int ww = h_src;
	int hh = w_src;
	for (int i=0; i<w_src; i++) {
		for (int j=0; j<h_src; j++) {
				ii = j;
				jj = i;
				if (ii >= ww || jj >= hh)
					return false;
				dst[ii+jj*(ww)] = src[i+j*w_src];
		}	
	}
	return true;
}

bool Filter::mat_multiply(double* dst, double* matA, double* matB, int w_dst, int h_dst, int w_matA, int h_matA, int w_matB, int h_matB) {
	if (dst == NULL || matA == NULL || matB == NULL)
		return false;
	memset(dst, 0, sizeof(double)*w_dst*h_dst);
	for (int j=0; j<h_dst; j++) {
		for (int i=0; i<w_dst; i++) {
			for (int p=0; (p<w_matA && p<h_matB); p++) {
				if ((dst + (i+j*w_dst)) == NULL)
					return false;
				if ((matA + (p+j*w_matA)) == NULL)
					return false;
				if ((matB + (i+p*w_matB)) == NULL)
					return false;
				dst[i+j*w_dst] += matA[p+j*w_matA] * matB[i+p*w_matB];
				if (abs(dst[i+j*w_dst]) >= (1000.0*1000.0)) {
					LOG("Crazy value at dst[%d][%d]:%.3f\n", i, j, dst[i+j*w_dst]);
					return false;
				}
			}
		}
	}
	return true;
}

bool Filter::mat_multiply_by_matA_Transpose(double* dst, double* matA, double* matB, int w_dst, int h_dst, int w_matA, int h_matA, int w_matB, int h_matB) {
	if (dst == NULL || matA == NULL || matB == NULL)
		return false;
	memset(dst, 0, sizeof(double)*w_dst*h_dst);
	for (int j=0; j<h_dst; j++) {
		for (int i=0; i<w_dst; i++) {
			for (int p=0; (p<w_matA && p<w_matB); p++) {
				if ((dst + (i+j*w_dst)) == NULL)
					return false;
				if ((matA + (p+j*w_matA)) == NULL)
					return false;
				if ((matB + (i+p*w_matB)) == NULL)
					return false;
				dst[i+j*w_dst] += matA[j+p*w_matA] * matB[i+p*w_matB];
				if (abs(dst[i+j*w_dst]) >= (1000.0*1000.0)) {
					LOG("Crazy value at dst[%d][%d]:%.3f\n", i, j, dst[i+j*w_dst]);
					return false;
				}
			}
		}
	}
	return true;
}

bool Filter::mat_multiply_by_matB_Transpose(double* dst, double* matA, double* matB, int w_dst, int h_dst, int w_matA, int h_matA, int w_matB, int h_matB) {
	if (dst == NULL || matA == NULL || matB == NULL)
		return false;
	memset(dst, 0, sizeof(double)*w_dst*h_dst);
	for (int j=0; j<h_dst; j++) {
		for (int i=0; i<w_dst; i++) {
			for (int p=0; (p<w_matA && p<w_matB); p++) {
				if ((dst + (i+j*w_dst)) == NULL)
					return false;
				if ((matA + (p+j*w_matA)) == NULL)
					return false;
				if ((matB + (i+p*w_matB)) == NULL)
					return false;
				dst[i+j*w_dst] += matA[p+j*w_matA] * matB[p+i*w_matB];
				if (abs(dst[i+j*w_dst]) >= (1000.0*1000.0)) {
					LOG("Crazy value at dst[%d][%d]:%.3f\n", i, j, dst[i+j*w_dst]);
					return false;
				}
			}
		}
	}
	return true;
}

bool Filter::element_wise_multiply(double* dst, double* img, double* ker, int w, int h) 
{
	if (w<1 || h<1) {
		LOG("w or h < 1\n");
		return false;
	}
	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			if (!&dst[i+j*w] || !&img[i+j*w] || !&ker[i+j*w]) {
				return false;
			}
			dst[i+j*w] = img[i+j*w] * ker[i+j*w];
		}
	}
	return true;
}

bool Filter::element_wise_complex_multiply(complex<double>* dst, complex<double>* img, complex<double>* ker, int w, int h) 
{
	if (w<1 || h<1) {
		LOG("w or h < 1\n");
		return false;
	}
	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			if (!&dst[i+j*w] || !&img[i+j*w] || !&ker[i+j*w]) {
				return false;
			}
			dst[i+j*w] = img[i+j*w] * ker[i+j*w];
		}
	}
	return true;
}


bool Filter::element_wise_complex_multiply(double** dst_re, double** dst_im
										 , double*  img_re, double*  img_im
										 , double*  ker_re, double*  ker_im
										 , int w, int h) 
{
	if (w<1 || h<1) {
		LOG("w or h < 1\n");
		return false;
	}
	
	LOG("w:%d, h:%d\n", w, h);
	if (*dst_re) {
		free(*dst_re);
		*dst_re = NULL;
	}
	*dst_re = (double*)malloc(sizeof(double)*w*h);
	if (*dst_re == NULL)
		return false;
	memset(*dst_re, 0, sizeof(double)*w*h);
	if (*dst_im) {
		free(*dst_im);
		*dst_im = NULL;
	}
	*dst_im = (double*)malloc(sizeof(double)*w*h);
	if (*dst_im == NULL)
		return false;
	memset(*dst_im, 0, sizeof(double)*w*h);

	double re = 0;
	double im = 0;

	double re1 = 0;
	double im1 = 0;
	double re2 = 0;
	double im2 = 0;

	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			re1 = img_re[i+j*w];
			im1 = img_im[i+j*w];
			re2 = ker_re[i+j*w];
			im2 = ker_im[i+j*w];
			re = re1*re2 - im1*im2;
			im = re1*im2 + im1*re2;
			(*dst_re)[i+j*w] = re;
			(*dst_im)[i+j*w] = im;
		}
	}

	return true;
}

bool Filter::mirrorPadding(double** pdst, double* src, int* w_dst, int* h_dst, int w_src, int h_src) {
	double* dst = *pdst;
	int w = 0;
	int h = 0;
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
	
	if (dst) {
		free(dst);
		dst = NULL;
	}
#if 0
	if (!nextPowerToFloorLog2(w_dst, w_src*2))
		return false;
	if (!nextPowerToFloorLog2(h_dst, h_src*2))
		return false;
#endif
	*w_dst = 2*w_src;
	*h_dst = 2*h_src;
	w = *w_dst;
	h = *h_dst;
	dst = (double*)malloc(sizeof(double)*w*h);
	if (dst == NULL)
		return false;
	memset(dst, 0, sizeof(double)*w*h);
	*pdst = dst;

	//Copy [u, v]
    rf_dst = 0;
	rt_dst = w_src-1;
	rs_dst = 1;
	cf_dst = 0;
	ct_dst = h_src-1;
	cs_dst = 1;

	rf_src = 0;
	rt_src = w_src-1;
	rs_src = 1;
	cf_src = 0;
	ct_src = h_src-1;
	cs_src = 1;
	LOG("Copy src[%d:%d:%d][%d:%d:%d] to dst[%d:%d:%d][%d:%d:%d]\n"
		, rf_src, rs_src, rt_src, cf_src, cs_src, ct_src
		, rf_dst, rs_dst, rt_dst, cf_dst, cs_dst, ct_dst);
	if (!copy(dst, w, rf_dst, rt_dst, rs_dst, cf_dst, ct_dst, cs_dst, src, w_src, rf_src, rt_src, rs_src, cf_src, ct_src, cs_src)) {
		return false;
	}

	//Copy [-u, v]
    rf_dst = 2*w_src-1;
	rt_dst = 0;
	rs_dst = -1;
	cf_dst = 0;
	ct_dst = h_src-1;
	cs_dst = 1;

	rf_src = 0;
	rt_src = w_src-1;
	rs_src = 1;
	cf_src = 0;
	ct_src = h_src-1;
	cs_src = 1;
	LOG("Copy src[%d:%d:%d][%d:%d:%d] to dst[%d:%d:%d][%d:%d:%d]\n"
		, rf_src, rs_src, rt_src, cf_src, cs_src, ct_src
		, rf_dst, rs_dst, rt_dst, cf_dst, cs_dst, ct_dst);
	if (!copy(dst, w, rf_dst, rt_dst, rs_dst, cf_dst, ct_dst, cs_dst, src, w_src, rf_src, rt_src, rs_src, cf_src, ct_src, cs_src)) {
		return false;
	}

	//Copy [u, -v]
    rf_dst = 0;
	rt_dst = w_src-1;
	rs_dst = 1;
	cf_dst = 2*h_src-1;
	ct_dst = 0;
	cs_dst = -1;

	rf_src = 0;
	rt_src = w_src-1;
	rs_src = 1;
	cf_src = 0;
	ct_src = h_src-1;
	cs_src = 1;

	LOG("Copy src[%d:%d:%d][%d:%d:%d] to dst[%d:%d:%d][%d:%d:%d]\n"
		, rf_src, rs_src, rt_src, cf_src, cs_src, ct_src
		, rf_dst, rs_dst, rt_dst, cf_dst, cs_dst, ct_dst);
	if (!copy(dst, w, rf_dst, rt_dst, rs_dst, cf_dst, ct_dst, cs_dst, src, w_src, rf_src, rt_src, rs_src, cf_src, ct_src, cs_src)) {
		return false;
	}

	//Copy [-u, -v]
    rf_dst = 2*w_src-1;
	rt_dst = 0;
	rs_dst = -1;
	cf_dst = 2*h_src-1;
	ct_dst = 0;
	cs_dst = -1;

	rf_src = 0;
	rt_src = w_src-1;
	rs_src = 1;
	cf_src = 0;
	ct_src = h_src-1;
	cs_src = 1;

	LOG("Copy src[%d:%d:%d][%d:%d:%d] to dst[%d:%d:%d][%d:%d:%d]\n"
		, rf_src, rs_src, rt_src, cf_src, cs_src, ct_src
		, rf_dst, rs_dst, rt_dst, cf_dst, cs_dst, ct_dst);
	if (!copy(dst, w, rf_dst, rt_dst, rs_dst, cf_dst, ct_dst, cs_dst, src, w_src, rf_src, rt_src, rs_src, cf_src, ct_src, cs_src)) {
		return false;
	}
	return true;
}

bool Filter::zeroPadding(double*& dst, double* src, int w, int h, int w_append, int h_append, int* w_dst, int* h_dst) {
	if (dst) {
		free(dst);
		dst = NULL;
	}
	*w_dst = w+w_append;
	*h_dst = h+h_append;
	dst = (double*)malloc(sizeof(double)*(*w_dst)*(*h_dst));
	if (dst == NULL)
		return false;
	memset(dst, 0, sizeof(double)*(*w_dst)*(*h_dst));
	int ii = 0;
	int jj = 0;
	for (int j=h_append; j<*h_dst; j++) {
		for (int i=w_append; i<*w_dst; i++) {
			ii = i-w_append;
			jj = j-h_append;
			if (ii>=0 && jj>=0 && ii<w && jj<h) {	
				dst[i+j*(*w_dst)] = src[ii+jj*w];
			}
			else
				return false;
		}	
	}
}

bool Filter::cutoff(double*& dst, double* src, int w, int h, int w_remove, int h_remove, int* w_dst, int* h_dst) {
	if (dst) {
		free(dst);
		dst = NULL;
	}
	*w_dst = w-w_remove;
	*h_dst = h-h_remove;
	dst = (double*)malloc(sizeof(double)*(*w_dst)*(*h_dst));
	if (dst == NULL)
		return false;
	memset(dst, 0, sizeof(double)*(*w_dst)*(*h_dst));
	int ii = 0;
	int jj = 0;
	for (int j=h_remove; j<h; j++) {
		for (int i=w_remove; i<w; i++) {
			ii = i-w_remove;
			jj = j-h_remove;
			if (ii>=0 && jj>=0 && ii<*w_dst && jj<*h_dst) {	
				dst[ii+jj*(*w_dst)] = src[i+j*w];
			}
			else
				return false;
		}	
	}
}

bool Filter::rotate90(uint8_t*& dst, uint8_t* src, int w, int h, int* w_dst, int* h_dst) {
	if (dst) {
		free(dst);
		dst = NULL;
	}
	*w_dst = h;
	*h_dst = w;
	dst = (uint8_t*)malloc(sizeof(uint8_t)*(*w_dst)*(*h_dst));
	if (dst == NULL)
		return false;

	memset(dst, 0, sizeof(uint8_t)*(*w_dst)*(*h_dst));
	int ii = 0;
	int jj = 0;
	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			if (ii>=0 && jj>=0 && ii<*w_dst && jj<*h_dst) {
				ii = ((*w_dst)-1)-j;
				jj = i;
				dst[ii+jj*(*w_dst)] = src[i+j*w];
			}
			else
				return false;
		}	
	}
}

bool Filter::rotate(uint8_t* dst, uint8_t* src, int w, int h, int depth, int theta) {
	if (depth != 8) {
		LOG("Only support 8bits image !\n");
		return false;
	}
	memset(dst, 0, sizeof(uint8_t)*w*h);
	float phi = theta/180.0*PI;
	float T[2][2]     = { cos(phi), -sin(phi),
						  sin(phi),  cos(phi)};
	float T_inv[2][2] = { cos(phi),  sin(phi),
						  -sin(phi), cos(phi)};
	int i_;
	int j_;

	int center_i = round(w/2.0);
	int center_j = round(h/2.0);

	//v' = Einv * v
	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			i_ = round(T_inv[0][0]*(i-center_i) + T_inv[0][1]*(j-center_j)) + center_i;
			j_ = round(T_inv[1][0]*(i-center_i) + T_inv[1][1]*(j-center_j)) + center_j;
			if (i_ < 0 || j_ < 0)
				continue;
			if (i_ >= w)
				continue;
			if (j_ >= h)
				continue;

			//LOG("dst[%d][%d]=src[%d][%d]\n", i, j, i_, j_);
			dst[i+j*w] = src[i_+j_*w];
		}
	}
	return true;
}

bool Filter::reflect(uint8_t* dst, uint8_t* src, int w, int h, int depth, int theta) {
	if (depth != 8) {
		LOG("Only support 8bits image !\n");
		return false;
	}
	memset(dst, 0, sizeof(uint8_t)*w*h);
	float phi = theta/180.0*PI;
	float T[2][2]     = { cos(phi),  sin(phi),
						  sin(phi), -cos(phi)};
	float T_inv[2][2] = { cos(phi),  sin(phi),
						  sin(phi),  -cos(phi)};
	int i_;
	int j_;

	int center_i = round(w/2.0);
	int center_j = round(h/2.0);

	//v' = Einv * v
	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			i_ = round(T_inv[0][0]*(i-center_i) + T_inv[0][1]*(j-center_j)) + center_i;
			j_ = round(T_inv[1][0]*(i-center_i) + T_inv[1][1]*(j-center_j)) + center_j;
			if (i_ < 0 || j_ < 0)
				continue;
			if (i_ >= w)
				continue;
			if (j_ >= h)
				continue;

			//LOG("dst[%d][%d]=src[%d][%d]\n", i, j, i_, j_);
			dst[i+j*w] = src[i_+j_*w];
		}
	}
	return true;
}

bool Filter::copy(double* dst, double* src, int w_dst, int h_dst, int w_src, int h_src) {	
	if (dst == NULL) 
		return false;
	if (src == NULL) 
		return false;
	if (w_src < w_dst) {
		LOG("w_src:%d < w_dst:%d\n", w_src, w_dst);
	}
	if (h_dst < h_dst) {
		LOG("h_src:%d < h_dst:%d\n", h_src, h_dst);
	}

	int k = 0;

	int rf_src = 0;
	int rt_src = (w_dst-1)<=(w_src-1)?(w_dst-1):(w_src-1);
	int cf_src = 0;
	int ct_src = (h_dst-1)<=(h_src-1)?(h_dst-1):(h_src-1);
	
	int rf_dst = 0;
	int rt_dst = w_dst-1;
	int cf_dst = 0;
	int ct_dst = h_dst-1;


	int ii = 0;
	int jj = 0;
	for (int j=cf_src; j<=ct_src; j++) {
		for (int i=rf_src; i<=rt_src; i++) {
			ii = rf_dst+(i-rf_src);
			jj = cf_dst+(j-cf_src);
			if (ii > rt_dst) {
				LOG("[%d][%d] out of from [%d][%d] to [%d][%d]\n",ii,jj,rf_dst,cf_dst,rt_dst,ct_dst);
				return false;
			}
			dst[ii+jj*w_dst] = (src)[(i+j*w_src)];
		}
	}
#if 1 //debug	
	ofstream fos;
	try {
		string file_name = "C:\\src\\amo\\DIP\\Debug\\";
		char str[512];
		sprintf(str, "cp_%dx%d_to_%dx%d.raw", w_src, h_src, w_dst, h_dst);
		file_name += str;
		remove(file_name.c_str());
		fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			LOG("%s\n", strerror(errno));
			return false;
		}
		else fos.clear();
		fos.write((char*)dst, sizeof(double)*(double)w_dst*(double)h_dst);
		fos.close();
		LOG("[Copy image file]:%s\n", file_name.c_str());
	}
	catch (exception ex) {
		return false;
	}
#endif
	LOG("Done to copy [%d]x[%d] into [%d]x[%d]\n", w_src, h_src, w_dst, h_dst);
	return true;
}

bool Filter::copy_center_around(double* dst, double* src, int w_dst, int h_dst, int w_src, int h_src) {	
	if (dst == NULL) 
		return false;
	if (src == NULL) 
		return false;
	if (w_dst < w_src && h_dst > h_src) //no service for gGyY
		return false;
	if (w_dst > w_src && h_dst < h_src) //no service for GgYy
		return false;

	int k = 0;
	int w_aliasing = 0; //size limit is dimension divided by 2 so even values causes one overflow
	int h_aliasing = 0;

	int rf_src = 0;
	int rt_src = 0;
	int cf_src = 0;
	int ct_src = 0;
	int rf_dst = 0;
	int rt_dst = 0;
	int cf_dst = 0;
	int ct_dst = 0;

	if (w_dst <= w_src && h_dst <= h_src) {
		w_aliasing = w_dst-((w_src/2+w_dst/2)-(w_src/2-w_dst/2));
		h_aliasing = h_dst-((h_src/2+h_dst/2)-(h_src/2-h_dst/2));
		if (w_aliasing || h_aliasing)
			LOG("aliasing of w:%d, h:%d\n", w_aliasing, h_aliasing);
		rf_dst = 0;
		rt_dst = w_dst-1;
		cf_dst = 0;
		ct_dst = h_dst-1;

		rf_src = (w_src/2-w_dst/2);
		rt_src = (w_src/2+w_dst/2) + w_aliasing - 1;
		cf_src = (h_src/2-h_dst/2);
		ct_src = (h_src/2+h_dst/2) + h_aliasing - 1;
		LOG("[Big src -> small dst]From [%d][%d] - [%d][%d] to [%d][%d] - [%d][%d]...\n", rf_src, cf_src, rt_src, ct_src, rf_dst, cf_dst, rt_dst, ct_dst);
	}
	else if (w_dst > w_src && h_dst > h_src){
		w_aliasing = w_src-((w_dst/2+w_src/2)-(w_dst/2-w_src/2));
		h_aliasing = h_src-((h_dst/2+h_src/2)-(h_dst/2-h_src/2));
		if (w_aliasing || h_aliasing)
			LOG("aliasing of w:%d, h:%d\n", w_aliasing, h_aliasing);

		rf_src = 0;
		rt_src = w_src-1;
		cf_src = 0;
		ct_src = h_src-1;

		rf_dst = (w_dst/2-w_src/2);
		rt_dst = (w_dst/2+w_src/2) + w_aliasing - 1;
		cf_dst = (h_dst/2-h_src/2);
		ct_dst = (h_dst/2+h_src/2) + h_aliasing - 1;
		LOG("[Small src -> big dst]From [%d][%d] - [%d][%d] to [%d][%d] - [%d][%d]...\n", rf_src, cf_src, rt_src, ct_src, rf_dst, cf_dst, rt_dst, ct_dst);
	}
	else {
		return false;
	}

	int ii = 0;
	int jj = 0;
	for (int j=cf_src; j<=ct_src; j++) {
		for (int i=rf_src; i<=rt_src; i++) {
			ii = rf_dst+(i-rf_src);
			jj = cf_dst+(j-cf_src);
			if (ii > rt_dst) {
				LOG("[%d][%d] out of from [%d][%d] to [%d][%d]\n",ii,jj,rf_dst,cf_dst,rt_dst,ct_dst);
				return false;
			}
			dst[ii+jj*w_dst] = (src)[(i+j*w_src)];
#if 0 //debug			
			if (dst[ii+jj*w_dst]) 
			LOG("dst[%d][%d] = src[%d][%d]:%f\n", ii, jj, i,j, dst[ii+jj*w_dst]);
#endif
		}
	}
#if 1 //debug	
	ofstream fos;
	try {
		string file_name = "C:\\src\\amo\\DIP\\Debug\\";
		char str[512];
		//sprintf(str, "copy_%dx%d_to_%dx%d_stamp%d.raw", w_src, h_src, w_dst, h_dst, (int)time(NULL)%10000);
		sprintf(str, "copy_%dx%d_to_%dx%d.raw", w_src, h_src, w_dst, h_dst);
		file_name += str;
		remove(file_name.c_str());
		fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			LOG("%s\n", strerror(errno));
			return false;
		}
		else fos.clear();
		fos.write((char*)dst, sizeof(double)*(double)w_dst*(double)h_dst);
		fos.close();
		LOG("[Copy image file]:%s\n", file_name.c_str());
	}
	catch (exception ex) {
		return false;
	}
#endif
	return true;
}

//[rf:rs:rt] like 0, 1, ... , 179 and [cf:cs:ct] like 0, 1, ... 79
bool Filter::copy(double* dst, int width_dst, int rf_dst, int rt_dst, int rs_dst, int cf_dst, int ct_dst, int cs_dst
	, double* src, int width_src, int rf_src, int rt_src, int rs_src, int cf_src, int ct_src, int cs_src) {
	if (dst == NULL || src == NULL || width_dst <= 0 || width_src <= 0)
		return false;
	int ii_dst = 0;
	int jj_dst = 0;
	int ii_src = 0;
	int jj_src = 0;
	for (jj_dst=cf_dst, jj_src=cf_src; jj_dst!=(ct_dst+cs_dst) && jj_src!=(ct_src+cs_src); jj_dst+=cs_dst, jj_src+=cs_src) {
		for  (ii_dst=rf_dst, ii_src=rf_src; ii_dst!=(rt_dst+rs_dst) && ii_src!=(rt_src+rs_src); ii_dst+=rs_dst, ii_src+=rs_src) {
			dst[ii_dst+jj_dst*width_dst] = src[ii_src+jj_src*width_src];
		}
	}
	return true;
}

static int func_cmp_histogram_equalization(const void *a, const void *b) {
	ImageKV* pix_a = (ImageKV*)a;
	ImageKV* pix_b = (ImageKV*)b;
	if (pix_a->val < pix_b->val)
		return -1;
	else if (pix_a->val > pix_b->val)
		return 1;
	else
		return 0;
}

bool Filter::histogram_equalization(double* src, int length) {
	if (length <= 0)
		return false;
	if (length < 2)
		return true;
	ImageKV* tmp = (ImageKV*)malloc(length*sizeof(ImageKV));
	if (tmp == NULL)
		return false;
	for (int i=0; i<length; i++) { 
		if (&src[i]==NULL)
			return false;
		if (&tmp[i]==NULL)
			return false;
		tmp[i].key = i;
		tmp[i].val = src[i];
	}
	qsort(tmp, length, sizeof(ImageKV), func_cmp_histogram_equalization);
	for (int i=0; i<length; i++) {
		src[tmp[i].key] = (double)i;
	}
	LOG("extended from [%f] to [%f]\n", 0, length-1);
	return true;
}

bool Filter::normalize(uint8_t*& dst, double* src, int width, int height, bool isAllPositive) {
	if (dst) {
		free(dst);
		dst = NULL;
	}
	dst = (uint8_t*)malloc(sizeof(uint8_t)*width*height);
	if (dst == NULL)
		return false;
	memset(dst, 0, sizeof(uint8_t)*width*height);
    double max   = std::numeric_limits<double>::min();
    double min   = std::numeric_limits<double>::max();
    double range = std::numeric_limits<double>::max();
    double norm  = 0.0;
    //find the boundary
    for (int j=0; j<height; j++) {
        for (int i=0; i<width; i++) { 
            if (src[i+j*width] > max) 
                max = src[i+j*width];
            else if (src[i+j*width] < min)
                min = src[i+j*width];
        }
    }
    //normalize double matrix to be an uint8_t matrix
    range = max - min;
	LOG("[Normalize for gray scale]max:%.1f, min:%.1f, range:%.1f\n", max, min, range);
	double dtmp = 0.0;
	double dtmp_norm = 0.0;
    for (int j=0; j<height; j++) {
        for (int i=0; i<width; i++) {
            norm = src[i+j*width];
			dtmp_norm = norm;
			norm = 255.0*(norm-min)/range;
			if (dtmp != dtmp_norm) {
				if (norm < 2 || norm > 254) {
					//LOG("[%d,%d]norm:%.1f to %.1f(%d)\n", i, j, dtmp_norm, norm, (uint8_t)norm);
				}
				dtmp = dtmp_norm;
			}
			dst[i+j*width] = (uint8_t)norm;
		}
    }
    return true;
}

bool Filter::normalize(double*& dst, double* src, int width, int height, double alpha, double beta) {
	if (alpha == 0 && beta == 1) {
		return normalize(dst, src, width, height, alpha, beta, true);
	}
	else {
		return normalize(dst, src, width, height, alpha, beta, false);
	}
	
}

//normType is NORM_MINMAX
bool Filter::normalize(double*& dst, double* src, int width, int height, double alpha, double beta, bool do_divide_by_sum) {
	if (dst) {
		free(dst);
		dst = NULL;
	}
	dst = (double*)malloc(sizeof(double)*width*height);
	if (dst == NULL)
		return false;
	memset(dst, 0, sizeof(double)*width*height);
    double max   = std::numeric_limits<double>::min();
    double min   = std::numeric_limits<double>::max();
    double range = std::numeric_limits<double>::max();
    double norm  = 0.0;
    double sum  = 0.0;
    //find the boundary
    for (int j=0; j<height; j++) {
        for (int i=0; i<width; i++) { 
            if (src[i+j*width] > max) 
                max = src[i+j*width];
            else if (src[i+j*width] < min)
                min = src[i+j*width];
        }
    }
    //normalize double matrix to be an normalized double matrix
    range = max - min;
	LOG("[Normalize for algorithm]max:%.1f, min:%.1f, range:%.1f\n", max, min, range);
	double dtmp = 0.0;
	double dtmp_norm = 0.0;
    for (int j=0; j<height; j++) {
        for (int i=0; i<width; i++) {
            norm = src[i+j*width];
			dtmp_norm = norm;
			norm = alpha + (beta-alpha)*(norm-min)/range;
			if (dtmp != dtmp_norm) {
				dtmp = dtmp_norm;
			}
			dst[i+j*width] = norm;
			sum  += dst[i+j*width];
		}
    }
	if (do_divide_by_sum) {
		LOG("[Normalize for kerne]element-divided by:%.1f\n", sum);
		for (int j=0; j<height; j++) {
			for (int i=0; i<width; i++) {
				dst[i+j*width] /= sum;
			}
		}
	}

#if 0 //debug
	ofstream fos;
	try {
		string file_name = "C:\\src\\amo\\DIP\\Debug\\normalized.raw";
		remove(file_name.c_str());
		fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			LOG("%s\n", strerror(errno));
			return false;
		}
		else fos.clear();
		fos.write((char*)dst, sizeof(double)*(double)width*(double)height);
		fos.close();
		LOG("[Normalized image file (%d x %d normalized to [%f,%f])]:%s\n", width, height, alpha, beta, file_name.c_str());
	}
	catch (exception ex) {
		return false;
	}
#endif
    return true;
}

bool Filter::cast(uint8_t*& dst, double* src, int width, int height) {
	if (dst) {
		free(dst);
		dst = NULL;
	}
	dst = (uint8_t*)malloc(sizeof(uint8_t)*width*height);
	if (dst == NULL)
		return false;
	memset(dst, 0, sizeof(uint8_t)*width*height);
    for (int j=0; j<height; j++) {
        for (int i=0; i<width; i++) { 
			dst[i+j*width] = (uint8_t)src[i+j*width];
        }
    }
    return true;
}

//correlation running with kernel without rotation 180 so no worry of even dimension aliasing
bool Filter::correlation(double*& dst, double* src, double* kernel, int width, int height, int window) {
	if (src == NULL || kernel == NULL)
		return false;
	if (width <= 0 || height <= 0 || width < window || height < window )
		return false;

#ifdef PRINT_FILTER //print
	LOG("--- source ---\n");
	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			printf("[");
			if (src[i+j*width] > -1) LOG(" ");
			if (src[i+j*width] < 100) LOG(" ");
			if (src[i+j*width] < 10) LOG(" ");
			printf("%.2f]", src[i+j*width]);
		}
		LOG("\n");
	}
	LOG("--- kernel ---\n");
	for (int j=0; j<window; j++) {
		for (int i=0; i<window; i++) {
			printf("[");
			if (kernel[i+j*window] > -1) LOG(" ");
			if (kernel[i+j*window] < 100) LOG(" ");
			if (kernel[i+j*window] < 10) LOG(" ");
			printf("%.2f]", kernel[i+j*window]);
		}
		LOG("\n");
	}
#endif
	
	if (dst) {
		free(dst);
		dst = NULL;
	}
	dst = (double*)malloc(sizeof(double)*width*height);
	if (dst == NULL)
		return false;
	memset(dst, 0, sizeof(double)*width*height);

	int ii = 0;
	int jj = 0;
	int nn = 0;
	int mm = 0;

	double max = std::numeric_limits<double>::min();
	double min = std::numeric_limits<double>::max();
	double range = std::numeric_limits<double>::max();

	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			for (int m=0; m<window; m++) {
				for (int n=0; n<window; n++) {
					ii = i+(n-window/2);
					jj = j+(m-window/2);
					nn = n;
					mm = m;
					if (ii >=0 && ii<width && jj>=0 && jj<height) {
						dst[i+j*width] += src[ii+jj*width]*kernel[nn+mm*window];
					}
					else {
						dst[i+j*width] += 0;
					}
				}
			}
			if (dst[i+j*width] > max) 
				max = dst[i+j*width];
			else if (dst[i+j*width] < min)
				min = dst[i+j*width];
		}
	}

	//normalize double matrix to be an uint8_t matrix
	range = max - min;
	double norm  = 0.0;
	LOG("correlated matrix max:%.1f, min:%.1f, range:%.1f\n", max, min, range);
	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			norm = dst[i+j*width];
			norm = 255.0*norm/range;
			dst[i+j*width] = norm;
		}
	}

#ifdef PRINT_FILTER //print
	LOG("--- convolution ---\n");
	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			printf("[");
			double d = dst[i+j*width];
			if (dst[i+j*width] < 0) {
				if (dst[i+j*width] > -100 ) LOG(" ");
				if (dst[i+j*width] > -10) LOG(" ");
			}
			else {
				LOG(" ");
				if (dst[i+j*width] < 100 ) LOG(" ");
				if (dst[i+j*width] < 10) LOG(" ");
			}
			printf("%.2f]", dst[i+j*width]);
		}
		LOG("\n");
	}
#endif
	return true;
}

bool Filter::convolution(double*& dst, double* src_assigned, double* kernel_assigned, int width_assigned, int height_assigned, int window_assigned) {
	LOG("[%s]Entry\n", __func__);
	if (src_assigned == NULL || kernel_assigned == NULL)
		return false;
	if (width_assigned <= 0 || height_assigned <= 0 || width_assigned < window_assigned || height_assigned < window_assigned )
		return false;

	int width  = width_assigned;
	int height = height_assigned;
	int window = window_assigned;
	int width_padded  = 0;
	int height_padded = 0;
	double* src           = src_assigned;
	double* src_cutoff    = NULL;
	double* kernel        = kernel_assigned;
	double* kernel_cutoff = NULL;
	double* dst_padded    = NULL;

	//Cut off to have odd dimension kernel and source if even size assigned
	int w_cutoff = 0;
	int h_cutoff = 0;
	int window_cutoff = 0;
	if (!(width%2)) {
		w_cutoff = 1; //hardcoded
		width -= w_cutoff;
	}
	if (!(height%2)) {
		h_cutoff = 1; //hardcoded
		height -= h_cutoff;
	}
	if (w_cutoff!=0 || h_cutoff!=0) {
		src_cutoff = (double*)malloc(sizeof(double)*width*height);
		if (src_cutoff == NULL)
			return false;
		memset(src_cutoff, 0, sizeof(double)*width*height);
		cutoff(src_cutoff, src_assigned, width_assigned, height_assigned, w_cutoff, h_cutoff, &width, &height);
		LOG("Cut off source [%d][%d] by %d rows and %d cols to be [%d][%d]\n"
				, width_assigned, height_assigned, w_cutoff, h_cutoff, width, height);
		src = src_cutoff;
	}

	if (!(window%2)) {
		window_cutoff = 1;
		window -= window_cutoff;
		kernel_cutoff = (double*)malloc(sizeof(double)*window*window);
		if (kernel_cutoff == NULL)
			return false;
		memset(kernel_cutoff, 0, sizeof(double)*width*height);
		LOG("Cut off kernel [%d][%d] by %d rows and %d cols to be [%d][%d]\n"
			, window_assigned, window_assigned, window_cutoff, window_cutoff, window, window);
		cutoff(kernel_cutoff, kernel_assigned, window_assigned, window_assigned, window_cutoff, window_cutoff, &window, &window);
		kernel = kernel_cutoff;
	}

#ifdef PRINT_FILTER //print
	LOG("--- source ---\n");
	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			printf("[");
			if (src[i+j*width] > -1) LOG(" ");
			if (src[i+j*width] < 100) LOG(" ");
			if (src[i+j*width] < 10) LOG(" ");
			LOG("%.1f]", src[i+j*width]);
		}
		LOG("\n");
	}
	LOG("--- kernel ---\n");
	for (int j=0; j<window; j++) {
		for (int i=0; i<window; i++) {
			printf("[");
			if (kernel[i+j*window] > -1) LOG(" ");
			if (kernel[i+j*window] < 100) LOG(" ");
			if (kernel[i+j*window] < 10) LOG(" ");
			LOG("%.1f]", kernel[i+j*window]);
		}
		LOG("\n");
	}
#endif
	
	if (dst) {
		free(dst);
		dst = NULL;
	}
	dst = (double*)malloc(sizeof(double)*width*height);
	if (dst == NULL)
		return false;
	memset(dst, 0, sizeof(double)*width*height);

	int ii = 0;
	int jj = 0;
	int nn = 0;
	int mm = 0;

	double max = std::numeric_limits<double>::min();
	double min = std::numeric_limits<double>::max();

	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			for (int m=0; m<window; m++) {
				for (int n=0; n<window; n++) {
					ii = i+(n-window/2);
					jj = j+(m-window/2);
					nn = (window-1)-n;
					mm = (window-1)-m;
					if (ii >=0 && ii<width && jj>=0 && jj<height && nn < window && mm < window) {
						dst[i+j*width] += src[ii+jj*width]*kernel[nn+mm*window];
						if (kernel[nn+mm*window] >0) {
							if (jj < 0)
								LOG("dst[%d][%d]+=src[%d][%d]:%.1f*kernel[%d][%d]:%.1f=%.1f\n"
									,i,j,ii,jj,src[ii+jj*width],nn,mm,kernel[nn+mm*window],dst[i+j*width]);
						}
					}
					else {
						dst[i+j*width] += 0;
					}
				}
			}
			if (dst[i+j*width] > max) 
				max = dst[i+j*width];
			else if (dst[i+j*width] < min)
				min = dst[i+j*width];
		}
	}
	LOG("convolution max:%.1f, min:%.1f\n", max, min);

	if (w_cutoff!=0 || h_cutoff!=0) {
		dst_padded = (double*)malloc(sizeof(double)*width_assigned*height_assigned);
		if (dst_padded == NULL)
			return dst_padded;
		LOG("Zero padding output by %d rows and %d cols\n", w_cutoff, h_cutoff);
		zeroPadding(dst_padded, dst, width, height, w_cutoff, h_cutoff, &width_padded, &height_padded);
		width = width_padded;
		height = height_padded;
		if (dst) {
			free(dst);
			dst = dst_padded;
		}
	}


#ifdef PRINT_FILTER //print
	LOG("--- convolution ---\n");
	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			printf("[");
			double d = dst[i+j*width];
			if (dst[i+j*width] < 0) {
				if (dst[i+j*width] > -100 ) LOG(" ");
				if (dst[i+j*width] > -10) LOG(" ");
			}
			else {
				LOG(" ");
				if (dst[i+j*width] < 100 ) LOG(" ");
				if (dst[i+j*width] < 10) LOG(" ");
			}
			LOG("%.1f]", dst[i+j*width]);
		}
		LOG("\n");
	}
#endif

	if (src_cutoff) {
		free(src_cutoff);
		src_cutoff = NULL;
	}
	if (kernel_cutoff) {
		free(kernel_cutoff);
		kernel_cutoff = NULL;
	}
	LOG("[%s]Exit\n", __func__);
}

bool Filter::convolutionZeroPadding(double*& dst, double* src, double* kernel, int width, int height, int window) {
	if (src == NULL || kernel == NULL)
		return false;
	if (width <= 0 || height <= 0 || width < window || height < window )
		return false;

	//zero padding
	int len_srcc = sizeof(double)*(width+2*(window-1))*(height+2*(window-1));
	double* srcc = (double*)malloc(len_srcc);
	if (srcc == NULL)
		return false;
	memset(srcc, 0, len_srcc);
	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			srcc[(i+(window-1))+(j+(window-1))*(width+2*(window-1))] = src[i+j*width];	
		}	
	}
	width += 2*(window-1);
	height += 2*(window-1);

#ifdef PRINT_FILTER //print
	LOG("--- source ---\n");
	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			printf("[");
			if (srcc[i+j*width] > -1) LOG(" ");
			if (srcc[i+j*width] < 100) LOG(" ");
			if (srcc[i+j*width] < 10) LOG(" ");
			printf("%.2f]", srcc[i+j*width]);
		}
		LOG("\n");
	}
	LOG("--- kernel ---\n");
	for (int j=0; j<window; j++) {
		for (int i=0; i<window; i++) {
			printf("[");
			if (kernel[i+j*window] > -1) LOG(" ");
			if (kernel[i+j*window] < 100) LOG(" ");
			if (kernel[i+j*window] < 10) LOG(" ");
			printf("%.2f]", kernel[i+j*window]);
		}
		LOG("\n");
	}
#endif

	if (dst) {
		free(dst);
		dst = NULL;
	}
	dst =(double*)malloc(sizeof(double)*width*height);
	if (dst == NULL)
		return false;
	memset(dst, 0, sizeof(double)*width*height);

	int ii = 0;
	int jj = 0;
	int nn = 0;
	int mm = 0;
	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			for (int m=0; m<window; m++) {
				for (int n=0; n<window; n++) {
#if 0 //im[1][2][3] .* h[3][2][1]
					ii = i-(window-1)+n;
					jj = j-(window-1)+m;
					nn = (window-1)-n;
					mm = (window-1)-m;
#else //im[3][2][1] .* h[1][2][3]
					ii = i-n;
					jj = j-m;
					nn = n;
					mm = m;
#endif
					if (ii >=0 && ii<width && jj>=0 && jj<height) {
						//LOG("dst[%d][%d]+=srcc[%d][%d]*kernel[%d][%d]\n",i,j,ii,jj,nn,mm);
						//LOG("dst[%d][%d]+=%.f*%.f\n",i,j,srcc[ii+jj*width],kernel[nn+mm*window]);
						dst[i+j*width] += srcc[ii+jj*width]*kernel[nn+mm*window];
					}
					else {
						dst[i+j*width] += 0;
					}
				}
			}
		}
	}

#ifdef PRINT_FILTER //print
	LOG("--- convolution with zero padding ---\n");
	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			printf("[");
			double d = dst[i+j*width];
			if (dst[i+j*width] < 0) {
				if (dst[i+j*width] > -100 ) LOG(" ");
				if (dst[i+j*width] > -10) LOG(" ");
			}
			else {
				LOG(" ");
				if (dst[i+j*width] < 100 ) LOG(" ");
				if (dst[i+j*width] < 10) LOG(" ");
			}
			printf("%.2f]", dst[i+j*width]);
		}
		LOG("\n");
	}
#endif
	return true;
}

bool Filter::sobel(double*& dst, double* src, int width, int height, string file) {
	if (src == NULL)
		return false;
	if (width <= 0 || height <= 0)
		return false;

	double edge_vertical[3*3]   = { 1.0, 0.0, -1.0,
								    2.0, 0.0, -2.0,
								    1.0, 0.0, -1.0};
	double edge_horizontal[3*3] = { 1.0, 2.0,  1.0,
								    0.0, 0.0,  0.0,
								   -1.0,-2.0, -1.0};

#if 0 //h_horizontal convolved by h_vertical ?
	double *edge_hypotenuse = NULL;
	int window = 3;
	this->convolution(edge_hypotenuse, edge_vertical, edge_horizontal, 3, 3, window);
	if (edge_hypotenuse == NULL) {
		return false;
	}
	else {
		for (int j=0; j<window; j++) {
			for (int i=0; i<window; i++) {
				LOG("[%.1f]", edge_hypotenuse[i+j*window]);
			}
			LOG("\n");
		}
	}
	//this->convolution(dst, src, edge_hypotenuse, width, height, 3);
#endif

	double *Gx = NULL;
	double *Gy = NULL;
	
	this->convolution(Gx, src, edge_horizontal, width, height, 3);
	this->convolution(Gy, src, edge_vertical, width, height, 3);

	if (Gx == NULL || Gy == NULL) 
		return false;
	
	if (dst) {
		free(dst);
		dst = NULL;
	}
	dst = (double*)malloc(sizeof(double)*width*height);
	if (dst == NULL)
		return false;
	memset(dst, 0, sizeof(double)*width*height);
	double gx = 0.0;
	double gy = 0.0;
	double gm = 0.0; 
	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			gx = pow(Gx[i+j*width],2);
			gy = pow(Gy[i+j*width],2);
			gm = sqrt(pow(Gx[i+j*width],2)+pow(Gy[i+j*width],2));
			if (gm >= 255.0) 
				gm = 255.0;
			//dst[i+j*width] = sqrt(pow(Gx[i+j*width],2)+pow(Gy[i+j*width],2));
			dst[i+j*width] = gm;
		}
	}

	if (file.empty())
		return true;
	ofstream fos;
	try {
		string file_name = "";
		string file_name_path = file.substr(0, file.find_last_of("/\\"));
		string file_name_file = file.substr(file.find_last_of("/\\"));
		string file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "SobelFiltered";
		file_name += file_name_path;
		file_name += file_name_file_to_matlab;
		file_name += "";
		remove(file_name.c_str());
		fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			LOG("%s\n", strerror(errno));
			return false;
		}
		else fos.clear();
		uint8_t c = NULL;
		for (int j=0; j<height; j++) {
			for (int i=0; i<width; i++) {
				c = (uint8_t) (*(dst+(i+j*width)));
				fos.write((char*)&c, sizeof(char));	
			}
		}
		//fos.write((char*)dst, width*height*sizeof(uint8_t));
		fos.close();
		LOG("[Soble filtered file]:%s\n", file_name.c_str());
	}
	catch (exception ex) {
		return false;
	}
	return true;
}

bool Filter::gaussian(double*& dst, double* src, int width, int height, string file) {
	if (src == NULL)
		return false;
	if (width <= 0 || height <= 0)
		return false;

	double kernel_Gaussian[3*3] = { 1.0/16.0, 1.0/8.0, 1.0/16.0,
									1.0/8.0 , 1.0/4.0, 1.0/8.0,
									1.0/16.0, 1.0/8.0, 1.0/16.0};
	this->convolution(dst, src, kernel_Gaussian, width, height, 3);
	//this->correlation(dst, src, kernel_Gaussian, width, height, 3);

	if (file.empty())
		return true;
	ofstream fos;
	try {
		string file_name = "";
		string file_name_path = file.substr(0, file.find_last_of("/\\"));
		string file_name_file = file.substr(file.find_last_of("/\\"));
		string file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "Gaussianed";
		file_name += file_name_path;
		file_name += file_name_file_to_matlab;
		file_name += "";
		remove(file_name.c_str());
		fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			LOG("%s\n", strerror(errno));
			return false;
		}
		else fos.clear();
		uint8_t *output = NULL;
		int output_len = sizeof(uint8_t)*width*height;
		//normalize(output, dst, width, height, true);
		cast(output, dst, width, height);
		fos.write((char*)output, output_len);
		fos.close();
		LOG("[Gaussian filtered file]:%s\n", file_name.c_str());
	}
	catch (exception ex) {
		return false;
	}
	return true;
}

bool Filter::gradient(double*& magnitude, double*& orientation, double* src, int width, int height, string file) {
	if (src == NULL)
		return false;
	if (width <= 0 || height <= 0)
		return false;

#if 0
	double gradient_x_correlation[3*3] = {-0.5, 0.0, 0.5,
										  -0.5, 0.0, 0.5,
										  -0.5, 0.0, 0.5};
	double gradient_y_correlation[3*3] = {-0.5,-0.5,-0.5,
										   0.0, 0.0, 0.0,
										   0.5, 0.5, 0.5};
	double gradient_x_convolution[3*3] = { 0.5, 0.0,-0.5,
										   0.5, 0.0,-0.5,
										   0.5, 0.0,-0.5};
	double gradient_y_convolution[3*3] = { 0.5, 0.5, 0.5,
										   0.0, 0.0, 0.0,
										  -0.5,-0.5,-0.5};
#elif 1 //prewitt
	double gradient_x_correlation[3*3] = {-1.0, 0.0, 1.0,
										  -1.0, 0.0, 1.0,
										  -1.0, 0.0, 1.0};
	double gradient_y_correlation[3*3] = {-1.0,-1.0,-1.0,
										   0.0, 0.0, 0.0,
										   1.0, 1.0, 1.0};

#else
	double gradient_x_correlation[3*3] = {-0.5, 0.0, 0.5,
										  -0.5, 0.0, 0.5,
										  -0.5, 0.0, 0.5};
	double gradient_y_correlation[3*3] = {-0.5,-0.5,-0.5,
										   0.0, 0.0, 0.0,
										   0.5, 0.5, 0.5};
#endif

	double *Gx = NULL;
	double *Gy = NULL;
	
#if 0
	this->convolution(Gx, src, gradient_x_convolution, width, height, 3);
	this->convolution(Gy, src, gradient_y_convolution, width, height, 3);
#else	
	this->correlation(Gx, src, gradient_x_correlation, width, height, 3);
	this->correlation(Gy, src, gradient_y_correlation, width, height, 3);
#endif

#if 0
	LOG("GX:\n");
	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			if (Gx[i+j*width]!=0)
			LOG("[(%d,%d):%.f]", i, j, Gx[i+j*width]);
		}
		LOG("\n");
	}
#endif

	if (Gx == NULL || Gy == NULL) 
		return false;
	
	//magnitude
	magnitude = new double[sizeof(double)*width*height];
	if (magnitude == NULL)
		return false;
	memset(magnitude, 0, sizeof(double)*width*height);
	double gx = 0.0;
	double gy = 0.0;
	double gm = 0.0; 
	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			gx = pow(Gx[i+j*width],2);
			gy = pow(Gy[i+j*width],2);
			gm = sqrt(pow(Gx[i+j*width],2)+pow(Gy[i+j*width],2));
			if (gm >= 255.0) {
				return false;
			}
			magnitude[i+j*width] = gm;
		}
	}

	//orientation
	orientation = new double[sizeof(double)*width*height];
	if (orientation == NULL)
		return false;
	memset(orientation, 0, sizeof(double)*width*height);
	double ori = 0.0;
	//orientation normalize
	///////////////////////////////
	// Quadrants of image:
	// 3(-dx,-dy) | 4(+dx,-dy)      [-pi,0]
	// ------------------------->x
	// 2(-dx,+dy) | 1(+dx,+dy)      [0,pi]
	//            v
	//            y
	// Definition of arctan2():
	// -135(-dx,-dy) | -45(+dx,-dy)
	// ------------------------->x
	//  135(-dx,+dy) | +45(+dx,+dy)
	//               v
	//               y
	///////////////////////////////
	//[-pi,0) to be [128,255] 
	//-45.0   to be 223.1
	//-90.0   to be 191.2
	//-135.0  to be 159.4
	//-153.4  to be 146.3
	//[0,+pi] to be [0,127]
	// 45.0   to be 31.9
	// 90.0   to be 63.8
	// 135.0  to be 95.6
	// 180.0  to be 127.5
	///////////////////////////////
	double dtmp = 0.0;
	double ori_normalized = 0.0;
	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			gx = (Gx[i+j*width]);
			gy = (Gy[i+j*width]);
			ori = atan2(Gy[i+j*width], Gx[i+j*width])/PI*(180.0); //[-pi,+pi]
			if (gx >= 0 && gy >= 0) { //[Qudrant 1]:[0,90] to be [0,63] 
				if (ori < 0) {
					 LOG("[Err1QUA]ori:%.1f\n", ori);
					return false;
				}
				ori_normalized = (ori)*255.0/360.0; 
				if (ori != 0.0 && dtmp != ori) {
					//LOG("[Qudrant 1]orientation: %.1f to be %.1f(%d)\n", ori, ori_normalized, (uint8_t)ori_normalized);
					dtmp = ori;
				}
			}
			else if (gx >= 0 && gy < 0) { //[Qudrant 4]:[270,360) equal to [-90, 0) to be [191,255]
				if (ori > 0) {
					LOG("[Err4QUA]orientation:%.1f\n", ori);
					return false;
				}
				ori_normalized = (360.0+ori)*255.0/360.0;
				if (ori != 0.0 && dtmp != ori) {
					//LOG("[Qudrant 4]orientation:%.1f to be %.1f(%d)\n", ori, ori_normalized, (uint8_t)ori_normalized);
					dtmp = ori;
				}
			}
			else if (gx < 0 && gy >= 0) { //[Qudrant 2]:(90,180] to be [64,127]
				if (ori < 0) {
					LOG("[Err2QUA]orientation:%.1f\n", ori);
					return false;
				}
				ori_normalized = (ori)*255.0/360.0; 
				if (ori != 0.0 && dtmp != ori) {
					//LOG("[Qudrant 2]orientation: %.1f to be %.1f(%d)\n", ori, ori_normalized, (uint8_t)ori_normalized);
					dtmp = ori;
				}
			}
			else if (gx < 0 && gy < 0) { //[Qudrant 3]:(180,270) equal to (-180, -90) to be [128,190] 
				if (ori > 0) {
					LOG("[Err3QUA]orientation:%.1f\n", ori);
					return false;
				}
				ori_normalized = (360.0+ori)*255.0/360.0; 
				if (ori != 0.0 && dtmp != ori) { 
					//LOG("[Qudrant 3]orientation:%.1f to be %.1f(%d)\n", ori, ori_normalized, (uint8_t)ori_normalized);
					dtmp = ori;
				}
			}
			else {
				LOG("[EXCEPTION]orientation:%.1f\n", ori);
				return false;
			}
			orientation[i+j*width] = ori_normalized;
		}
	}

#if 0
	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			gx = (Gx[i+j*width]);
			gy = (Gy[i+j*width]);
			ori = atan2(Gy[i+j*width], Gx[i+j*width])/PI*(180.0); //[-pi,+pi]
			if (ori < 0) {
				ori_normalized = (360.0+ori)*255.0/360.0; //[-pi,0) to be [127,255] 
				//if (ori != 0.0 && ori > -90.0) LOG("ori:%.1f to be %.1f\n", ori, ori_normalized);
			}
			else {
				ori_normalized = (ori)*255.0/360.0; //[0,+pi] to [0,126]
				//if (ori != 0.0 && ori < 90.0) LOG("ori:%.1f to be %.1f\n", ori, ori_normalized);
			}
			orientation[i+j*width] = ori_normalized;
		}
	}

#endif 

	if (file.empty())
		return true;
	ofstream fos;
	try {
		string file_name = "";
		string file_name_path = file.substr(0, file.find_last_of("/\\"));
		string file_name_file = file.substr(file.find_last_of("/\\"));
		string file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "GradientMAG";
		file_name += file_name_path;
		file_name += file_name_file_to_matlab;
		file_name += "";
		remove(file_name.c_str());
		fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			LOG("%s\n", strerror(errno));
			return false;
		}
		else fos.clear();
		uint8_t *output = NULL;
		int output_len = sizeof(uint8_t)*width*height;
		normalize(output, magnitude, width, height, true);
		fos.write((char*)output, output_len);
		if (output) { 
			delete[](output);
			output = NULL;
		}
		fos.close();
		LOG("[Gradient magnitude output file]:%s\n", file_name.c_str());

		file_name = "";
		file_name_path = file.substr(0, file.find_last_of("/\\"));
		file_name_file = file.substr(file.find_last_of("/\\"));
		file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "GradientORI";
		file_name += file_name_path;
		file_name += file_name_file_to_matlab;
		file_name += "";
		remove(file_name.c_str());
		fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			LOG("%s\n", strerror(errno));
			return false;
		}
		else fos.clear();
		output = NULL;
		output_len = sizeof(uint8_t)*width*height;
		//normalize(output, orientation, width, height, true);
		cast(output, orientation, width, height);
		fos.write((char*)output, output_len);
		if (output) { 
			delete[](output);
			output = NULL;
		}
		fos.close();
		LOG("[Gradient orientation output file]:%s\n", file_name.c_str());

		file_name = "";
		file_name_path = file.substr(0, file.find_last_of("/\\"));
		file_name_file = file.substr(file.find_last_of("/\\"));
		file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "GX";
		file_name += file_name_path;
		file_name += file_name_file_to_matlab;
		file_name += "";
		remove(file_name.c_str());
		fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			LOG("%s\n", strerror(errno));
			return false;
		}
		else fos.clear();
		output = NULL;
		output_len = sizeof(uint8_t)*width*height;
		normalize(output, Gx, width, height, true); //normailzed Gx:-127.5 to 0.0(0), 0.0 to 127.5(127), 127.5 to 255.0(255)
		fos.write((char*)output, output_len);
		if (output) { 
			delete[](output);
			output = NULL;
		}
		fos.close();
		LOG("[Gradient x output file]:%s\n", file_name.c_str());

		file_name = "";
		file_name_path = file.substr(0, file.find_last_of("/\\"));
		file_name_file = file.substr(file.find_last_of("/\\"));
		file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "GY";
		file_name += file_name_path;
		file_name += file_name_file_to_matlab;
		file_name += "";
		remove(file_name.c_str());
		fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			LOG("%s\n", strerror(errno));
			return false;
		}
		else fos.clear();
		output = NULL;
		output_len = sizeof(uint8_t)*width*height;
		normalize(output, Gy, width, height, true);
		fos.write((char*)output, output_len);
		if (output) 
			delete[](output);
		fos.close();
		LOG("[Gradient y output file]:%s\n", file_name.c_str());
	}
	catch (exception ex) {
		return false;
	}

	if (Gx)
		delete[](Gx);
	if (Gy)
		delete[](Gy);
	return true;
}

bool Filter::laplacian(double*& magnitude, double* src, int width, int height, string file) {
	if (src == NULL)
		return false;
	if (width <= 0 || height <= 0)
		return false;

	double laplacian_low[3*3] = {0.0, 1.0, 0.0,
			  					 1.0,-4.0, 1.0,
							     0.0, 1.0, 0.0};
	
	double laplacian_high[3*3] = {1.0, 1.0, 1.0,
			  				 	  1.0,-8.0, 1.0,
							      1.0, 1.0, 1.0};

	//this->correlation(magnitude, src, laplacian_high, width, height, 3);
	this->correlation(magnitude, src, laplacian_low, width, height, 3);
	if (magnitude == NULL) 
		return false;

	if (file.empty())
		return true;
	ofstream fos;
	try {
		string file_name = "";
		string file_name_path = file.substr(0, file.find_last_of("/\\"));
		string file_name_file = file.substr(file.find_last_of("/\\"));
		string file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "Laplacianed";
		file_name += file_name_path;
		file_name += file_name_file_to_matlab;
		file_name += "";
		remove(file_name.c_str());
		fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			LOG("%s\n", strerror(errno));
			return false;
		}
		else fos.clear();
		uint8_t *output = NULL;
		int output_len = sizeof(uint8_t)*width*height;
		//normalize(output, magnitude, width, height, true);
		cast(output, magnitude, width, height);
		fos.write((char*)output, output_len);
		LOG("[Laplacianed output file]:%s\n", file_name.c_str());
		if (output) { 
			delete[](output);
			output = NULL;
		}
	}
	catch (exception ex) {
		return false;
	}
	return true;
}

bool Filter::deconvolution_wiener(uint8_t** reconstructed, complex<double>* I, complex<double>* H, int width, int height, int window, double nsr, string file, bool is_H_shifted_to_center) {
	LOG("[%s]Entry\n", __func__);
	if (I == NULL || H == NULL)
		return false;
	if (width <= 0 || height <= 0)
		return false;
	
	if (*reconstructed != NULL) {
		free(*reconstructed);
		*reconstructed = NULL;
	}

	*reconstructed = (uint8_t*)malloc(sizeof(uint8_t)*width*height);
	if (*reconstructed == NULL)
		return false;
	memset(*reconstructed, 0, sizeof(uint8_t)*width*height);

	complex<double>* G = (complex<double>*)malloc(sizeof(complex<double>)*width*height);
	if (G == NULL)
		return false;
	memset(G, 0, sizeof(complex<double>)*width*height);

	complex<double> c_recst(0, 0);
	complex<double> c_conjH(0, 0);
	complex<double> c_I(0, 0);
	double factor = 0.0; 
	double sqH = 0.0;

	for (int v=0; v<height; v++) {
		for (int u=0; u<width; u++) {
#if 1   //G=I.*(H*).*(1/(H^2+nsr)) which is wiener
			sqH = H[u+v*width]._Val[0]*H[u+v*width]._Val[0] + H[u+v*width]._Val[1]*H[u+v*width]._Val[1];
			factor = 1.0f/(sqH+nsr);
			c_conjH = complex<double>(H[u+v*width]._Val[0], -1.0f*(H[u+v*width]._Val[1]));
			c_I     = I[u+v*width];
			c_recst = (c_conjH * c_I) * factor;
			G[u+v*width] = (complex<double>)c_recst;
			if (v < 0) {
				LOG("[%d][%d]:[c_conjH]:(%.2f,%.2f), [c_I]:(%.2f,%.2f), [c_recst]:(%.2f,%.2f)\n", u, v
					, c_conjH._Val[0], c_conjH._Val[1]
					, c_I._Val[0], c_I._Val[1]
					, c_recst._Val[0], c_recst._Val[1]);
			}
#elif 0 //G=I.*(1/H) which is naive deconv
			if (H[u+v*width]._Val[0] == 0 && H[u+v*width]._Val[1] == 0) {
				if (I[u+v*width]._Val[0] == 0 && I[u+v*width]._Val[1] == 0) {
					G[u+v*width] = 0;
				} 
				else {
					c_recst = I[u+v*width];
					G[u+v*width] = c_recst;
				}
			}
			else {
				c_I._Val[0]     = I[u+v*width]._Val[0];
				c_I._Val[1]     = I[u+v*width]._Val[1];
				c_recst = c_I / H[u+v*width];
				G[u+v*width] = c_recst;
			}
#else   //G = I or H
			c_recst._Val[0] = H[u+v*width]._Val[0];
			c_recst._Val[1] = H[u+v*width]._Val[1];
			//c_recst = I[u+v*width];
			G[u+v*width] = c_recst;
#endif
		}
	}

	LOG("DFT SPCTRUM of G...\n");
	double w;
	double h;
	DFT dft(width, height, G);
	double *spec = NULL;
	dft.spectrum(spec, &w, &h, false);

	LOG("Real of G...\n");
	double* G_real = (double*)malloc(sizeof(double)*width*height);
	for (int v=0; v<height; v++) {
		for (int u=0; u<width; u++) {
			G_real[u+v*width] = (double)(G[u+v*width])._Val[0];
		}
	}

	LOG("IDFT of G...\n");
	double *output_idft = NULL;
	dft.idft2(output_idft, &w, &h, window, true);
	uint8_t* output = NULL;
	if (output_idft == NULL)
		return false;

	Filter filter;
#if 1
	if (!(filter.normalize(output, output_idft, w, h, true))) {
		return false;
	}
#else
	if (!(filter.cast(output, output_idft, w, h))) {
		return false;
	}
#endif

	memcpy(*reconstructed, output, sizeof(uint8_t)*w*h);
	if (is_H_shifted_to_center) {
		DFT dft_sft;
		dft_sft.fftshift(*reconstructed, w, h);
	}
	if (file.empty())
		return true;
	ofstream fos;
	try {
		string file_name = "";
		string file_name_path = file.substr(0, file.find_last_of("/\\"));
		string file_name_file = file.substr(file.find_last_of("/\\"));
		string file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "Deconv.raw";
		file_name += file_name_path;
		file_name += file_name_file_to_matlab;
		file_name += "";
		remove(file_name.c_str());
		fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			LOG("%s\n", strerror(errno));
			return false;
		}
		else fos.clear();
		fos.write((char*)(*reconstructed), sizeof(uint8_t)*w*h);
		fos.close();
		LOG("[IDFT of G file]:%s\n", file_name.c_str());	

		file_name = "";
		file_name_path = file.substr(0, file.find_last_of("/\\"));
		file_name_file = file.substr(file.find_last_of("/\\"));
		file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "G_Spec.raw";
		file_name += file_name_path;
		file_name += file_name_file_to_matlab;
		file_name += "";
		remove(file_name.c_str());
		fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			LOG("%s\n", strerror(errno));
			return false;
		}
		else fos.clear();
		if (spec)
			fos.write((char*)spec, sizeof(double)*(double)width*(double)height);
		fos.close();
		LOG("[DFT spec of G file]:%s\n", file_name.c_str());

		file_name = "";
		file_name_path = file.substr(0, file.find_last_of("/\\"));
		file_name_file = file.substr(file.find_last_of("/\\"));
		file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "G_Real";
		file_name += file_name_path;
		file_name += file_name_file_to_matlab;
		file_name += ".raw";
		remove(file_name.c_str());
		fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			LOG("%s\n", strerror(errno));
			return false;
		}
		else fos.clear();
		if (G_real)
			fos.write((char*)G_real, sizeof(double)*(double)width*(double)height);
		fos.close();
		LOG("[DFT real of G file]:%s\n", file_name.c_str());
	}
	catch (exception ex) {
		return false;
	}

	if (G)
		free(G);
		G = NULL;
	if (G_real)
		free(G_real);
		G_real = NULL;
	if (output)
		free(output);
	if (output_idft)
		free(output_idft);
	if (spec)
		free(spec);

	LOG("[%s]Exit\n", __func__);
	return true;
}

bool Filter::deconvolution_dct(uint8_t** reconstructed, double* I, double* H, int width, int height, int window, int group, double nsr, string file) {
	LOG("[%s]Entry\n", __func__);
	if (I == NULL || H == NULL)
		return false;
	if (width <= 0 || height <= 0)
		return false;
	
	if (*reconstructed != NULL) {
		free(*reconstructed);
		*reconstructed = NULL;
	}

	*reconstructed = (uint8_t*)malloc(sizeof(uint8_t)*width*height);
	if (*reconstructed == NULL)
		return false;
	memset(*reconstructed, 0, sizeof(uint8_t)*width*height);

	double* G = (double*)malloc(sizeof(double)*width*height);
	if (G == NULL)
		return false;
	memset(G, 0, sizeof(double)*width*height);

	double c_recst = 0.0;
	double c_H     = 0.0;
	double c_I     = 0.0;
	double factor  = 0.0; 
	double sqH     = 0.0;

#if 0 //debug
	double H_clone[80*80];
	memcpy(H_clone, H, sizeof(double)*80*80);
	double I_clone[80*80];
	memcpy(I_clone, I, sizeof(double)*80*80);
#endif

	for (int v=0; v<height; v++) {
		for (int u=0; u<width; u++) {
#if 1   //G=I.*(H*).*(1/(H^2+nsr)) which is wiener
			sqH    = H[u+v*width]*H[u+v*width];
			factor = 1.0f/(sqH+nsr);
			c_H    = H[u+v*width];
			c_I    = I[u+v*width];
			//c_recst = (c_I) / (c_H) * factor;
			c_recst = (c_I) / (c_H);
			G[u+v*width] = c_recst;
			if (v < 0) {
				LOG("[%d][%d]:[c_H]:(%.2f), [c_I]:(%.2f), [c_recst]:(%.2f)\n", u, v, c_H, c_I, c_recst);
			}
#else   //G = I or H
			//c_recst = H[u+v*width];
			c_recst = I[u+v*width];
			G[u+v*width] = c_recst;
#endif
		}
	}

	LOG("DCT SPCTRUM of G...\n");
	double w;
	double h;
	DCT dct(width, height, G);
	double *spec = NULL;
	dct.spectrum(&spec, &w, &h, false);

	LOG("IDCT of G...\n");
	double *output_idct = NULL;
	dct.idct2(output_idct, &w, &h, window, group);
	uint8_t* output = NULL;
	if (output_idct == NULL)
		return false;

	Filter filter;
#if 1
	if (!(filter.normalize(output, output_idct, w, h, true))) {
		return false;
	}
#else
	if (!(filter.cast(output, output_idct, w, h))) {
		return false;
	}
#endif

	//copy reconstruction to outside callee
	memcpy(*reconstructed, output, sizeof(uint8_t)*w*h);

	if (file.empty())
		return true;
	ofstream fos;
	try {
		string file_name = "";
		string file_name_path = file.substr(0, file.find_last_of("/\\"));
		string file_name_file = file.substr(file.find_last_of("/\\"));
		string file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "Deconv_byDCT.raw";
		file_name += file_name_path;
		file_name += file_name_file_to_matlab;
		file_name += "";
		remove(file_name.c_str());
		fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			LOG("%s\n", strerror(errno));
			return false;
		}
		else fos.clear();
		fos.write((char*)(*reconstructed), sizeof(uint8_t)*w*h);
		fos.close();
		LOG("[IDFT of G file]:%s\n", file_name.c_str());	

		file_name = "";
		file_name_path = file.substr(0, file.find_last_of("/\\"));
		file_name_file = file.substr(file.find_last_of("/\\"));
		file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "G_Spec_byDCT.raw";
		file_name += file_name_path;
		file_name += file_name_file_to_matlab;
		file_name += "";
		remove(file_name.c_str());
		fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			LOG("%s\n", strerror(errno));
			return false;
		}
		else fos.clear();
		if (spec)
			fos.write((char*)spec, sizeof(double)*(double)width*(double)height);
		fos.close();
		LOG("[DFT spec of G file]:%s\n", file_name.c_str());
	}
	catch (exception ex) {
		return false;
	}

	if (G)
		free(G);
		G = NULL;
	if (output)
		free(output);
	if (output_idct)
		free(output_idct);
	if (spec)
		free(spec);


	LOG("[%s]Exit\n", __func__);
	return true;
}

bool Filter::circle(uint8_t** dst, double** kernel, int w_circle, int h_circle, int r_circle, int window, int w_img, int h_img
	, complex<double>** H, double** Y, bool is_normalize_and_element_wise_divided_by_sum, bool is_inverse_psf, bool do_DCT) {
	if (w_img <= 0 || h_img <= 0 || w_circle <= 0 || h_circle <= 0 || r_circle <= 0)
		return false;
	uint8_t* circle = (uint8_t*)malloc(sizeof(uint8_t)*w_circle*h_circle);

	double centerX = 0;
	double centerY = 0;
	double delX = 0;
	double delY = 0;
	double delR = 0;
	int p = 0;
	double scale = 0;
	double decay = 0;
	double period = r_circle;
	string file_name = "";

	double* h = NULL;
	double* h_normed = NULL;
	
	if (do_DCT) {
		centerX = 0;
		centerY = 0;
	}
	else {
		centerX = w_circle/2;
		centerY = h_circle/2;	
	}
	
	for (int j=0; j<h_circle; j++) {
		for (int i=0; i<w_circle; i++) {
			p = i+j*w_circle;
			delX = abs(i-centerX);
			delY = abs(j-centerY);
			delR = sqrt(pow(delX,2) + pow(delY,2));
			if (delR <= r_circle) {
				if (is_inverse_psf) {
					scale = (sin(2*PI*(delR/(r_circle/1.0))))+abs(sin(1*PI));
				}
				else {
					scale = (cos(2*PI*(delR/(r_circle/1.0))))+abs(cos(1*PI));
				}
				decay = abs(sin(2*PI*delR/(r_circle*2.0))); //ult-s psf
				//decay = abs(sin(2*PI*delR/(r_circle*3.0))); //normal psf
				if (decay>1.0) decay = 1.0;
				if (decay<0.0) decay = 0.0;
				circle[p] = (uint8_t)255.0f*(1.0-decay)*(scale);
				//circle[p] = (uint8_t)255.0f*(1.0)*(scale);
				LOG("delR:%f, at [%d][%d], decay:%f, scale:%f, circle:%d\n", delR, i,j, decay, scale, circle[p]);
			}
			else {
				circle[p] = 0x0;
			}
		}
	}

	ofstream fos;
	try {
		string file_name = "C:\\src\\amo\\DIP\\Debug\\circle";
		if (is_inverse_psf)
			file_name += "_inverse";
		file_name += ".raw";
		remove(file_name.c_str());
		fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			LOG("%s\n", strerror(errno));
			return false;
		}
		else fos.clear();
		uint8_t *output = NULL;
		fos.write((char*)circle, sizeof(uint8_t)*(double)w_circle*(double)h_circle);
		fos.close();
		LOG("[Circle file (%dx%d)]:%s\n", w_circle, h_circle, file_name.c_str());
	}
	catch (exception ex) {
		if (circle)
			free(circle);
		return false;
	}
	
	//
	//exit if no need to dive into DFT or DCT
	//
	if (H==NULL && Y==NULL && dst==NULL && kernel == NULL) { 
		if (circle)
			free(circle);
			LOG("Exit without developing job\n");
		return true;
	}

	//copy circle raw image to callee
	if (dst != NULL) {
		LOG("Copy circle raw image to outside...\n");
		if (*dst != NULL) {
			memset(*dst, 0, sizeof(uint8_t)*w_img*h_img);
			for (int j=0; j<h_circle; j++) {
				for (int i=0; i<w_circle; i++) {
					(*dst)[i+j*w_img] = circle[i+j*w_circle];
				}
			}
		}
		else {
			return false;
		} 
		file_name = "C:\\src\\amo\\DIP\\Debug\\circle_img";
		if (is_inverse_psf)
			file_name += "_inverse";
		file_name += ".raw";
		remove(file_name.c_str());
		fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			LOG("%s\n", strerror(errno));
			return false;
		}
		else fos.clear();
		if (dst && *dst)
			fos.write((char*)(*dst), sizeof(char)*(double)w_img*(double)h_img);
		fos.close();
		LOG("[Circle image file (%d x %d)]:%s\n", w_img, h_img, file_name.c_str());
	}

	//make h which is the kernel
	LOG("Make kernel with sum of one...\n");
	h = (double*) malloc(sizeof(double)*w_img*h_img);
	h_normed = NULL;
	for (int p = 0; p<w_img*h_img; p++) {
		if (&((circle)[p]) != NULL) {
			h[p] = (double)circle[p];
		}
	}
	
	//provide H for DFT 
	if (H != NULL) {
		LOG("Make H for DFT...\n");
		if (is_normalize_and_element_wise_divided_by_sum) { //usaully for fft of algo. kernel
			normalize(h_normed, h, w_img, h_img, 0, 1); 
			if (h_normed == NULL || h == NULL) return false;
			memcpy(h, h_normed, sizeof(double)*w_img*h_img);
			free(h_normed);
			h_normed = NULL;
		}
	
		if (kernel != NULL) {
			if (*kernel != NULL) {
				free(*kernel);
				*kernel = NULL;
			}
			*kernel = (double*) malloc(sizeof(double)*w_img*h_img);
			memcpy(*kernel, h, sizeof(double)*w_img*h_img);
		}

		LOG("DFT of h... (N:%d, M:%d)\n", w_img, h_img);
		DFT dft;
		dft.dft2(H, h, w_img, h_img, window);

		double* spec = NULL;
		double w_spec;
		double h_spec;
		dft.spectrum(spec, &w_spec, &h_spec, false);

		try {
			file_name = "C:\\src\\amo\\DIP\\Debug\\circle_DFTSpec";
			if (is_inverse_psf)
				file_name += "_inverse";
			file_name += ".raw";
			remove(file_name.c_str());
			fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
			if (fos.fail()) {
				LOG("%s\n", strerror(errno));
				return false;
			}
			else fos.clear();
			if (spec)
				fos.write((char*)spec, w_spec*h_spec*sizeof(double));
			fos.close();
			LOG("[DFT Spec of circle image file]:%s\n", file_name.c_str());
		}
		catch (exception ex) {
			return false;
		}
		if (spec)
			free(spec);
	}

	//provide Y for DCT 
	if (Y != NULL) {
		LOG("Make H for DCT...\n");
		if (is_normalize_and_element_wise_divided_by_sum) { //usaully for fft of algo. kernel
			normalize(h_normed, h, w_img, h_img, 0, 1); 
			if (h_normed == NULL || h == NULL) return false;
			memcpy(h, h_normed, sizeof(double)*w_img*h_img);
			free(h_normed);
			h_normed = NULL;
		}
	
		if (kernel != NULL) {
			if (*kernel != NULL) {
				free(*kernel);
				*kernel = NULL;
			}
			*kernel = (double*) malloc(sizeof(double)*w_img*h_img);
			memcpy(*kernel, h, sizeof(double)*w_img*h_img);
		}

		LOG("DCT of h... (N:%d, M:%d)\n", w_img, h_img);
		DCT dct;
		dct.dct2(Y, h, w_img, h_img, window);

		double* spec = NULL;
		double w_spec;
		double h_spec;
		dct.spectrum(&spec, &w_spec, &h_spec, false);

		try {
			file_name = "C:\\src\\amo\\DIP\\Debug\\circle_DCTSpec";
			if (is_inverse_psf)
				file_name += "_inverse";
			file_name += ".raw";
			remove(file_name.c_str());
			fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
			if (fos.fail()) {
				LOG("%s\n", strerror(errno));
				return false;
			}
			else fos.clear();
			if (spec)
				fos.write((char*)spec, w_spec*h_spec*sizeof(double));
			fos.close();
			LOG("[DFT Spec of circle image file]:%s\n", file_name.c_str());
		}
		catch (exception ex) {
			return false;
		}
		if (spec)
			free(spec);
	}

	//make kernel as h only
	if (Y == NULL && H == NULL) {
		if (kernel != NULL) {
			if (is_normalize_and_element_wise_divided_by_sum) { //kernel used to do filterring by conv or fft
				normalize(h_normed, h, w_img, h_img, 0, 1); 
				if (h_normed == NULL || h == NULL) return false;
				memcpy(h, h_normed, sizeof(double)*w_img*h_img);
				free(h_normed);
				h_normed = NULL;
			}
	
			if (kernel != NULL) {
				if (*kernel != NULL) {
					free(*kernel);
					*kernel = NULL;
				}
				*kernel = (double*) malloc(sizeof(double)*w_img*h_img);
				memcpy(*kernel, h, sizeof(double)*w_img*h_img);
			}
		}	
	} 

	if (circle)
		free(circle);
	if (h)
		free(h);
	if (h_normed)
		free(h_normed);

	return true;
}

bool Filter::rectangle(uint8_t** dst, int w_rect, int h_rect, int r_rect, int w_img, int h_img, complex<double>** H, bool dft_by_normalized) {
	if (w_img <= 0 || h_img <= 0 || r_rect <= 0 || w_rect < r_rect*2 || h_rect < r_rect*2)
		return false;
	uint8_t* rect = (uint8_t*)malloc(sizeof(uint8_t)*w_rect*h_rect);
	memset(rect, 0, sizeof(uint8_t)*w_rect*h_rect);
	double radio_decay = 0;
	int p = 0;
	for (int j=(h_rect/2-r_rect); j<(h_rect/2+r_rect); j++) {
		for (int i=(w_rect/2-r_rect); i<(w_rect/2+r_rect); i++) {
			p = i+j*w_rect;
			//radio_decay = abs(i-w_rect/2.0)/(w_rect/2.0);
			if (j==h_rect/2 && i==w_rect/2)
				rect[p] = 128;
			else
				rect[p] = 128*(1.0-radio_decay);
		}
	}

	ofstream fos;
	try {
		string file_name = "C:\\src\\amo\\DIP\\Debug\\rectangle.raw";
		remove(file_name.c_str());
		fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			LOG("%s\n", strerror(errno));
			return false;
		}
		else fos.clear();
		uint8_t *output = NULL;
		fos.write((char*)rect, (double)w_rect*(double)h_rect);
		fos.close();
		LOG("[Rectangle file (%dx%d)]:%s\n", w_rect, h_rect, file_name.c_str());
	}
	catch (exception ex) {
		if (rect)
			free(rect);
		return false;
	}

	if (H==NULL || dst==NULL) { 
		if (rect)
			free(rect);
		LOG("Exit without developing job\n");
		return true;
	}

	LOG("Developing job...\n");
	if (dst) {
		if (*dst != NULL) {
			memset(*dst, 0, sizeof(uint8_t)*w_img*h_img);
			for (int j=0; j<h_rect; j++) {
				for (int i=0; i<w_rect; i++) {
					(*dst)[i+j*w_img] = (uint8_t)rect[i+j*w_rect];
				}
			}
		}
	}

	if (*dst == NULL) {
		return false;
	}

	double* h = (double*) malloc(sizeof(double)*w_img*h_img);
	for (int p = 0; p<w_img*h_img; p++) {
		if (&((*dst)[p]) != NULL) {
			h[p] = (double)(*dst)[p];
		}
	}
	double* h_normed = NULL;
	for (int p = 0; p<w_img*h_img; p++) {
		if (&((*dst)[p]) != NULL) {
			h[p] = (*dst)[p];
		}
	}
	if (dft_by_normalized) {
		normalize(h_normed, h, w_img, h_img, 0, 1); 
		if (h_normed == NULL || h == NULL) return false;
		memcpy(h, h_normed, sizeof(double)*w_img*h_img);
		free(h_normed);
		h_normed = NULL;
	}

	LOG("DFT of H... (N:%d, M:%d)\n", w_img, h_img);
	DFT dft;
	dft.dft2(H, h, w_img, h_img);

	double* spec = NULL;
	double w_spec;
	double h_spec;
	dft.spectrum(spec, &w_spec, &h_spec, false);

	try {
		string file_name = "C:\\src\\amo\\DIP\\Debug\\rectangle_img.raw";
		remove(file_name.c_str());
		fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			LOG("%s\n", strerror(errno));
			return false;
		}
		else fos.clear();
		if (h)
			fos.write((char*)(*dst), sizeof(char)*(double)w_img*(double)h_img);
		fos.close();
		LOG("[Rectangle image file (%d x %d)]:%s\n", w_img, h_img, file_name.c_str());

		file_name = "C:\\src\\amo\\DIP\\Debug\\rectangle_DFTSpec.raw";
		remove(file_name.c_str());
		fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			LOG("%s\n", strerror(errno));
			return false;
		}
		else fos.clear();
		if (spec)
			fos.write((char*)spec, w_spec*h_spec*sizeof(double));
		fos.close();
		LOG("[DFT Spec of rectangle image file]:%s\n", file_name.c_str());
	}
	catch (exception ex) {
		return false;
	}

	if (spec)
		free(spec);
	if (rect)
		free(rect);
	return true;
}

bool Filter::cos_x(uint8_t** dst, int w_func, int h_func, int dft_window, int w_img, int h_img, double freq, double phi, complex<double>** H, bool dft_by_normalized) {
		if (w_func <= 0 || h_func <= 0)
		return false;
	uint8_t* cosx = (uint8_t*)malloc(sizeof(uint8_t)*w_func*h_func);
	memset(cosx, 0, sizeof(uint8_t)*w_func*h_func);
	double theta = 0;
	int p = 0;
	double d = 300; //um
	double c = 3000; //m pers
	double scale = w_img/w_func;
	double value = 0;
	for (int j=0; j<h_func; j++) {
		for (int i=0; i<w_func; i++) {
			p = i+j*w_func;
			theta = phi + 2*PI*(freq*d/c)*i*scale;
			value = (127.0*cos(2*PI*i*0.005)+127.0);
			//value = (127.0*cos(2*PI*i*0.005-PI)+127.0);
 			cosx[p] = (uint8_t)value;
		}
	}


	ofstream fos;
	try {
		string file_name = "C:\\src\\amo\\DIP\\Debug\\cos_x.raw";
		remove(file_name.c_str());
		fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			LOG("%s\n", strerror(errno));
			return false;
		}
		else fos.clear();
		uint8_t *output = NULL;
		fos.write((char*)cosx, (double)w_func*(double)h_func);
		fos.close();
		LOG("[Cos_x file (%dx%d)]:%s\n", w_func, h_func, file_name.c_str());
	}
	catch (exception ex) {
		if (cosx)
			free(cosx);
		return false;
	}

	if (H==NULL || dst==NULL) { 
		if (cosx)
			free(cosx);
		LOG("Exit without developing job\n");
		return true;
	}

	LOG("Developing job...\n");
	if (dst) {
		if (*dst != NULL) {
			memset(*dst, 0, sizeof(uint8_t)*w_img*h_img);
			for (int j=0; j<h_func; j++) {
				for (int i=0; i<w_func; i++) {
					(*dst)[i+j*w_img] = (uint8_t)cosx[i+j*w_func];
				}
			}
		}
	}

	if (*dst == NULL) {
		return false;
	}

	double* h = (double*) malloc(sizeof(double)*w_img*h_img);
	double* h_normed = NULL;
	for (int p = 0; p<w_img*h_img; p++) {
		if (&((*dst)[p]) != NULL) {
			h[p] = (double)(*dst)[p];
		}
	}
	if (dft_by_normalized) {
		normalize(h_normed, h, w_img, h_img, 0, 1); 
		if (h_normed == NULL || h == NULL) return false;
		memcpy(h, h_normed, sizeof(double)*w_img*h_img);
		free(h_normed);
		h_normed = NULL;
	}	

#if 0 //debug	
	double cosx_tmp[80*80];
	memcpy(cosx_tmp, h, sizeof(double)*80*80);
#endif

	LOG("DFT of H... (N:%d, M:%d)\n", w_img, h_img);
	DFT dft;
	dft.dft2(H, h, w_img, h_img, dft_window);

	double* spec = NULL;
	double w_spec;
	double h_spec;
	dft.spectrum(spec, &w_spec, &h_spec, false);

	try {
		string file_name = "C:\\src\\amo\\DIP\\Debug\\cos_x_img.raw";
		remove(file_name.c_str());
		fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			LOG("%s\n", strerror(errno));
			return false;
		}
		else fos.clear();
		if (h)
			fos.write((char*)(*dst), sizeof(char)*(double)w_img*(double)h_img);
		fos.close();
		LOG("[Cos_x image file (%d x %d)]:%s\n", w_img, h_img, file_name.c_str());

		file_name = "C:\\src\\amo\\DIP\\Debug\\cos_x_DFTSpec.raw";
		remove(file_name.c_str());
		fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			LOG("%s\n", strerror(errno));
			return false;
		}
		else fos.clear();
		if (spec)
			fos.write((char*)spec, w_spec*h_spec*sizeof(double));
		fos.close();
		LOG("[DFT Spec of cos_x image file]:%s\n", file_name.c_str());
	}
	catch (exception ex) {
		return false;
	}

	if (spec)
		free(spec);
	if (cosx)
		free(cosx);
	return true;
}
