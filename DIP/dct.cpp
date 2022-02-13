	/******************************************************************************
	**                      INCLUDE
	*******************************************************************************/
#include "dct.h"
#include "filter.h"

	/******************************************************************************
	**                      DEFINE
	*******************************************************************************/

	/******************************************************************************
	**                      GLOBAL VARIABLE
	*******************************************************************************/

	/******************************************************************************
	**                      DECLARATION
	*******************************************************************************/

	/******************************************************************************
	**                      IMPLEMENTATION
	*******************************************************************************/
DCT::DCT() {
	//cout << "[DCT::DCT()]:" << endl;
	m = 0;
	n = 0;
	matrix = NULL;
}

DCT::DCT(double n_, double m_, double* M) {
	n = n_;
	m = m_;
	matrix = (double*) malloc(sizeof(double)*(int)n*(int)m);
	if (matrix == NULL) {
		matrix = M;
	}
	else {
		memcpy(matrix, M, sizeof(double)*(int)n*(int)m);
	}
}

DCT::~DCT() {
	//cout << "[DCT::~DCT()]:" << endl;
	if (matrix) {
		free(matrix);
		matrix = NULL;
	}
	clear();
}

bool DCT::clear(void) {
	if (matrix != NULL)
		free(matrix);
	matrix = NULL;
	m = 0;
	n = 0;
	return true;
}

	/******************************************************************************
	**                      DCT
	*******************************************************************************/
/**
 * N-by-N discrete cosine transform (DCT) matrix, which you can use to perform a 2-D IDCT on an image
 * dctmtx: 
 *     ctm = sqrt(1/N), u=0; =sqrt(2/N)*cos(pi*(2*i+1)*u/(2*N)), 1<=u<=(N-1)
 * DCT:
 *     IM = (ctm)  * im * (ctm)'
 * IDCT: 
 *     im = (ctm') * IM * (ctm')'
 */
bool DCT::dctmtx(double* ctm, int N) {
	double alpha = 0;
	for (int u=0; u<N; u++) {
		for (int i=0; i<N; i++) {
			if (u == 0) {
				alpha = sqrt(1.0/(double)N);
			}
			else {
				alpha = sqrt(2.0/(double)N);
			}
			ctm[i+u*N] = alpha*cos(PI*(2.0*(double)i+1.0)*u/(2.0*(double)N));
		}
	}
	return true;
}

bool DCT::dct2(double* IM, double* im, int N) {
	Filter f;
	bool ret = false;
	//get dctmtx
	double* ctm  = (double*)malloc(N*N*sizeof(double));
	double* ctmT = (double*)malloc(N*N*sizeof(double));
	double* IMui = (double*)malloc(N*N*sizeof(double));

	//get ctm
	if (!dctmtx(ctm, N)) {
		printf("[%s][%s]Failed to dctmtx\n", "DCT", __func__);
		goto EXIT_FAST_DCT2;
	}

	//get ctm'
	if (!f.mat_transpose(ctmT, ctm, N, N)) {
		printf("[%s][%s]Failed to mat_transpose\n", "DCT", __func__);
		goto EXIT_FAST_DCT2;
	}

	//doing IM = ctm * im * ctm'
	if (!f.mat_multiply(IMui, im, ctmT, N, N, N, N, N, N)) {
		printf("[%s][%s]Failed to mat_multiply im by ctm'\n", "DCT", __func__);
		goto EXIT_FAST_DCT2;
	}
	if (!f.mat_multiply(IM, ctm, IMui, N, N, N, N, N, N)) {
		printf("[%s][%s]Failed to mat_multiply ctm by IMui\n", "DCT", __func__);
		goto EXIT_FAST_DCT2;
	}
	this->n = N;
	this->m = N;
	this->matrix = (double*)malloc(sizeof(double)*this->n*this->m);
	if (matrix == NULL) 
		return false;
	memcpy(this->matrix, IM, sizeof(double)*this->n*this->m);
	ret = true;

EXIT_FAST_DCT2:
	if (ctm)
		free(ctm);
	if (ctmT)
		free(ctmT);
	if (IMui)
		free(IMui);
	return ret;
}

bool DCT::dct2_ram_cost_reduce(double* IM, double* im, int N) {
	Filter f;
	bool ret = false;
	//get dctmtx
	double* ctm  = (double*)malloc(N*N*sizeof(double));
	double* IMui = (double*)malloc(N*N*sizeof(double));

	//get ctm
	if (!dctmtx(ctm, N)) {
		printf("[%s][%s]Failed to dctmtx\n", "DCT", __func__);
		goto EXIT_FAST_DCT2;
	}

	//doing IM = ctm * im * ctm'
	if (!f.mat_multiply_by_matB_Transpose(IMui, im, ctm, N, N, N, N, N, N)) {
		printf("[%s][%s]Failed to mat_multiply_by_matB_Transpose im by ctm\n", "DCT", __func__);
		goto EXIT_FAST_DCT2;
	}
	if (!f.mat_multiply(IM, ctm, IMui, N, N, N, N, N, N)) {
		printf("[%s][%s]Failed to mat_multiply ctm by IMui\n", "DCT", __func__);
		goto EXIT_FAST_DCT2;
	}
	this->n = N;
	this->m = N;
	this->matrix = (double*)malloc(sizeof(double)*this->n*this->m);
	if (matrix == NULL) 
		return false;
	memcpy(this->matrix, IM, sizeof(double)*this->n*this->m);
	ret = true;

EXIT_FAST_DCT2:
	if (ctm)
		free(ctm);
	if (IMui)
		free(IMui);
	return ret;
}

bool DCT::idct2(double* im, double* IM, int N) {
	Filter f;
	bool ret = false;
	//get dctmtx
	double* ctm  = (double*)malloc(N*N*sizeof(double));
	double* ctmT = (double*)malloc(N*N*sizeof(double));
	double* imui = (double*)malloc(N*N*sizeof(double));

	//get ctm of DCT
	if (!dctmtx(ctm, N)) {
		printf("[%s][%s]Failed to dctmtx\n", "DCT", __func__);
		goto EXIT_FAST_IDCT2;
	}

	//get ctm of IDCT which is the transpose matrix of ctm of DCT
	if (!f.mat_transpose(ctmT, ctm, N, N)) {
		printf("[%s][%s]Failed to mat_transpose\n", "DCT", __func__);
		goto EXIT_FAST_IDCT2;
	}

	//doing im = (ctm') * IM * (ctm')'
	if (!f.mat_multiply(imui, IM, ctm, N, N, N, N, N, N)) {
		printf("[%s][%s]Failed to mat_multiply im by (ctm')'\n", "DCT", __func__);
		goto EXIT_FAST_IDCT2;
	}
	if (!f.mat_multiply(im, ctmT, imui, N, N, N, N, N, N)) {
		printf("[%s][%s]Failed to mat_multiply (ctm') by imui\n", "DCT", __func__);
		goto EXIT_FAST_IDCT2;
	}
	ret = true;

EXIT_FAST_IDCT2:
	if (ctm)
		free(ctm);
	if (ctmT)
		free(ctmT);
	if (imui)
		free(imui);
	return ret;
}

bool DCT::idct2_ram_cost_reduce(double* im, double* IM, int N) {
	Filter f;
	bool ret = false;
	//get dctmtx
	double* ctm  = (double*)malloc(N*N*sizeof(double));
	double* imui = (double*)malloc(N*N*sizeof(double));

	//get ctm of DCT
	if (!dctmtx(ctm, N)) {
		printf("[%s][%s]Failed to dctmtx\n", "DCT", __func__);
		goto EXIT_FAST_IDCT2;
	}

	//doing im = (ctm') * IM * (ctm')'
	if (!f.mat_multiply(imui, IM, ctm, N, N, N, N, N, N)) {
		printf("[%s][%s]Failed to mat_multiply im by (ctm')'\n", "DCT", __func__);
		goto EXIT_FAST_IDCT2;
	}
	if (!f.mat_multiply_by_matA_Transpose(im, ctm, imui, N, N, N, N, N, N)) {
		printf("[%s][%s]Failed to mat_multiply_by_matA_Transpose ctm by imui\n", "DCT", __func__);
		goto EXIT_FAST_IDCT2;
	}
	ret = true;

EXIT_FAST_IDCT2:
	if (ctm)
		free(ctm);
	if (imui)
		free(imui);
	return ret;
}

bool DCT::dct2(double** dst, double* src, double w, double h) {
	return dct2(dst, src, w, h, (w>=h)?w:h);
}

bool DCT::dct2(double** dst, double* src, double w, double h, double window) {
	if (w <= 0 || h <= 0)
		return false;
	uint64_t w_pow_check = (uint64_t)w&((uint64_t)w-1);
	uint64_t h_pow_check = (uint64_t)h&((uint64_t)h-1);
	if (w_pow_check != 0 || h_pow_check != 0) //check if pow of 2
	{
		printf("[%s][%s]Dimension not power of 2 for DCT-II but continue...\n", "DCT", __func__);
		//return false;
	}
	matrix = (double*)malloc(sizeof(double)*w*h);
	if (matrix == NULL) 
		return false;
	memset(matrix, 0, sizeof(double)*w*h);
	double c;
	double Cu = 0;
	double Cv = 0;
	for (double v=0; v<h; v++) {
		for (double u=0; u<w; u++) {
			for (double j=0; j<h; j++) {
				for (double i=0; i<w; i++) {
					if (u<window && v<window) {
						if (u==0) { //matlab F1
							Cu = (sqrt(1.0))/(sqrt(w));
						}
						else {
							Cu = (sqrt(2.0))/(sqrt(w));
						}
						if (v==0) { 
							Cv = (sqrt(1.0))/(sqrt(h));
						}
						else {
							Cv = (sqrt(2.0))/(sqrt(h));
						}
						c = Cu*Cv*src[(int)i+(int)j*(int)w]*cos(PI*((u*(2.0*i+1.0)/(2.0*w))))*cos(PI*((v*(2.0*j+1.0))/(2.0*h)));
						matrix[(int)u+(int)v*(int)w] += c; 
					}
					else {
						c = 0;
						matrix[(int)u+(int)v*(int)w] += 0;
						break;
					}
				}
			}
		}	
	}
	m = h;
	n = w;
	if (dst) {
		if (*dst) {
			free(*dst);
			*dst = NULL;
		}
		*dst = (double*)malloc(sizeof(double)*w*h);
		memcpy(*dst, matrix, sizeof(double)*w*h);
	}

#if PRINT_DCT //print
	printf("--- dct ---\n");
	for (int v=0; v<h; v++) {
		for (int u=0; u<w; u++) {
			printf("[");
			if (matrix[u+v*(int)w] > 0)         printf(" ");
			if (abs(matrix[u+v*(int)w]) < 99.999999)  printf(" ");
			if (abs(matrix[u+v*(int)w]) <  9.999999)  printf(" ");
			printf("%.1f]", matrix[u+v*(int)w]);
		}
		printf("\n");
	}
#endif

	return true;
}

bool DCT::dct2(double** dst, double* src, double w, double h, double window, double group) {
	if (group != 0 && w != h)
		return false;
	if (w <= 0 || h <= 0)
		return false;
	if (group != 0 && (((int)w%(int)group) || ((int)h%(int)group)))
		return false;
	uint64_t w_pow_check = (uint64_t)w&((uint64_t)w-1);
	uint64_t h_pow_check = (uint64_t)h&((uint64_t)h-1);
	if (w_pow_check != 0 || h_pow_check != 0) //check if pow of 2
		return false;
	matrix = (double*)malloc(sizeof(double)*w*h);
	if (matrix == NULL) 
		return false;
	memset(matrix, 0, sizeof(double)*w*h);
	double c;
	double Cu = 0;
	double Cv = 0;

	int area = 0;
	int ii = 0; //data using absolute coordinate, phase of cos using relative coordinate of group 
	int jj = 0;
	int uu = 0;
	int vv = 0;
	for (int y=0; y<h; y+=group) {
	for (int x=0; x<w; x+=group) {
		area++;
		for (double v=y; v<y+group; v++) {
			for (double u=x; u<x+group; u++) {
				for (double j=y; j<y+group; j++) {
					for (double i=x; i<x+group; i++) {
						if (u<window && v<window) {
							ii = (int)i%(int)group;
							jj = (int)j%(int)group;
							uu = (int)u%(int)group;
							vv = (int)v%(int)group;
							if (u==x) { //matlab F1
								//printf("[%s]Area%d [%d][%d]-[%d][%d] Cu is DC [%.f][%.f]\n", __func__, area, x, y, x+(int)group-1, y+(int)group-1, u, v);
								Cu = (sqrt(1.0))/(sqrt(group));
							}
							else {
								Cu = (sqrt(2.0))/(sqrt(group));
							}
							if (v==y) {
								//printf("[%s]Area%d [%d][%d]-[%d][%d] Cv is DC [%.f][%.f]\n", __func__, area, x, y, x+(int)group-1, y+(int)group-1, u, v);
								Cv = (sqrt(1.0))/(sqrt(group));
							}
							else {
								Cv = (sqrt(2.0))/(sqrt(group));
							}
							c = Cu*Cv*src[(int)i+(int)j*(int)w]*cos(PI*((uu*(2.0*ii+1.0)/(2.0*group))))*cos(PI*((vv*(2.0*jj+1.0))/(2.0*group)));
							matrix[(int)u+(int)v*(int)w] += c;
#if PRINT_DCT_GROUP 
							if (i >= 0) {
								printf("[%s]Area%d [%d][%d]-[%d][%d], matrix[%.f][%.f]:", __func__, area, x, y, x+(int)group-1, y+(int)group-1, u, v);
								if (matrix[(int)u+(int)v*(int)w] >= 0) printf(" ");
								if (abs(matrix[(int)u+(int)v*(int)w]) < 10) printf(" ");
								if (abs(matrix[(int)u+(int)v*(int)w]) < 100) printf(" ");
								printf("%.1f after += ", matrix[(int)u+(int)v*(int)w]);
								if (c >= 0) printf(" ");
								if (abs(c) < 10) printf(" ");
								if (abs(c) < 100) printf(" ");
								printf("%.2f by image[%.f][%.f]:", c, i, j);
								if (src[(int)i+(int)j*(int)w] >= 0) printf(" ");
								if (abs(src[(int)i+(int)j*(int)w]) < 10) printf(" ");
								if (abs(src[(int)i+(int)j*(int)w]) < 100) printf(" ");
								printf("%.1f times (%.1f*%.1f) * cos(%.1f):%.2f * cos(%.1f):%.2f\n"
										, src[(int)i+(int)j*(int)w], Cu, Cv
										, PI*((uu*(2.0*ii+1.0)/(2.0*group))), cos(PI*((uu*(2.0*ii+1.0)/(2.0*group))))
										, PI*((vv*(2.0*jj+1.0))/(2.0*group)), cos(PI*((vv*(2.0*jj+1.0))/(2.0*group)))
										);
							}
#endif
						}
						else {
							//c = 0;
							//matrix[(int)u+(int)v*(int)w] += 0;
							break;
						}
					}
				}
			}	
		}
	}
	}
	m = h;
	n = w;
	if (dst) {
		if (*dst) {
			free(*dst);
			*dst = NULL;
		}
		*dst = (double*)malloc(sizeof(double)*w*h);
		memcpy(*dst, matrix, sizeof(double)*w*h);
	}

#if PRINT_DCT //print
	printf("--- dct ---\n");
	for (int v=0; v<h; v++) {
		for (int u=0; u<w; u++) {
			printf("[");
			if (matrix[u+v*(int)w] > 0)         printf(" ");
			if (abs(matrix[u+v*(int)w]) < 99.999999)  printf(" ");
			if (abs(matrix[u+v*(int)w]) <  9.999999)  printf(" ");
			printf("%.1f]", matrix[u+v*(int)w]);
		}
		printf("\n");
	}
#endif

	return true;
}

bool DCT::idct2(double*& dst, double* w, double* h) {
	return idct2(dst, this->matrix, w, h);
}
bool DCT::idct2(double*& dst, double* matrix, double* w, double* h) {
	if (n <= 0 || m <= 0)
		return false;
	uint64_t n_pow_check = (uint64_t)n&((uint64_t)n-1);
	uint64_t m_pow_check = (uint64_t)m&((uint64_t)m-1);
	if (n_pow_check != 0 || m_pow_check != 0) //check if pow of 2
	{
		printf("[%s][%s]Dimension not power of 2 for DCT-II but continue...\n", "DCT", __func__);
		//return false;
	}

	if (w == NULL || h == NULL)
		return false;
	if (matrix == NULL)
		return false;

	dst = (double*) malloc(sizeof(double)*m*n);
	if (dst == NULL)
		return false;
	memset(dst, 0, sizeof(double)*m*n);

	double c;
	double Cu = 0;
	double Cv = 0;
	double factor_norm = 2.0/sqrt(n*m); //matlab F1
	for (double j=0; j<m; j++) {
		for (double i=0; i<n; i++) {
			for (double v=0; v<m; v++) {
				for (double u=0; u<n; u++) {		
					if (u==0) { 
						Cu = (sqrt(1.0))/(sqrt(2));
					}
					else {
						Cu = 1.0;
					}
					if (v==0) { 
						Cv = (sqrt(1.0))/(sqrt(2));
					}
					else {
						Cv = 1.0;
					}
					c = factor_norm*Cu*Cv*matrix[(int)u+(int)v*(int)n]*cos(PI*((u*(2.0*i+1.0)/(2.0*n))))*cos(PI*((v*(2.0*j+1.0))/(2.0*m)));
					dst[(int)i+(int)j*(int)n] += c; 
				}
			}
		}
	}

	*w = n;
	*h = m;

#if PRINT_DCT //print
	printf("--- idct ---\n");
	for (int j=0; j<m; j++) {
		for (int i=0; i<n; i++) {
			printf("[");
			if (dst[i+j*(int)n] > 0)         printf(" ");
			if (abs(dst[i+j*(int)n]) < 99.999999)  printf(" ");
			if (abs(dst[i+j*(int)n]) <  9.999999)  printf(" ");
			printf("%.6f]", dst[i+j*(int)n]);
		}
		printf("\n");
	}
#endif

	return true;
}

bool DCT::idct2(double*& dst, double* w, double* h, double group) {
	return idct2(dst, w, h, this->n, group);
}

bool DCT::idct2(double*& dst, double* w, double* h, double window, double group) {
	return idct2(dst, this->matrix, w, h, window, group);
}

bool DCT::idct2(double*& dst, double* matrix, double* w, double* h, double window, double group) {
	if (this->n != this->m)
		return false;
	if (n <= 0 || m <= 0)
		return false;
	if (((int)n%(int)group) || ((int)m%(int)group))
		return false;
	uint64_t n_pow_check = (uint64_t)n&((uint64_t)n-1);
	uint64_t m_pow_check = (uint64_t)m&((uint64_t)m-1);
	if (n_pow_check != 0 || m_pow_check != 0) //check if pow of 2
		return false;
	if (w == NULL || h == NULL)
		return false;
	if (matrix == NULL)
		return false;

	dst = (double*) malloc(sizeof(double)*m*n);
	if (dst == NULL)
		return false;
	memset(dst, 0, sizeof(double)*m*n);

	double c;
	double Cu = 0;
	double Cv = 0;
	double factor_norm = 2.0/sqrt(group*group); //matlab F1
	int area = 0;
	int ii = 0;
	int jj = 0;
	int uu = 0;
	int vv = 0;
	for (int y = 0; y<m; y+=group) {
		for (int x = 0; x<n; x+=group) {
			area++;
			for (double j=y; j<y+group; j++) {
				for (double i=x; i<x+group; i++) {
					for (double v=y; v<y+group; v++) {
						if (v >= window)
							break;
						for (double u=x; u<x+group; u++) {
							if (u >= window)
								break;
							ii = (int)i%(int)group;
							jj = (int)j%(int)group;
							uu = (int)u%(int)group;
							vv = (int)v%(int)group;
							if (u==x) {
								//printf("[%s]Area%d [%d][%d]-[%d][%d] Cu is DC [%.f][%.f]\n", __func__, area, x, y, x+(int)group-1, y+(int)group-1, u, v);
								Cu = (sqrt(1.0))/(sqrt(2));
							}
							else {
								Cu = 1.0;
							}
							if (v==y) { 
								//printf("[%s]Area%d [%d][%d]-[%d][%d] Cv is DC [%.f][%.f]\n", __func__, area, x, y, x+(int)group-1, y+(int)group-1, u, v);
								Cv = (sqrt(1.0))/(sqrt(2));
							}
							else {
								Cv = 1.0;
							}
							c = factor_norm*Cu*Cv*matrix[(int)u+(int)v*(int)n]*cos(PI*((uu*(2.0*ii+1.0)/(2.0*group))))*cos(PI*((vv*(2.0*jj+1.0))/(2.0*group)));
							dst[(int)i+(int)j*(int)n] += c;
#if PRINT_DCT_GROUP
							if (i >= 0) {
									printf("[%s]Area%d [%d][%d]-[%d][%d], image[%.f][%.f]:", __func__, area, x, y, x+(int)group-1, y+(int)group-1, i, j);
									if (dst[(int)i+(int)j*(int)n] >= 0) printf(" ");
									if (abs(dst[(int)i+(int)j*(int)n]) < 10) printf(" ");
									if (abs(dst[(int)i+(int)j*(int)n]) < 100) printf(" ");
									printf("%.1f after += ", dst[(int)i+(int)j*(int)n]);
									if (c >= 0) printf(" ");
									if (abs(c) < 10) printf(" ");
									if (abs(c) < 100) printf(" ");
									printf("%.2f by matrix[%.f][%.f]", c, u, v);
									if (matrix[(int)u+(int)v*(int)n] >= 0) printf(" ");
									if (abs(matrix[(int)u+(int)v*(int)n]) < 10) printf(" ");
									if (abs(matrix[(int)u+(int)v*(int)n]) < 100) printf(" ");
									printf("%.1f times (%.1f*%.1f*%.1f) * cos(%.1f):%.2f * cos(%.1f):%.2f\n"
											, matrix[(int)u+(int)v*(int)n], factor_norm, Cu, Cv
											, PI*((uu*(2.0*ii+1.0)/(2.0*group))), cos(PI*((uu*(2.0*ii+1.0)/(2.0*group))))
											, PI*((vv*(2.0*jj+1.0))/(2.0*group)), cos(PI*((vv*(2.0*jj+1.0))/(2.0*group)))
											);
							}					
#endif
						}
					}
				}
			}
		}
	}
	*w = n;
	*h = m;

#if PRINT_DCT //print
	printf("--- idct ---\n");
	for (int j=0; j<m; j++) {
		for (int i=0; i<n; i++) {
			printf("[");
			if (dst[i+j*(int)n] > 0)         printf(" ");
			if (abs(dst[i+j*(int)n]) < 99.999999)  printf(" ");
			if (abs(dst[i+j*(int)n]) <  9.999999)  printf(" ");
			printf("%f]", dst[i+j*(int)n]);
		}
		printf("\n");
	}
#endif

	return true;
}

bool DCT::spectrum(double** dst, double* w, double* h, bool isNormalize) {
	return spectrum(dst, this->matrix, w, h, isNormalize);
}

bool DCT::spectrum(double** dst, double* matrix, double* w, double* h, bool isNormalize) {
	if (w == NULL || h == NULL)
		return false;
	if (matrix == NULL)
		return false;
	if (m <= 0)
		return false;
	if (n <= 0)
		return false;
	double* dst_tmp = (double*)malloc(sizeof(double)*(int)n*(int)m);
	if (dst_tmp == NULL)
		return false;
	memset(dst_tmp, 0, sizeof(double)*m*n);

	double c = (0,0);
	double cmag = 0;
	double max = 0;
	for (double v=0; v<m; v++) {
		for (double u=0; u<n; u++) {
			c = matrix[(int)u+(int)v*(int)n];
			cmag = c;
			dst_tmp[(int)u+(int)v*(int)n] = cmag;
			if (isNormalize && cmag > max)
				max = cmag;
		}	
	}

	if (isNormalize) {
		Filter f;
		f.normalize(*dst,dst_tmp,(int)n,(int)m,0,255);
	}
	else {
		if (dst) {
			if (*dst) {
				free(*dst);
			}
			*dst = (double*)malloc(sizeof(double)*(int)n*(int)m);
			if (*dst == NULL)
				return false;
			memcpy(*dst, dst_tmp, sizeof(double)*(int)n*(int)m);
		}
	}

	*w = n;
	*h = m;

	if (dst_tmp)
		free(dst_tmp);
	return true;
}
