	/******************************************************************************
	**                      INCLUDE
	*******************************************************************************/
#include "pgm.h"
#include "dft.h"
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
PGM::PGM() {
	//cout << "[PGM::PGM()]:" << endl;
	raw = NULL;
	type = 0;
	width = 0;
	height = 0;
	depth = 0;
	size = 0;
}

PGM::~PGM() {
	//cout << "[PGM::~PGM()]:" << endl;
	if (raw) free(raw);
}

bool PGM::read(string file_name, uint8_t*& data, int* size) {
	ifstream fis;
	ofstream fos;

	fis.open(file_name.c_str(), ios::in | ios::binary);
	if (fis.fail()) {
		printf("%s\n", strerror(errno));
		return false;
	}

#if 0
	string file_name_raw = "C:\\src\\amo\\DIP\\Debug\\pgm.raw";
#else
	string file_name_raw = "";
	string file_name_raw_path = file_name.substr(0, file_name.find_last_of("/\\"));
	string file_name_raw_file = file_name.substr(file_name.find_last_of("/\\"));
	string file_name_raw_file_to_matlab = file_name_raw_file.substr(0, file_name_raw_file.find_last_of("/."));
	file_name_raw += file_name_raw_path;
	file_name_raw += file_name_raw_file_to_matlab;
	file_name_raw += "";
#endif
	remove(file_name_raw.c_str());
	fos.open(file_name_raw, fstream::in | fstream::out | fstream::trunc | fstream::binary);
	if (fos.fail()) {
		printf("%s\n", strerror(errno));
		return false;
	}
	else fos.clear();
	file = file_name_raw;

	fis.seekg(0, ios::beg);
	string line; 
	//extract
	for (int i=0; i<3; i++) {
		if (getline(fis, line).fail()) {
			return false;
		}
		//printf("line:%s\n", line.c_str());
		if (i == 0) {
			char type_c1;
			char type_c2;
			if (sscanf(line.c_str(), "%c%c",&type_c1, &type_c2) <= 0) {
				printf("sscanf() failed\n");
				return false;
			}
			if (type_c1 != 'P') {
				printf("type_c1 != 'P'\n");
				return false;
			}
			this->type = type_c2;
		}
		else if (i == 1) {
			char* pc = NULL;
			pc = strtok((char*)line.c_str(), " ");
			if (pc == NULL || !(*pc>=0x30 && *pc<=0x39)) {
				i--;
				continue;
			}
			this->width = std::stoi(string(pc), NULL);
			pc = strtok(NULL, " ");
			if (pc == NULL || !(*pc>=0x30 && *pc<=0x39)) {
				i--;
				continue;
			}
			this->height = std::stoi(string(pc), NULL);
			this->size = width*height;
		}
		else if (i == 2) {
			char* pc = NULL;
			pc = strtok((char*)line.c_str(), " ");
			if (pc == NULL || !(*pc>=0x30 && *pc<=0x39)) {
				i--;
				continue;
			}
			int l = std::stoi(string(pc), NULL);
			if (l >= 0xffff) {
				this->depth = 2;
			}
			else {
				this->depth = 1;
			}
		}
	}

#if 1 //print
	printf("Extract raw data from %s\n", file_name.c_str());
	printf("type  :P%c\n", this->type);
	printf("width :%d\n", this->width);
	printf("height:%d\n", this->height);
	printf("depth :%d\n", this->depth);
	printf("size  :%d\n", this->size);
#endif

	//extract data
	//fis.read((char*)raw, width*height*depth);
	
	raw = new uint8_t[width*height*depth];
	memset(raw, 0, width*height*depth);
	//output
	uint16_t s;
	uint8_t c;
	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			fis.read((char*)(raw+i+j*width), depth);
			if (depth ==2) {
				s = *((uint16_t*)(raw+i+j*width));
				c = s & 0xff;
			}
			else {
				c = *(raw+i+j*width);
			}
			fos.write((char*)&c, 1);
			//if (i == width) fos << endl;
			//if (i == height) fos << endl;
		}
	}
	printf("output file:%s\n", file_name_raw.c_str());
	fis.close();
	fos.close();


	return true;
}

bool PGM::dft_idft(int m_, int n_) {
	if (raw == NULL)	return false;
	if (size <= 0)	return false;
	if (m_*n_ > size) return false;

	uint8_t* im = new uint8_t[m_*n_];
	if (im == NULL)
		return false;
	memset(im, 0, m_*n_);
	int k = 0;
#if 1
	for (int j=(height/2-m_/2); j<(height/2+m_/2); j++) {
		for (int i=(width/2-n_/2); i<(width/2+n_/2); i++) {
			if (im+k) {
				*(im+k) = raw[(i+j*width)];
				k++;
			}
			else 
				return false;
		}
	}
#else
	for (int j=0; j<m_; j++) {
		for (int i=0; i<n_; i++) {
			*(im+k) = raw[(i+j*width)];
			k++;
		}
	}
#endif

#ifdef PRINT_DFT_IDFT
	printf("--- im ---\n");
	for (int j=0; j<m_; j++) {
		for (int i=0; i<n_; i++) {
			printf("[");
			if (im[i+j*n_] < (uint8_t)100) {
				printf(" ");
			}
			if (im[i+j*n_] < (uint8_t)10) {
				printf(" ");
			}
			printf("%d]", im[i+j*n_]);
		}
		printf("\n");
	}
#endif
	double m = m_;
	double n = n_;
	double l = m*n;
	DFT dft;
	if (m*n != l) 
		return false;
	double* img = new double[sizeof(double)*m*n];
	if (img == NULL) 
		return false;
	for (int i=0; i<l; i++) {
		img[i] = (double)im[i];
	}
	printf("DFT...\n");
	dft.dft2(img, m, n);

	printf("SPCTRUM...\n");
	double* spec = NULL;
	double w;
	double h;
	dft.spectrum(spec, &w, &h, false);
	//dft.fftshift(spec, w, h);
#ifdef PRINT_DFT_IDFT //print
	for (int v=0; v<(int)h; v++) {
		for (int u=0; u<(int)w; u++) {
			printf("[");
			if (spec[u+v*(int)n] < 10000) printf(" ");
			if (spec[u+v*(int)n] < 1000) printf(" ");
			if (spec[u+v*(int)n] < 100) printf(" ");
			if (spec[u+v*(int)n] < 10) printf(" ");
			printf("%.1f]", spec[u+v*(int)n]);
		}
		printf("\n");
	}
#endif

	printf("REAL...\n");
	double* real = new double[sizeof(double)*m*n];
	for (int v=0; v<(int)m; v++) {
		for (int u=0; u<(int)n; u++) {
			real[u+v*(int)n] = dft.matrix[u+v*(int)n]._Val[0];
		}
	}

	printf("IDFT...\n");
	double *output_idft = NULL;
	dft.idft2(output_idft, &w, &h);
	uint8_t* output = new uint8_t[l];
	if (output == NULL || output_idft == NULL)
		return false;
	for (int i=0; i<l; i++) {
		if (output+i != NULL) {
			output[i] = (uint8_t)((output_idft[i]));
		}
	}

#ifdef PRINT_DFT_IDFT
	printf("--- output ---\n");
	for (int j=0; j<m_; j++) {
		for (int i=0; i<n_; i++) {
			printf("[");
			if (output[i+j*n_] < 100) printf(" ");
			if (output[i+j*n_] < 10) printf(" ");
			printf("%d]", output[i+j*n_]);
		}
		printf("\n");
	}
#endif

	ofstream fos;
	try {
		string file_name_raw = "";
		string file_name_raw_path = file.substr(0, file.find_last_of("/\\"));
		string file_name_raw_file = file.substr(file.find_last_of("/\\"));
		string file_name_raw_file_to_matlab = file_name_raw_file.substr(0, file_name_raw_file.find_last_of("/.")) + "Ift.raw";
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
		fos.write((char*)output, l*sizeof(uint8_t));
		fos.close();
		printf("output file:%s\n", file_name_raw.c_str());
	
		file_name_raw = "";
		file_name_raw_path = file.substr(0, file.find_last_of("/\\"));
		file_name_raw_file = file.substr(file.find_last_of("/\\"));
		file_name_raw_file_to_matlab = file_name_raw_file.substr(0, file_name_raw_file.find_last_of("/.")) + "Spec.raw";
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
		if (spec)
			fos.write((char*)spec, l*sizeof(double));
		fos.close();
		printf("output file:%s\n", file_name_raw.c_str());

		file_name_raw = "";
		file_name_raw_path = file.substr(0, file.find_last_of("/\\"));
		file_name_raw_file = file.substr(file.find_last_of("/\\"));
		file_name_raw_file_to_matlab = file_name_raw_file.substr(0, file_name_raw_file.find_last_of("/.")) + "_spec.raw";
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
		if (real)
			fos.write((char*)real, l*sizeof(double));
		fos.close();
		printf("output file:%s\n", file_name_raw.c_str());
	}
	catch (exception ex) {
		return false;
	}

	if (im)
		delete[] im;
	if (img)
		delete[] img;
	if (output)
		delete[] output;
	if (output_idft)
		delete[] output_idft;
	if (spec)
		delete[] spec;
	if (real)
		delete[] real;

	return true;
}

bool PGM::low_pass_eff(int m_, int n_, int window) {
	printf("LOW PASS...\n");
	if (raw == NULL)	return false;
	if (size <= 0)	return false;
	if (m_*n_ > size) return false;
	if (window > m_ || window > n_) return false;

	uint8_t* im = new uint8_t[m_*n_];
	if (im == NULL)
		return false;
	memset(im, 0, m_*n_);
	int k = 0;

	for (int j=(height/2-m_/2); j<(height/2+m_/2); j++) {
		for (int i=(width/2-n_/2); i<(width/2+n_/2); i++) {
			if (im+k) {
				*(im+k) = raw[(i+j*width)];
				k++;
			}
			else 
				return false;
		}
	}

#ifdef PRINT_DFT_IDFT
	printf("--- im ---\n");
	for (int j=0; j<m_; j++) {
		for (int i=0; i<n_; i++) {
			printf("[");
			if (im[i+j*n_] < (uint8_t)100) {
				printf(" ");
			}
			if (im[i+j*n_] < (uint8_t)10) {
				printf(" ");
			}
			printf("%d]", im[i+j*n_]);
		}
		printf("\n");
	}
#endif
	double m = m_;
	double n = n_;
	double l = m*n;
	DFT dft;
	if (m*n != l) 
		return false;
	double* img = new double[sizeof(double)*m*n];
	if (img == NULL) 
		return false;
	for (int i=0; i<l; i++) {
		img[i] = (double)im[i];
	}
	printf("DFT...\n");
	dft.dft2(img, m, n, (double)window);

	printf("SPCTRUM...\n");
	double* spec = NULL;
	double w;
	double h;
	dft.spectrum(spec, &w, &h, false);
	//dft.fftshift(spec, w, h);
#ifdef PRINT_DFT_IDFT //print
	for (int v=0; v<(int)h; v++) {
		for (int u=0; u<(int)w; u++) {
			printf("[");
			if (spec[u+v*(int)n] < 10000) printf(" ");
			if (spec[u+v*(int)n] < 1000) printf(" ");
			if (spec[u+v*(int)n] < 100) printf(" ");
			if (spec[u+v*(int)n] < 10) printf(" ");
			printf("%.1f]", spec[u+v*(int)n]);
		}
		printf("\n");
	}
#endif

	printf("IDFT...\n");
	double *output_idft = NULL;
	dft.idft2(output_idft, &w, &h, (double)window);
	uint8_t* output = new uint8_t[l];
	if (output == NULL || output_idft == NULL)
		return false;
	for (int i=0; i<l; i++) {
		if (output+i != NULL) {
			output[i] = (uint8_t)((output_idft[i]));
		}
	}

#ifdef PRINT_DFT_IDFT
	printf("--- output ---\n");
	for (int j=0; j<m_; j++) {
		for (int i=0; i<n_; i++) {
			printf("[");
			if (output[i+j*n_] < 100) printf(" ");
			if (output[i+j*n_] < 10) printf(" ");
			printf("%d]", output[i+j*n_]);
		}
		printf("\n");
	}
#endif

	ofstream fos;
	try {	
		string file_name_raw = "";
		string file_name_raw_path = file.substr(0, file.find_last_of("/\\"));
		string file_name_raw_file = file.substr(file.find_last_of("/\\"));
		string file_name_raw_file_to_matlab = file_name_raw_file.substr(0, file_name_raw_file.find_last_of("/.")) + ".raw";
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
		fos.write((char*)im, m_*n_*sizeof(uint8_t));
		fos.close();
		printf("output file:%s\n", file_name_raw.c_str());

		file_name_raw = "";
		file_name_raw_path = file.substr(0, file.find_last_of("/\\"));
		file_name_raw_file = file.substr(file.find_last_of("/\\"));
		file_name_raw_file_to_matlab = file_name_raw_file.substr(0, file_name_raw_file.find_last_of("/.")) + "IftLP.raw";
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
		fos.write((char*)output, l*sizeof(uint8_t));
		fos.close();
		printf("output file:%s\n", file_name_raw.c_str());
	
		file_name_raw = "";
		file_name_raw_path = file.substr(0, file.find_last_of("/\\"));
		file_name_raw_file = file.substr(file.find_last_of("/\\"));
		file_name_raw_file_to_matlab = file_name_raw_file.substr(0, file_name_raw_file.find_last_of("/.")) + "SpecLP.raw";
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
		if (spec)
			fos.write((char*)spec, l*sizeof(double));
		fos.close();
		printf("output file:%s\n", file_name_raw.c_str());
	}
	catch (exception ex) {
		return false;
	}

	if (im)
		delete[] im;
	if (img)
		delete[] img;
	if (output)
		delete[] output;
	if (output_idft)
		delete[] output_idft;
	if (spec)
		delete[] spec;

	return true;
}

bool PGM::filtering(double* kernel, int window) {
	if (raw == NULL)	return false;
	if (size <= 0)	return false;
	if (window > height || window > width) return false;

	Filter filter;
	double* filtered = NULL;
	uint8_t* output = new uint8_t[sizeof(uint8_t)*width*height];
	double* src = new double[sizeof(double)*width*height];
	if (src == NULL) return false;
	memset(src, 0, sizeof(double)*width*height);
	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			src[i+j*width] = raw[i+j*width];
		}
	}

	if (!(filter.convolution(filtered, src, kernel, width, height, window))) {
		return false;
	}
	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			output[i+j*width] = (uint8_t)filtered[i+j*width];
		}
	}

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
		fos.write((char*)output, width*height*sizeof(uint8_t));
		fos.close();
		printf("output file:%s\n", file_name_raw.c_str());
	}
	catch (exception ex) {
		return false;
	}

	if (src)
		delete[] src;
	if (filtered)
		delete[] filtered;

	return true;
}