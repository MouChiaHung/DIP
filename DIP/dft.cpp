	/******************************************************************************
	**                      INCLUDE
	*******************************************************************************/
#include "dft.h"
using namespace std;

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
DFT::DFT() {
	//cout << "[DFT::DFT()]:" << endl;
	m = 0;
	n = 0;
	matrix = NULL;
}

DFT::~DFT() {
	//cout << "[DFT::~DFT()]:" << endl;
	clear();
}

bool DFT::clear(void) {
	if (matrix != NULL)
		free(matrix);
	matrix = NULL;
	m = 0;
	n = 0;
	return true;
}

bool DFT::dft2(double* src, double w, double h) {
#ifdef PRINT_DFT //print
	printf("--- source ---\n");
	for (int j=0; j<(int)h; j++) {
		for (int i=0; i<(int)w; i++) {
			printf("[");
			if (src[i+j*(int)w] < 100) printf(" ");
			if (src[i+j*(int)w] < 10) printf(" ");
			printf("%.1f]", src[i+j*(int)w]);
		}
		printf("\n");
	}
#endif
	matrix = (complex<double>*)malloc(sizeof(complex<double>)*w*h);
	if (matrix == NULL) 
		return false;
	memset(matrix, 0, sizeof(complex<double>)*w*h);
	complex<double> c(0,0);
	for (double v=0; v<h; v++) {
		for (double u=0; u<w; u++) {
			for (double j=0; j<h; j++) {
				for (double i=0; i<w; i++) {
#if 0 //trace
					double p = src[(int)i+(int)j*w];
					double phase = -2*PI*(u*i/w+v*j/h);
					double cos_ = cos(phase);
					double sin_ = sin(phase);
#endif
					c._Val[0] = src[(int)i+(int)j*(int)w]*cos(-2*PI*(u*i/w+v*j/h));
					c._Val[1] = src[(int)i+(int)j*(int)w]*sin(-2*PI*(u*i/w+v*j/h));
					matrix[(int)u+(int)v*(int)w] += c; 
				}
			}
		}	
	}
//#if PRINT_DFT //print
#if 0 //print
	printf("--- dft ---\n");
	for (int v=0; v<h; v++) {
		for (int u=0; u<w; u++) {
			printf("matrix[%d,%d]:(%f)+i(%f)\n",u ,v ,real(matrix[u+v*(int)w]), imag(matrix[u+v*(int)w]));
		}	
	}
#endif
	m = h;
	n = w;
	return true;
}

bool DFT::idft2(double*& dst, double* w, double* h) {
	if (w == NULL || h == NULL)
		return false;
	if (matrix == NULL)
		return false;
	if (m <= 0)
		return false;
	if (n <= 0)
		return false;
	dst = (double*) malloc(sizeof(double)*m*n);
	if (dst == NULL)
		return false;
	memset(dst, 0, sizeof(double)*m*n);

	complex<double> cx(0, 0);
	complex<double> cw(0, 0);
	complex<double> c(0, 0);
	double val = 0;
	double* pval = 0;
	double norm = 0;
	for (double j=0; j<m; j++) {
		for (double i=0; i<n; i++) {
			for (double v=0; v<m; v++) {
				for (double u=0; u<n; u++) {
#if 0 //trace
					double phase = 2*PI*(u*i/n+v*j/m);
					double cos_ = cos(phase);
					double sin_ = sin(phase);
					norm = 1.0f/(m*n);
					pval = &dst[(int)i+(int)j*(int)m];
#endif
					cx = matrix[(int)u+(int)v*(int)n];
					cw._Val[0] = (cos(2*PI*(u*i/n+v*j/m)));
					cw._Val[1] = (sin(2*PI*(u*i/n+v*j/m)));
					c = cx*cw;
					dst[(int)i+(int)j*(int)n] += (1.0f/(m*n))*c._Val[0]; //u>0&&v>0&&i>0&&j>0
				}
			}
		}
	}
#ifdef PRINT_DFT //print
	printf("--- idft ---\n");
	for (int j=0; j<m; j++) {
		for (int i=0; i<n; i++) {
			printf("[");
			if (dst[i+j*(int)n] < 100) printf(" ");
			if (dst[i+j*(int)n] < 10) printf(" ");
			if (dst[i+j*(int)n] <= 0) dst[i+j*(int)n] = 0;
			printf("%.1f]", dst[i+j*(int)n]);
		}
		printf("\n");
	}
#endif
	*w = n;
	*h = m;
	return true;
}

bool DFT::spectrum(double*& dst, double* w, double* h, bool isNormalize) {
	if (w == NULL || h == NULL)
		return false;
	if (matrix == NULL)
		return false;
	if (m <= 0)
		return false;
	if (n <= 0)
		return false;
	dst = new double[sizeof(double)*m*n];
	if (dst == NULL)
		return false;
	memset(dst, 0, sizeof(double)*m*n);

	complex<double> c = (0,0);
	double cmag = 0;
	double max = 0;
	for (double v=0; v<m; v++) {
		for (double u=0; u<n; u++) {
			c = matrix[(int)u+(int)v*(int)n];
			cmag = sqrt(c._Val[0]*c._Val[0] + c._Val[1]*c._Val[1]);
			dst[(int)u+(int)v*(int)n] = cmag;
			if (isNormalize && cmag > max)
				max = cmag;
		}	
	}
	if (isNormalize) {
		for (int v=0; v<m; v++) {
			for (int u=0; u<n; u++) {
				dst[u+v*(int)n] *= (((double)0xff)/max);
			}
		}
	}

#ifdef PRINT_DFT //print
	printf("--- spectrum ---\n");
	for (int v=0; v<(int)m; v++) {
		for (int u=0; u<(int)n; u++) {
			printf("[");
			if (dst[u+v*(int)n] < 100) printf(" ");
			if (dst[u+v*(int)n] < 10) printf(" ");
			printf("%.1f]", dst[u+v*(int)n]);
		}
		printf("\n");
	}
#endif
	*w = n;
	*h = m;
	return true;
}

bool DFT::fftshift(double*& buf, int w, int h) {
	if (buf == NULL)
		return false;
	double* tmp = new double[sizeof(double)*w*h];
	if (tmp == NULL)
		return false;
	memcpy(tmp, buf, sizeof(double)*w*h);
	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			if (i<w/2 && j<h/2) {
				buf[i+j*w] = tmp[(i+w/2)+(j+h/2)*w];
			}
			else if (i>=w/2 && j<h/2) {
				buf[i+j*w] = tmp[(i-w/2)+(j+h/2)*w];
			}
			else if (i<w/2 && j>=h/2) {
				buf[i+j*w] = tmp[(i+w/2)+(j-h/2)*w];
			}
			else if (i>=w/2 && j>=h/2) {
				buf[i+j*w] = tmp[(i-w/2)+(j-h/2)*w];
			}
			else {
				printf("[%d,%d] no shifted\n", i, j);
			}
		}
	}
#ifdef PRINT_DFT //print
	printf("--- fftshift ---\n");
	for (int j=0; j<h; j++) {
		for (int i=0; i<w; i++) {
			printf("[");
			if (buf[i+j*w] < 100) printf(" ");
			if (buf[i+j*w] < 10) printf(" ");
			printf("%.1f]", buf[i+j*w]);
		}
		printf("\n");
	}
#endif
}

























