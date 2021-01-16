	/******************************************************************************
	**                      INCLUDE
	*******************************************************************************/
#include "filter.h"
using namespace std;

	/******************************************************************************
	**                      DEFINE
	*******************************************************************************/
#define PI 3.14159265

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

bool Filter::normalize(uint8_t*& dst, double* src, int width, int height, bool isAllPositive) {
	dst = new uint8_t[sizeof(uint8_t)*width*height];
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
	printf("normalize max:%.1f, min:%.1f, range:%.1f\n", max, min, range);
	double dtmp = 0.0;
	double dtmp_norm = 0.0;
    for (int j=0; j<height; j++) {
        for (int i=0; i<width; i++) {
            norm = src[i+j*width];
			dtmp_norm = norm;
			norm = 255.0*(norm-min)/range;
			if (dtmp != dtmp_norm) {
				if (norm < 2 || norm > 254) {
					//printf("[%d,%d]norm:%.1f to %.1f(%d)\n", i, j, dtmp_norm, norm, (uint8_t)norm);
				}
				dtmp = dtmp_norm;
			}
			dst[i+j*width] = (uint8_t)norm;
		}
    }
    return true;
}

bool Filter::cast(uint8_t*& dst, double* src, int width, int height) {
	dst = new uint8_t[sizeof(uint8_t)*width*height];
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

bool Filter::correlation(double*& dst, double* src, double* kernel, int width, int height, int window) {
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
	printf("correlated matrix max:%.1f, min:%.1f, range:%.1f\n", max, min, range);
	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			norm = dst[i+j*width];
			norm = 255.0*norm/range;
			dst[i+j*width] = norm;
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
	printf("convolution max:%.1f, min:%.1f\n", max, min);

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

	double kernel_Gaussian[3*3] = { 1.0/16.0, 1.0/8.0, 1.0/16.0,
									1.0/8.0 , 1.0/4.0, 1.0/8.0,
									1.0/16.0, 1.0/8.0, 1.0/16.0};
	this->convolution(dst, src, kernel_Gaussian, width, height, 3);
	//this->correlation(dst, src, kernel_Gaussian, width, height, 3);

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
		uint8_t *output = NULL;
		int output_len = sizeof(uint8_t)*width*height;
		normalize(output, dst, width, height, true);
		fos.write((char*)output, output_len);
		fos.close();
		printf("output file:%s\n", file_name_raw.c_str());
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
	printf("GX:\n");
	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			if (Gx[i+j*width]!=0)
			printf("[(%d,%d):%.f]", i, j, Gx[i+j*width]);
		}
		printf("\n");
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
					 printf("[Err1QUA]ori:%.1f\n", ori);
					return false;
				}
				ori_normalized = (ori)*255.0/360.0; 
				if (ori != 0.0 && dtmp != ori) {
					//printf("[Qudrant 1]orientation: %.1f to be %.1f(%d)\n", ori, ori_normalized, (uint8_t)ori_normalized);
					dtmp = ori;
				}
			}
			else if (gx >= 0 && gy < 0) { //[Qudrant 4]:[270,360) equal to [-90, 0) to be [191,255]
				if (ori > 0) {
					printf("[Err4QUA]orientation:%.1f\n", ori);
					return false;
				}
				ori_normalized = (360.0+ori)*255.0/360.0;
				if (ori != 0.0 && dtmp != ori) {
					//printf("[Qudrant 4]orientation:%.1f to be %.1f(%d)\n", ori, ori_normalized, (uint8_t)ori_normalized);
					dtmp = ori;
				}
			}
			else if (gx < 0 && gy >= 0) { //[Qudrant 2]:(90,180] to be [64,127]
				if (ori < 0) {
					printf("[Err2QUA]orientation:%.1f\n", ori);
					return false;
				}
				ori_normalized = (ori)*255.0/360.0; 
				if (ori != 0.0 && dtmp != ori) {
					//printf("[Qudrant 2]orientation: %.1f to be %.1f(%d)\n", ori, ori_normalized, (uint8_t)ori_normalized);
					dtmp = ori;
				}
			}
			else if (gx < 0 && gy < 0) { //[Qudrant 3]:(180,270) equal to (-180, -90) to be [128,190] 
				if (ori > 0) {
					printf("[Err3QUA]orientation:%.1f\n", ori);
					return false;
				}
				ori_normalized = (360.0+ori)*255.0/360.0; 
				if (ori != 0.0 && dtmp != ori) { 
					//printf("[Qudrant 3]orientation:%.1f to be %.1f(%d)\n", ori, ori_normalized, (uint8_t)ori_normalized);
					dtmp = ori;
				}
			}
			else {
				printf("[EXCEPTION]orientation:%.1f\n", ori);
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
				//if (ori != 0.0 && ori > -90.0) printf("ori:%.1f to be %.1f\n", ori, ori_normalized);
			}
			else {
				ori_normalized = (ori)*255.0/360.0; //[0,+pi] to [0,126]
				//if (ori != 0.0 && ori < 90.0) printf("ori:%.1f to be %.1f\n", ori, ori_normalized);
			}
			orientation[i+j*width] = ori_normalized;
		}
	}

#endif 

	if (file.empty())
		return true;
	ofstream fos;
	try {
		string file_name_raw = "";
		string file_name_raw_path = file.substr(0, file.find_last_of("/\\"));
		string file_name_raw_file = file.substr(file.find_last_of("/\\"));
		string file_name_raw_file_to_matlab = file_name_raw_file.substr(0, file_name_raw_file.find_last_of("/.")) + "GradientMAG";
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
		uint8_t *output = NULL;
		int output_len = sizeof(uint8_t)*width*height;
		normalize(output, magnitude, width, height, true);
		fos.write((char*)output, output_len);
		if (output) 
			delete[](output);
		
		fos.close();
		printf("gradient magnitude output file:%s\n", file_name_raw.c_str());

		file_name_raw = "";
		file_name_raw_path = file.substr(0, file.find_last_of("/\\"));
		file_name_raw_file = file.substr(file.find_last_of("/\\"));
		file_name_raw_file_to_matlab = file_name_raw_file.substr(0, file_name_raw_file.find_last_of("/.")) + "GradientORI";
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
		output = NULL;
		output_len = sizeof(uint8_t)*width*height;
		//normalize(output, orientation, width, height, true);
		cast(output, orientation, width, height);
		fos.write((char*)output, output_len);
		if (output) 
			delete[](output);
		fos.close();
		printf("gradient orientation output file:%s\n", file_name_raw.c_str());

		file_name_raw = "";
		file_name_raw_path = file.substr(0, file.find_last_of("/\\"));
		file_name_raw_file = file.substr(file.find_last_of("/\\"));
		file_name_raw_file_to_matlab = file_name_raw_file.substr(0, file_name_raw_file.find_last_of("/.")) + "GX";
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
		output = NULL;
		output_len = sizeof(uint8_t)*width*height;
		normalize(output, Gx, width, height, true); //normailzed Gx:-127.5 to 0.0(0), 0.0 to 127.5(127), 127.5 to 255.0(255)
		fos.write((char*)output, output_len);
		if (output) 
			delete[](output);
		fos.close();
		printf("gradient x output file:%s\n", file_name_raw.c_str());

		file_name_raw = "";
		file_name_raw_path = file.substr(0, file.find_last_of("/\\"));
		file_name_raw_file = file.substr(file.find_last_of("/\\"));
		file_name_raw_file_to_matlab = file_name_raw_file.substr(0, file_name_raw_file.find_last_of("/.")) + "GY";
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
		output = NULL;
		output_len = sizeof(uint8_t)*width*height;
		normalize(output, Gy, width, height, true);
		fos.write((char*)output, output_len);
		if (output) 
			delete[](output);
		fos.close();
		printf("gradient y output file:%s\n", file_name_raw.c_str());
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
		string file_name_raw = "";
		string file_name_raw_path = file.substr(0, file.find_last_of("/\\"));
		string file_name_raw_file = file.substr(file.find_last_of("/\\"));
		string file_name_raw_file_to_matlab = file_name_raw_file.substr(0, file_name_raw_file.find_last_of("/.")) + "Laplacianed";
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
		uint8_t *output = NULL;
		int output_len = sizeof(uint8_t)*width*height;
		//normalize(output, magnitude, width, height, true);
		cast(output, magnitude, width, height);
		fos.write((char*)output, output_len);
		printf("laplacianed output file:%s\n", file_name_raw.c_str());
		if (output) 
			delete[](output);
	}
	catch (exception ex) {
		return false;
	}
	return true;
}