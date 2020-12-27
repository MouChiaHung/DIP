	/******************************************************************************
	**                      INCLUDE
	*******************************************************************************/
#include "filter.h"
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
Filter::Filter() {

}

Filter::~Filter() {

}

bool Filter::convolution(double*& dst, double* src, double* kernel, int width, int height, int window) {
	if (src == NULL || kernel == NULL)
		return false;
	if (width <= 0 || height <= 0 || width < window || height < window )
		return false;

#ifdef PRINT_FILTER //print
	printf("--- source ---\n");
	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			printf("[");
			if (src[i+j*width] > -1) printf(" ");
			if (src[i+j*width] < 100) printf(" ");
			if (src[i+j*width] < 10) printf(" ");
			printf("%.2f]", src[i+j*width]);
		}
		printf("\n");
	}
	printf("--- kernel ---\n");
	for (int j=0; j<window; j++) {
		for (int i=0; i<window; i++) {
			printf("[");
			if (kernel[i+j*window] > -1) printf(" ");
			if (kernel[i+j*window] < 100) printf(" ");
			if (kernel[i+j*window] < 10) printf(" ");
			printf("%.2f]", kernel[i+j*window]);
		}
		printf("\n");
	}
#endif
	
	dst = new double[sizeof(double)*width*height];
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
					ii = i+(n-window/2);
					jj = j+(m-window/2);
					nn = (window-1)-n;
					mm = (window-1)-m;
					if (ii >=0 && ii<width && jj>=0 && jj<height) {
						dst[i+j*width] += src[ii+jj*width]*kernel[nn+mm*window];
					}
					else {
						dst[i+j*width] += 0;
					}
				}
			}
		}
	}

#ifdef PRINT_FILTER //print
	printf("--- convolution ---\n");
	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			printf("[");
			double d = dst[i+j*width];
			if (dst[i+j*width] < 0) {
				if (dst[i+j*width] > -100 ) printf(" ");
				if (dst[i+j*width] > -10) printf(" ");
			}
			else {
				printf(" ");
				if (dst[i+j*width] < 100 ) printf(" ");
				if (dst[i+j*width] < 10) printf(" ");
			}
			printf("%.2f]", dst[i+j*width]);
		}
		printf("\n");
	}
#endif
	return true;
}


bool Filter::convolutionZeroPadding(double*& dst, double* src, double* kernel, int width, int height, int window) {
	if (src == NULL || kernel == NULL)
		return false;
	if (width <= 0 || height <= 0 || width < window || height < window )
		return false;

	//zero padding
	int len_srcc = sizeof(double)*(width+2*(window-1))*(height+2*(window-1));
	double* srcc = new double[len_srcc];
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
	printf("--- source ---\n");
	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			printf("[");
			if (srcc[i+j*width] > -1) printf(" ");
			if (srcc[i+j*width] < 100) printf(" ");
			if (srcc[i+j*width] < 10) printf(" ");
			printf("%.2f]", srcc[i+j*width]);
		}
		printf("\n");
	}
	printf("--- kernel ---\n");
	for (int j=0; j<window; j++) {
		for (int i=0; i<window; i++) {
			printf("[");
			if (kernel[i+j*window] > -1) printf(" ");
			if (kernel[i+j*window] < 100) printf(" ");
			if (kernel[i+j*window] < 10) printf(" ");
			printf("%.2f]", kernel[i+j*window]);
		}
		printf("\n");
	}
#endif

	dst = new double[sizeof(double)*width*height];
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
					ii = i-(window-1)+n;
					jj = j-(window-1)+m;
					nn = (window-1)-n;
					mm = (window-1)-m;
					if (ii >=0 && ii<width && jj>=0 && jj<height) {
						//printf("dst[%d][%d]+=srcc[%d][%d]*kernel[%d][%d]\n",i,j,ii,jj,nn,mm);
						//printf("dst[%d][%d]+=%.f*%.f\n",i,j,srcc[ii+jj*width],kernel[nn+mm*window]);
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
	printf("--- convolution with zero padding ---\n");
	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			printf("[");
			double d = dst[i+j*width];
			if (dst[i+j*width] < 0) {
				if (dst[i+j*width] > -100 ) printf(" ");
				if (dst[i+j*width] > -10) printf(" ");
			}
			else {
				printf(" ");
				if (dst[i+j*width] < 100 ) printf(" ");
				if (dst[i+j*width] < 10) printf(" ");
			}
			printf("%.2f]", dst[i+j*width]);
		}
		printf("\n");
	}
#endif
	return true;
}

bool Filter::sobel(double*& dst, double* src, int width, int height, string file) {
	if (src == NULL)
		return false;
	if (width <= 0 || height <= 0)
		return false;
	/*
	dst = new double[sizeof(double)*width*height];
	if (dst == NULL) 
		return false;
	memset(dst, 0, sizeof(double)*width*height);
	*/

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
				printf("[%.1f]", edge_hypotenuse[i+j*window]);
			}
			printf("\n");
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
	
	dst = new double[sizeof(double)*width*height];
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
		string file_name_raw = "";
		string file_name_raw_path = file.substr(0, file.find_last_of("/\\"));
		string file_name_raw_file = file.substr(file.find_last_of("/\\"));
		string file_name_raw_file_to_matlab = file_name_raw_file.substr(0, file_name_raw_file.find_last_of("/.")) + "Filtered";
		file_name_raw += file_name_raw_path;
		file_name_raw += file_name_raw_file_to_matlab;
		file_name_raw += "";
		remove(file_name_raw.c_str());
		fos.open(file_name_raw, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			printf("%s\n", strerror(errno));
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
		printf("output file:%s\n", file_name_raw.c_str());
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
	/*
	dst = new double[sizeof(double)*width*height];
	if (dst == NULL) 
		return false;
	memset(dst, 0, sizeof(double)*width*height);
	*/

	double kernel_Gaussian[3*3] = { 1/16.0, 1/8.0, 1/16.0,
									1/8.0 , 1/4.0, 1/8.0,
									1/16.0, 1/8.0, 1/16.0};
	this->convolution(dst, src, kernel_Gaussian, width, height, 3);

	if (file.empty())
		return true;
	ofstream fos;
	try {
		string file_name_raw = "";
		string file_name_raw_path = file.substr(0, file.find_last_of("/\\"));
		string file_name_raw_file = file.substr(file.find_last_of("/\\"));
		string file_name_raw_file_to_matlab = file_name_raw_file.substr(0, file_name_raw_file.find_last_of("/.")) + "Gaussianed";
		file_name_raw += file_name_raw_path;
		file_name_raw += file_name_raw_file_to_matlab;
		file_name_raw += "";
		remove(file_name_raw.c_str());
		fos.open(file_name_raw, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			printf("%s\n", strerror(errno));
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
		printf("output file:%s\n", file_name_raw.c_str());
	}
	catch (exception ex) {
		return false;
	}
	return true;
}

