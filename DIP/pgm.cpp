	/******************************************************************************
	**                      INCLUDE
	*******************************************************************************/
#include "pgm.h"
#include "dft.h"
#include "dct.h"
#include "filter.h"


	/******************************************************************************
	**                      DEFINE
	*******************************************************************************/
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
	sprintf(buf_print, "[%s][%s]", "PGM", func);
	pos_print = strlen(buf_print);
	vsprintf(buf_print+pos_print, xx_fmt, args);
	printf(buf_print);
	va_end(args);
}

PGM::PGM() {
	//cout << "[PGM::PGM()]:" << endl;
	type = 0;
	width = 0;
	height = 0;
	depth = 0;
	size = 0;
	raw = NULL;
	I = NULL;
	width_I = 0;
	height_I = 0;
	raw = NULL;
	real = NULL;
	imag = NULL;
	spec = NULL;
	raw_IDFT = NULL;
	raw_filtered = NULL;
	w_filtered = 0;
	h_filtered = 0;
	Y= NULL;
	raw_IDCT = NULL;
	width_Y = 0;
	height_Y = 0;
}

PGM::~PGM() {
	//cout << "[PGM::~PGM()]:" << endl;
	if (raw) {
		free(raw);
		raw = NULL;
	}
	if (I) {
		free(I);
		I = NULL;
	}
	if (raw_IDFT) {
		free(raw_IDFT);
		raw_IDFT = NULL;
	}
	if (raw_filtered) {
		free(raw_filtered);
		raw_filtered = NULL;
	}
}

bool PGM::read(string file_name_assigned, uint8_t*& data, int* size) {
	ifstream fis;
	ofstream fos;

	fis.open(file_name_assigned.c_str(), ios::in | ios::binary);
	if (fis.fail()) {
		LOG("%s\n", strerror(errno));
		return false;
	}

#if 0
	string file_name = "C:\\src\\amo\\DIP\\Debug\\pgm.raw";
#else
	string file_name = "";
	string file_name_path = file_name_assigned.substr(0, file_name_assigned.find_last_of("/\\"));
	string file_name_file = file_name_assigned.substr(file_name_assigned.find_last_of("/\\"));
	string file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/."));
	file_name += file_name_path;
	file_name += file_name_file_to_matlab;
	file_name += "";
#endif
	remove(file_name.c_str());
	fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
	if (fos.fail()) {
		LOG("%s\n", strerror(errno));
		return false;
	}
	else fos.clear();
	file = file_name;

	fis.seekg(0, ios::beg);
	string line; 
	//extract
	for (int i=0; i<3; i++) {
		if (getline(fis, line).fail()) {
			return false;
		}
		//LOG("line:%s\n", line.c_str());
		if (i == 0) {
			char type_c1;
			char type_c2;
			if (sscanf(line.c_str(), "%c%c",&type_c1, &type_c2) <= 0) {
				LOG("sscanf() failed\n");
				return false;
			}
			if (type_c1 != 'P') {
				LOG("type_c1 != 'P'\n");
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
	LOG("Extract raw data from %s\n", file_name.c_str());
	LOG("type  :P%c\n", this->type);
	LOG("width :%d\n", this->width);
	LOG("height:%d\n", this->height);
	LOG("depth :%d\n", this->depth);
	LOG("size  :%d\n", this->size);
#endif

	//extract data
	//fis.read((char*)raw, width*height*depth);
	
	raw = (uint8_t*)malloc(width*height*depth);
	memset(raw, 0, width*height*depth);
	//output
	uint16_t s = 0;
	uint8_t c = 0;
#if 1 //cast but truncate
	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			if (depth ==2) {
				fis.read((char*)((uint16_t*)raw+i+j*width), depth);
				s = *((uint16_t*)((uint16_t*)raw+i+j*width));
#if 0 //special format of fp device 
				((uint16_t*)(raw))[i+j*width] &= 0xff;
				s &= 0xff;
#endif				
				fos.write((char*)&s, depth);
			}
			else if (depth == 1) {
				fis.read((char*)((uint8_t*)raw+i+j*width), depth);
				c = *((uint8_t*)((uint8_t*)raw+i+j*width));
				fos.write((char*)&c, 1);
			}
			else {
				LOG("Not supported depth\n");
				return false;
			}
			
			//if (i == width) fos << endl;
			//if (i == height) fos << endl;
		}
	}
#else

#endif
	LOG("[Raw image file]:%s\n", file_name.c_str());
	fis.close();
	fos.close();

	return true;
}

bool PGM::write(char* filename, char* data, int w, int h, int bitsOfPixel) {
	FILE* pf = NULL;
	char str[128];
	int  stamp = time(NULL)%1000;
	char tmp[16];
	sprintf(tmp, "%d", stamp);
	tmp[strlen(tmp)] = '\0';
	strncpy(str, filename, 128);
	strncat(str, tmp, strlen(tmp));
	strncat(str, ".PGM", strlen(".PGM"));
	pf = fopen(str, "wb");

	switch (bitsOfPixel) {
	case (8):
		fprintf(pf, "P5\n%d %d\n255\n", w, h);
		break;
	case (16):
		fprintf(pf, "P5\n%d %d\n65535\n", w, h);
		break;
	default:
		break;
	}

	fwrite(data, 1, (bitsOfPixel/8)*w*h, pf);
	fclose(pf);
	printf("[PGM file]:%s\n", str);
	return true;
}


	/******************************************************************************
	**                      DFT
	*******************************************************************************/
bool PGM::dft(complex<double>** dst, int n_, int m_) {
	return this->dft(dst, n_, m_, n_, false);
}

bool PGM::dft(complex<double>** dst, int n_, int m_, int window) {
	return this->dft(dst, n_, m_, window, false);
}

bool PGM::dft(complex<double>** dst, int n_, int m_, int window, bool dft_by_raw_filtered) {
	return dft(dst, n_, m_, window, dft_by_raw_filtered, false);
}

bool PGM::dft(complex<double>** dst, int n_, int m_, int window, bool dft_by_raw_filtered, bool do_hist) {
	if (raw == NULL) return false;
	if (size <= 0)	return false;
	if (n_*m_ > size) return false;
	if (depth != 1 && depth!= 2) return false;
	if (dft_by_raw_filtered && raw_filtered == NULL)
		return false;
	if (dft_by_raw_filtered && (n_ != w_filtered || m_ != h_filtered))
		return false;	
	else 
		LOG("Using raw data filtered (%dx%d)\n", n_, m_);

	void* im = NULL;
	if (depth == 1) {
		im = malloc(sizeof(uint8_t)*n_*m_);
	}
	else if (depth == 2){
		im = malloc(sizeof(uint16_t)*n_*m_);
	}
	memset(im, 0, n_*m_*depth);

	int k = 0;
	for (int j=0; j<m_; j++) {
		for (int i=0; i<n_; i++) {
			if (depth == 2) {
				if ((uint16_t*)im+k) {
					if (!dft_by_raw_filtered)
						((uint16_t*)im)[k] = ((uint16_t*)raw)[(i+j*width)];
					else
						((uint16_t*)im)[k] = raw_filtered[(i+j*w_filtered)];
				}
				else {
					return false;
				}
			}
			else if (depth == 1) {
				if ((uint8_t*)im+k) {
					if (!dft_by_raw_filtered)
						((uint8_t*)im)[k] = ((uint8_t*)raw)[(i+j*width)];
					else
						((uint8_t*)im)[k] = raw_filtered[(i+j*w_filtered)];
				}
				else {
					return false;
				}
			} 
			k++;			
		}
	}

	double n = n_;
	double m = m_;
	double l = n*m;
	double* img = (double*)malloc(sizeof(double)*n*m);
	memset(img, 0 , sizeof(double)*n*m);
	if (img == NULL) 
		return false;
	for (int i=0; i<l; i++) {
		if (depth == 2) {
			if (!dft_by_raw_filtered)
				img[i] = (double)((uint16_t*)im)[i];
			else
				img[i] = raw_filtered[i];
		}
		else if (depth == 1) {
			if (!dft_by_raw_filtered)
				img[i] = (double)((uint8_t*)im)[i];
			else
				img[i] = raw_filtered[i];
		}
	}

	if (do_hist) {
		Filter f;
		f.histogram_equalization(img, n*m);
	}

	LOG("DFT...\n");
	DFT dft;
	if (I) {
		free(I);
		I = NULL;
	}
	I = (complex<double>*)malloc(sizeof(complex<double>)*n_*m_);
	if (I == NULL)
		return false;
	if (!dft.dft2(&I, img, n, m, window)) {
		LOG("dft2 failed\n");
		return false;
	}
	width_I = n_;
	height_I = m_;

	//copy to callee
	if (dst) {
		memcpy(*dst, I, sizeof(complex<double>)*n_*m_);
	}

	LOG("SPCTRUM...\n");
	double w;
	double h;
	dft.spectrum(spec, &w, &h, false);
	//dft.fftshift(spec, w, h);

	LOG("REAL...\n");
	real = (double*)malloc(sizeof(double)*n_*m_);
	for (int v=0; v<(int)m; v++) {
		for (int u=0; u<(int)n; u++) {
			//real[u+v*(int)n] = dft.matrix[u+v*(int)n]._Val[0];
			real[u+v*(int)n] = I[u+v*(int)n]._Val[0];
		}
	}

	LOG("IMAG...\n");
	imag = (double*)malloc(sizeof(double)*n_*m_);
	for (int v=0; v<(int)m; v++) {
		for (int u=0; u<(int)n; u++) {
			imag[u+v*(int)n] = I[u+v*(int)n]._Val[1];
		}
	}

	ofstream fos;
	try {
		string file_name = "";
		string file_name_path = file.substr(0, file.find_last_of("/\\"));
		string file_name_file = file.substr(file.find_last_of("/\\"));
		string file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "_im_PGMDft.raw";
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
		fos.write((char*)im, sizeof(uint8_t)*n*m*depth);
		fos.close();
		LOG("[%dx%d Image by uint8 file]:%s\n", (int)n, (int)m, file_name.c_str());

		file_name = "";
		file_name_path = file.substr(0, file.find_last_of("/\\"));
		file_name_file = file.substr(file.find_last_of("/\\"));
		file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "_im_PGMDft_Double.raw";
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
		if (img)
			fos.write((char*)img, sizeof(double)*n*m);
		fos.close();
		LOG("[%dx%d Image by double file]:%s\n", (int)n, (int)m, file_name.c_str());

		file_name = "";
		file_name_path = file.substr(0, file.find_last_of("/\\"));
		file_name_file = file.substr(file.find_last_of("/\\"));
		file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "DFTSpec.raw";
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
			fos.write((char*)spec, l*sizeof(double));
		fos.close();
		LOG("[DFT spec file]:%s\n", file_name.c_str());

		file_name = "";
		file_name_path = file.substr(0, file.find_last_of("/\\"));
		file_name_file = file.substr(file.find_last_of("/\\"));
		file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "DFTReal.raw";
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
		if (real)
			fos.write((char*)real, l*sizeof(double));
		fos.close();
		LOG("[DFT real file]:%s\n", file_name.c_str());

		file_name = "";
		file_name_path = file.substr(0, file.find_last_of("/\\"));
		file_name_file = file.substr(file.find_last_of("/\\"));
		file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "DFTImag.raw";
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
		if (real)
			fos.write((char*)imag, l*sizeof(double));
		fos.close();
		LOG("[DFT imaginary file]:%s\n", file_name.c_str());
	}
	catch (exception ex) {
		return false;
	}

	if (im)
		free(im);
	if (img)
		free(img);
	if (real)
		free(real);
	if (imag)
		free(imag);

	return true;
}

bool PGM::idft(complex<double>* src, int n_, int m_, bool output_real) {
	if (raw == NULL)  return false;
	if (size <= 0)  return false;
	if (width_I <= 0)  return false;
	if (height_I <= 0)  return false;

	double w = width_I;
	double h = height_I;
	int l = m_*n_;
	
	LOG("IDFT...\n");
	double *output_idft = NULL;
	uint8_t* output = NULL;
	if (src == NULL) {
		if (I == NULL)  return false;  
		DFT dft(width_I, height_I, I);
		dft.idft2(output_idft, &w, &h, output_real);
		if (output_idft == NULL)
		return false;
	}
	else {
		DFT dft(width_I, height_I, src);
		dft.idft2(output_idft, &w, &h, output_real);
		if (output_idft == NULL)
		return false;
	}
	
	Filter filter;
#if 0
	if (!(filter.normalize(output, output_idft, n_, m_, true))) {
		return false;
	}
#else
	if (!(filter.cast(output, output_idft, n_, m_))) {
		return false;
	}
#endif

	ofstream fos;
	try {
		string file_name = "";
		string file_name_path = file.substr(0, file.find_last_of("/\\"));
		string file_name_file = file.substr(file.find_last_of("/\\"));
		string file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "IDFT.raw";
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
		fos.write((char*)output, l*sizeof(uint8_t));
		fos.close();
		LOG("[%dx%d IDFT file]:%s\n", n_, m_, file_name.c_str());		
	}
	catch (exception ex) {
		return false;
	}

	if (output)
		free(output);

	if (output_idft)
		free(output_idft);

	return true;
}

bool PGM::dft_idft(int n_, int m_) {
	if (raw == NULL)	return false;
	if (size <= 0)	return false;
	if (n_*m_ > size) return false;
	if (depth != 1 && depth!= 2) return false;

	void* im = NULL;
	if (depth == 1) {
		im = malloc(n_*m_*depth);
	}
	else if (depth == 2){
		im = malloc(n_*m_*depth);
	}
	memset(im, 0, n_*m_*depth);

	int w_aliasing = n_-((width/2+n_/2)-(width/2-n_/2));
	int h_aliasing = m_-((height/2+m_/2)-(height/2-m_/2));
	if (w_aliasing || h_aliasing)
		LOG("width aliasing:%d, height aliasing:%d\n", w_aliasing, h_aliasing);

	int k = 0;
	for (int j=(height/2-m_/2); j<(height/2+m_/2)+h_aliasing; j++) {
		for (int i=(width/2-n_/2); i<(width/2+n_/2)+w_aliasing; i++) {
			if (depth == 2) {
				if ((uint16_t*)im+k) {
					((uint16_t*)im)[k] = ((uint16_t*)raw)[(i+j*width)];
				}
				else {
					return false;
				}
			}
			else if (depth == 1) {
				if ((uint8_t*)im+k) {
					((uint8_t*)im)[k] = ((uint8_t*)raw)[(i+j*width)];
				}
				else {
					return false;
				}
			} 
			k++;			
		}
	}

#ifdef PRINT_DFT_IDFT
	LOG("--- im ---\n");
	for (int j=0; j<m_; j++) {
		for (int i=0; i<n_; i++) {
			LOG("[");
			if (im[i+j*n_] < (uint8_t)100) {
				LOG(" ");
			}
			if (im[i+j*n_] < (uint8_t)10) {
				LOG(" ");
			}
			LOG("%d]", im[i+j*n_]);
		}
		LOG("\n");
	}
#endif
	double m = m_;
	double n = n_;
	double l = n*m;
	DFT dft;
	double* img = (double*)malloc(sizeof(double)*n_*m_);
	if (img == NULL) 
		return false;
	for (int i=0; i<l; i++) {
		if (depth == 2) {
			img[i] = (double)((uint16_t*)im)[i];
		}
		else if (depth == 1) {
			img[i] = (double)((uint8_t*)im)[i];
		}
	}
	LOG("DFT...\n");
	if (I) {
		free(I);
		I = NULL;
	}
	I = (complex<double>*)malloc(sizeof(complex<double>)*n_*m_);
	if (I == NULL)
		return false;
	dft.dft2(&I, img, n, m);
	width_I = n_;
	height_I = m_;

	LOG("SPCTRUM...\n");
	double* spec = NULL;
	double w;
	double h;
	dft.spectrum(spec, &w, &h, false);
	//dft.fftshift(spec, w, h);
#ifdef PRINT_DFT_IDFT //print
	for (int v=0; v<(int)h; v++) {
		for (int u=0; u<(int)w; u++) {
			LOG("[");
			if (spec[u+v*(int)n] < 10000) LOG(" ");
			if (spec[u+v*(int)n] < 1000) LOG(" ");
			if (spec[u+v*(int)n] < 100) LOG(" ");
			if (spec[u+v*(int)n] < 10) LOG(" ");
			LOG("%.1f]", spec[u+v*(int)n]);
		}
		LOG("\n");
	}
#endif

	LOG("REAL...\n");
	real = (double*)malloc(sizeof(double)*n_*m_);
	for (int v=0; v<(int)m; v++) {
		for (int u=0; u<(int)n; u++) {
			real[u+v*(int)n] = I[u+v*(int)n]._Val[0];
		}
	}

	LOG("IMAG...\n");
	imag = (double*)malloc(sizeof(double)*n_*m_);
	for (int v=0; v<(int)m; v++) {
		for (int u=0; u<(int)n; u++) {
			imag[u+v*(int)n] = I[u+v*(int)n]._Val[1];
		}
	}

	LOG("IDFT of complex real...\n");
	double*  output_idft_real = NULL;
	uint8_t* output_real = NULL;
	dft.idft2(output_idft_real, &w, &h, true);
	if (output_idft_real == NULL)
		return false;
	Filter filter;
#if 0	
	if (!(filter.normalize(output_real, output_idft_real, n_, m_, true))) {
		return false;
	}
#else
	if (!(filter.cast(output_real, output_idft_real, n_, m_))) {
		return false;
	}
#endif

#ifdef PRINT_DFT_IDFT
	LOG("--- output_real ---\n");
	for (int j=0; j<m_; j++) {
		for (int i=0; i<n_; i++) {
			LOG("[");
			if (output[i+j*n_] < 100) LOG(" ");
			if (output[i+j*n_] < 10) LOG(" ");
			LOG("%d]", output[i+j*n_]);
		}
		LOG("\n");
	}
#endif

	LOG("IDFT of complex imaginary...\n");
	double*  output_idft_imag = NULL;
	uint8_t* output_imag = NULL;
	dft.idft2(output_idft_imag, &w, &h, false);
	if (output_idft_imag == NULL)
		return false;
#if 1
	if (!(filter.normalize(output_imag, output_idft_imag, n_, m_, true))) {
		return false;
	}
#else
	if (!(filter.cast(output_imag, output_idft_imag, n_, m_))) {
		return false;
	}
#endif

	ofstream fos;
	try {
		string file_name = "";
		string file_name_path = file.substr(0, file.find_last_of("/\\"));
		string file_name_file = file.substr(file.find_last_of("/\\"));
		string file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "_im_DftIDft.raw";
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
		fos.write((char*)im, n*m*sizeof(uint8_t)*depth);
		fos.close();
		LOG("[Image file]:%s\n", file_name.c_str());

		file_name = "";
		file_name_path = file.substr(0, file.find_last_of("/\\"));
		file_name_file = file.substr(file.find_last_of("/\\"));
		file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "IDFT_FromComplexReal.raw";
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
		fos.write((char*)output_real, l*sizeof(uint8_t));
		fos.close();
		LOG("[IDFT file]:%s\n", file_name.c_str());

		file_name = "";
		file_name_path = file.substr(0, file.find_last_of("/\\"));
		file_name_file = file.substr(file.find_last_of("/\\"));
		file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "IDFT_FromComplexImag.raw";
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
		fos.write((char*)output_imag, l*sizeof(uint8_t));
		fos.close();
		LOG("[IDFT file]:%s\n", file_name.c_str());
	
		file_name = "";
		file_name_path = file.substr(0, file.find_last_of("/\\"));
		file_name_file = file.substr(file.find_last_of("/\\"));
		file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "Spec.raw";
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
			fos.write((char*)spec, l*sizeof(double));
		fos.close();
		LOG("[DFT spectrum file]:%s\n", file_name.c_str());

		file_name = "";
		file_name_path = file.substr(0, file.find_last_of("/\\"));
		file_name_file = file.substr(file.find_last_of("/\\"));
		file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "DFTReal.raw";
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
		if (real)
			fos.write((char*)real, l*sizeof(double));
		fos.close();
		LOG("[DFT real file]:%s\n", file_name.c_str());
	}
	catch (exception ex) {
		return false;
	}

	if (im)
		free(im);
	if (img)
		free(img);
	if (real)
		free(real);
	if (imag)
		free(imag);
	if (output_real)
		free(output_real);
	if (output_idft_real)
		free(output_idft_real);
	if (output_imag)
		free(output_imag);
	if (output_idft_imag)
		free(output_idft_imag);
	if (spec)
		free(spec);

	return true;
}

bool PGM::low_pass_eff(int n_, int m_, int window) {
	LOG("LOW PASS...\n");
	if (raw == NULL)	return false;
	if (size <= 0)	return false;
	if (n_*m_ > size) return false;
	if (window > n_ || window > m_) return false;
	if (depth != 1 && depth!= 2) return false;

	void* im = NULL;
	if (depth == 1) {
		im = malloc(n_*m_*depth);
	}
	else if (depth == 2){
		im = malloc(n_*m_*depth);
	}
	memset(im, 0, n_*m_*depth);
	int w_aliasing = n_-((width/2+n_/2)-(width/2-n_/2));
	int h_aliasing = m_-((height/2+m_/2)-(height/2-m_/2));
	if (w_aliasing || h_aliasing)
		LOG("width aliasing:%d, height aliasing:%d", w_aliasing, h_aliasing);

	int k = 0;
	for (int j=(height/2-m_/2); j<(height/2+m_/2)+h_aliasing; j++) {
		for (int i=(width/2-n_/2); i<(width/2+n_/2)+w_aliasing; i++) {
			if (depth == 2) {
				if ((uint16_t*)im+k) {
					((uint16_t*)im)[k] = ((uint16_t*)raw)[(i+j*width)];
				}
				else {
					return false;
				}
			}
			else if (depth == 1) {
				if ((uint8_t*)im+k) {
					((uint8_t*)im)[k] = ((uint8_t*)raw)[(i+j*width)];
				}
				else {
					return false;
				}
			} 
			k++;			
		}
	}

	double n = n_;
	double m = m_;
	double l = n*m;
	DFT dft;
	if (n*m != l) 
		return false;
	double* img = (double*)malloc(sizeof(double)*n*m);
	if (img == NULL) 
		return false;
	for (int i=0; i<l; i++) {
		if (depth == 2) {
			img[i] = (double)((uint16_t*)im)[i];
		}
		else if (depth == 1) {
			img[i] = (double)((uint8_t*)im)[i];
		}
	}
	LOG("DFT...\n");
	dft.dft2(NULL, img, n, m, (double)window);

	LOG("SPCTRUM...\n");
	double* spec = NULL;
	double w;
	double h;
	dft.spectrum(spec, &w, &h, false);
	//dft.fftshift(spec, w, h);

	LOG("IDFT...\n");
	double *output_idft = NULL;
	dft.idft2(output_idft, &w, &h, (double)window);
	uint8_t* output = NULL;
	if (output_idft == NULL)
		return false;

	Filter filter;
#if 0	
	if (!(filter.normalize(output, output_idft, n_, m_, true))) {
		return false;
	}
#else
	if (!(filter.cast(output, output_idft, n_, m_))) {
		return false;
	}
#endif

	ofstream fos;
	try {	
		string file_name = "";
		string file_name_path = file.substr(0, file.find_last_of("/\\"));
		string file_name_file = file.substr(file.find_last_of("/\\"));
		string file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "_im_LP.raw";
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
		fos.write((char*)im, (n*m*sizeof(uint8_t)*depth));
		fos.close();
		LOG("[LP filtered file]:%s\n", file_name.c_str());

		file_name = "";
		file_name_path = file.substr(0, file.find_last_of("/\\"));
		file_name_file = file.substr(file.find_last_of("/\\"));
		file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "IDFTLP.raw";
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
		fos.write((char*)output, l*sizeof(uint8_t));
		fos.close();
		LOG("[IDFT specturm of LP filtered file]:%s\n", file_name.c_str());
	
		file_name = "";
		file_name_path = file.substr(0, file.find_last_of("/\\"));
		file_name_file = file.substr(file.find_last_of("/\\"));
		file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "SpecLP.raw";
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
			fos.write((char*)spec, l*sizeof(double));
		fos.close();
		LOG("[DFT specturm of LP filtered file]:%s\n", file_name.c_str());
	}
	catch (exception ex) {
		return false;
	}

	if (im)
		free(im);
	if (img)
		free(img);
	if (output)
		free(output);
	if (output_idft)
		free(output_idft);
	if (spec)
		free(spec);

	return true;
}

bool PGM::hist(double** dst, bool update_raw) {
	if (raw == NULL)	return false;
	if (size <= 0 || width <= 0 || height <= 0)	return false;
	if (depth != 1 && depth!= 2) return false;
	double* tmp = NULL;
	double* tmp_norm = NULL;
	tmp = (double*)malloc(sizeof(double)*width*height);
	if (tmp == NULL) return false;
	memset(tmp, 0, sizeof(double)*width*height);
	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			if (depth == 2) {
				tmp[i+j*width] = ((uint16_t*)raw)[(i+j*width)];
			}
			else if (depth == 1) {
				tmp[i+j*width] = ((uint8_t*)raw)[(i+j*width)];
			} 
		}
	}
	Filter f;
	if (!(f.histogram_equalization(tmp, width*height)))
		return false;
	
	if (dst) {
		if (*dst) {
			memcpy(*dst, tmp, sizeof(double)*width*height);
		}
	}

	if (update_raw) {
		Filter f;
		tmp_norm = (double*)malloc(sizeof(double)*width*height);
		if (tmp_norm == NULL) return false;
		memset(tmp_norm, 0, sizeof(double)*width*height);
		f.normalize(tmp_norm, tmp, width, height, 0, 255);
		for (int i=0; i<width*height; i++) {
			if (depth == 2) {
				((uint16_t*)this->raw)[i] = tmp_norm[i];
			}
			else if (depth ==  1) {
				((uint8_t*)this->raw)[i] = tmp_norm[i];
			}
			else {
				return false;
			}
		}
	}

	ofstream fos;
	try {
		string file_name = "";
		string file_name_path = file.substr(0, file.find_last_of("/\\"));
		string file_name_file = file.substr(file.find_last_of("/\\"));
		string file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "_hist.raw";
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
		fos.write((char*)tmp, (double)width*(double)height*sizeof(double));
		fos.close();
		LOG("[%dx%d Histogram equalized image file]:%s\n", width, height, file_name.c_str());

		if (update_raw) {
			file_name = "";
			file_name_path = file.substr(0, file.find_last_of("/\\"));
			file_name_file = file.substr(file.find_last_of("/\\"));
			file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "_im_histed.raw";
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
			fos.write((char*)this->raw, (double)width*(double)height*depth);
			fos.close();
			LOG("[%dx%d Raw histogram equalized image file]:%s\n", width, height, file_name.c_str());
		}
	}
	catch (exception ex) {
		return false;
	}
	if (tmp)
		free(tmp);
	if (tmp_norm)
		free(tmp_norm);
	return true;
}

bool PGM::filtering_by_DFT(double**dst, double* kernel, complex<double>* kernel_DFT, int w_kernel, int h_kernel, int window, bool dft_by_raw_filtered, bool do_inverse_transform) {
	return filtering_by_DFT(dst, kernel, kernel_DFT, w_kernel, h_kernel, window,  dft_by_raw_filtered, do_inverse_transform, 100.0);
}

bool PGM::filtering_by_DFT(double**dst, double* kernel, complex<double>* kernel_DFT, int w_kernel, int h_kernel, int window, bool dft_by_raw_filtered, bool do_inverse_transform, double nsr) {
	if (raw == NULL)	
		return false;
	if (size <= 0)	
		return false;
	if (depth != 1 && depth!= 2) 
		return false;
	if (window > (w_kernel/2) && window > (h_kernel/2)) 
		return false;
	string file_name = "";
	ofstream fos;
	double* h = NULL;
	double* y = NULL;
	complex<double>* Y = NULL;
	complex<double>* I = NULL;
	complex<double>* H = NULL;
	double* spec_Y = NULL;
	double  w_spec_Y;
	double  h_spec_Y;
	double* spec_H = NULL;
	double  w_spec_H;
	double  h_spec_H;
	int n = 0;
	int m = 0;

	if (dft_by_raw_filtered) {
		n = this->w_filtered;
		m = this->h_filtered;
	}
	else {
		n = this->width;
		m = this->height;
	}

	if (kernel_DFT != NULL) {
		LOG("DFT of kernel assigned... (N:%d, M:%d)\n", n, m);
		H = kernel_DFT;
		DFT dft_kernel(n, m, H);
		if (!dft_kernel.spectrum(spec_H, H, &w_spec_H, &h_spec_H, false)) {
			LOG("spectrum failed\n");
			return false;
		}
		LOG("DFT of kernel assigned done (w_spec_H:%d, h_spec_H:%d)\n", (int)w_spec_H, (int)h_spec_H);
	}
	else {		
		LOG("Zero-padding on h... (N:%d, M:%d) to be (N:%d, M:%d)\n", w_kernel, h_kernel, n, m);
		Filter f;
		h = (double*)malloc(sizeof(double)*n*m);
		if (h == NULL)
			return false;
		memset(h, 0, sizeof(double)*n*m);
		if (!f.copy_center_around(h, kernel, n, m, w_kernel, h_kernel)) {
			LOG("copy_center_around failed\n");
			return false;
		}

		LOG("DFT of kernel... (N:%d, M:%d)\n", n, m);
		DFT dft_kernel;
		if (!dft_kernel.dft2(&H, h, n, m, window)) {
			LOG("dft2 failed\n");
			return false;
		}

		if (do_inverse_transform) {
			LOG("**** Do inverse transform ****\n");
			complex<double> c_H_tmp(0, 0);
			complex<double> c_H_conj(0, 0);
			complex<double> c_H_reciprocal(0, 0);
			double factor = 0.0; 
			double sqH = 0.0;
			//double nsr = 1.0/500.0;
			LOG("**** nsr:%f ****\n", nsr);
			for (int v=0; v<m; v++) {
				for (int u=0; u<n; u++) {
					//(1/H) = (H*).*(1/(H^2+nsr)) which is wiener
					if (H[u+v*n]._Val[0] == 0 && H[u+v*n]._Val[1] == 0) {
						H[u+v*n]._Val[0] = 1;
						H[u+v*n]._Val[1] = 0;
						//LOG("H[%d][%d]:(%.2f,%.2f)\n", u, v, H[u+v*n]._Val[0], H[u+v*n]._Val[1]);
						continue;
					}
					sqH = H[u+v*n]._Val[0]*H[u+v*n]._Val[0] + H[u+v*n]._Val[1]*H[u+v*n]._Val[1];
					factor = 1.0f/(sqH+nsr);
					c_H_tmp = H[u+v*n];
					c_H_conj = complex<double>(H[u+v*n]._Val[0], -1.0f*(H[u+v*n]._Val[1]));
					c_H_reciprocal = (c_H_conj) * factor;
					H[u+v*n] = c_H_reciprocal;
					if (H[u+v*n]._Val[0] > 0.0 && false) {
						LOG("H[%d][%d]:(%.2f,%.2f), c_conjH:(%.2f,%.2f), c:(%.2f,%.2f)\n"
							, u, v
							, H[u+v*n]._Val[0] , H[u+v*n]._Val[1]
							, c_H_conj._Val[0] , c_H_conj._Val[1]
							, c_H_tmp._Val[0]  , c_H_tmp._Val[1]);
					}
				}
			}
		}

		if (!dft_kernel.spectrum(spec_H, H, &w_spec_H, &h_spec_H, false)) {
			LOG("spectrum failed\n");
			return false;
		}
		LOG("DFT of kernel done (w_spec_H:%d, h_spec_H:%d)\n", (int)w_spec_H, (int)h_spec_H);
	}
	try {
		file_name = "";
		string file_name_path = file.substr(0, file.find_last_of("/\\"));
		string file_name_file = file.substr(file.find_last_of("/\\"));
		string file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "_filteringByDFT_kernel.raw";
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
		if (h)
			fos.write((char*)h, (double)n*(double)m*sizeof(double));
		fos.close();
		LOG("[%dx%d Kernel image file]:%s\n", n, m, file_name.c_str());
		
		file_name = "";
		file_name_path = file.substr(0, file.find_last_of("/\\"));
		file_name_file = file.substr(file.find_last_of("/\\"));
		file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "_filteringByDFT_kernel_DFT.raw";
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
		if (spec_H)
			fos.write((char*)spec_H, w_spec_H*h_spec_H*sizeof(double));
		fos.close();
		LOG("[%dx%d DFT of kernel image file]:%s\n", (int)w_spec_H, (int)h_spec_H, file_name.c_str());
	}
	catch (exception ex) {
		return false;
	}

	LOG("DFT of image... (N:%d, M:%d)\n", n, m);
	I = (complex<double>*)malloc(sizeof(complex<double>)*n*m);
	if (I == NULL)
		return false;
	memset(I, 0, sizeof(complex<double>)*n*m);
	if (!this->dft(&I, n, m, window, dft_by_raw_filtered, true)) {
		LOG("dft failed\n");
		return false;
	}
	LOG("DFT of image done\n");

	LOG("Product by I(N:%d, M:%d) .* H(N:%d, M:%d)...\n", n, m, n, m);
	Y = (complex<double>*)malloc(sizeof(complex<double>)*n*m);
	if (Y == NULL)
		return false;
	memset(Y, 0,sizeof(complex<double>)*n*m);
	Filter f_prod;
	if (!f_prod.element_wise_complex_multiply(Y, I, H, n, m)) {
		LOG("element_wise_complex_multiply failed\n");
		return false;
	}

	DFT dft_filtered(n,m,Y);
	if (!dft_filtered.spectrum(spec_Y, Y, &w_spec_Y, &h_spec_Y, false)) {
		LOG("spectrum failed\n");
		return false;
	}
	LOG("Product done\n");

	LOG("IDFT of product... (N:%d, M:%d)\n", n, m);
	y = (double*)malloc(sizeof(double)*n*m);
	if (y == NULL)
		return false;
	memset(y, 0, sizeof(double)*n*m);
	double w_idft = 0;
	double h_idft = 0;
	DFT dft_forY(n, m, Y);
	if (!dft_forY.idft2(y, &w_idft, &h_idft, window, true)) {
		LOG("idft2 failed\n");
		return false;
	}
	LOG("IDFT of product done (N:%d, M:%d)\n", (int)w_idft, (int)h_idft);

#if 1
	try {
		file_name = "";
		string file_name_path = file.substr(0, file.find_last_of("/\\"));
		string file_name_file = file.substr(file.find_last_of("/\\"));
		string file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "_filteringByDFT_filtered.raw";
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
		if (y)
			fos.write((char*)y, (double)w_idft*(double)h_idft*sizeof(double));
		fos.close();
		LOG("[%dx%d Filtered image file]:%s\n", (int)w_idft, (int)h_idft, file_name.c_str());

		file_name = "";
		file_name_path = file.substr(0, file.find_last_of("/\\"));
		file_name_file = file.substr(file.find_last_of("/\\"));
		file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "_filteringByDFT_filtered_DFT.raw";
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
		if (spec_Y)
			fos.write((char*)spec_Y, (double)w_spec_Y*(double)h_spec_Y*sizeof(double));
		fos.close();
		LOG("[%dx%d DFT of filtered image file]:%s\n", (int)w_spec_Y, (int)h_spec_Y, file_name.c_str());
	}
	catch (exception ex) {
		return false;
	}
#endif


	if (h)
		free(h);
	if (H)
		free(H);	
	if (spec)
		free(spec);	
	return true;
}


/**
  * N*N DCT by (2N)*(2N) FFT
  * X_EXT = fft(x_ext);
  * k = (0:1:(2*N)-1);
  * W = cos(2*pi*(0.5*k)/(2*N))-i*sin(2*pi*(0.5*k)/(2*N));
  * factor = 0.5.*W;
  * X_DCT_2N = real(0.5.*W.*X_EXT);
  * X_DCT_N_scale = sqrt(2/N).*[sqrt(1/2), ones(1, N-1)];
  * X_DCT_N = X_DCT_N_scale.*X_DCT_2N(1:N);
  * 
  * (2N)*(2N) FFT by N*N DCT for X
  * X_DCT_N = dct(x);
  * k = (0:1:N-1);
  * W = cos(2*pi*(0.5*k)/(2*N)) + i*sin(2*pi*(0.5*k)/(2*N));
  * W = [2, sqrt(2.*(ones(1, N-1)))]*2.*W;
  * X_EXT_DFT_N = W.*X_DCT_N;
  * X_EXT_DFT_2N = [X_EXT_DFT_N, 0, conj(X_EXT_DFT_N(end:-1:2))];
 */
bool PGM::filtering_by_DCT(double**dst, double* kernel, double* kernel_DCT, int w_kernel, int h_kernel, bool dct_by_raw_filtered, bool do_even_extension, bool go_fast) {
	if (raw == NULL)	
		return false;
	if (size <= 0)	
		return false;
	if (depth != 1 && depth!= 2) 
		return false;

	string file_name = "";
	ofstream fos;
	double* image = NULL;
	double* h = NULL;
	double* y = NULL;
	double* Y = NULL;
	double* I = NULL;
	double* H = NULL;
	double* spec_Y = NULL;
	double  w_spec_Y;
	double  h_spec_Y;
	double* spec_I = NULL;
	double  w_spec_I;
	double  h_spec_I;
	double* spec_H = NULL;
	double  w_spec_H;
	double  h_spec_H;
	int n = 0;
	int m = 0;

	LOG("Prepare im...\n");
	Filter f_im_maker;

	if (dct_by_raw_filtered) {
		LOG("Prepare im with filtered image...\n");
		if (do_even_extension) {
			LOG("Prepare im with even extension...\n");
			if (!f_im_maker.mirrorPadding(&image, this->raw_filtered, &n, &m, this->w_filtered, this->h_filtered)) {
				return false;
			}
		}
		else {
			LOG("Prepare im without even extension...\n");
			if (dct_by_raw_filtered) {
				n = this->w_filtered;
				m = this->h_filtered;
			}
			else {
				n = this->width;
				m = this->height;
			}
			image = (double*)malloc(sizeof(double)*(n)*(m));
			memset(image, 0, sizeof(double)*(n)*(m));
			if (!f_im_maker.copy(image, this->raw_filtered, n, m, this->w_filtered, this->h_filtered)) {
				return false;
			}
		}
	}
	else {
		LOG("Prepare im with raw image...\n");
		double* image_tmp = (double*)malloc(sizeof(double)*(this->width)*(this->height));
		if (image_tmp == NULL)
			return false;
		memset(image_tmp, 0, sizeof(double)*(this->width)*(this->height));

		//copy uint8 raw to be double im
		int k = 0;
		for (int j=0; j<(this->height); j++) {
			for (int i=0; i<(this->width); i++) {
				if (depth == 2) {
					if (image_tmp+k) {
						image_tmp[k] = ((uint16_t*)(this->raw))[(i+j*(this->width))];
					}
					else {
						return false;
					}
				}
				else if (depth == 1) {
					if (image_tmp+k) {
						image_tmp[k] = ((uint8_t*)(this->raw))[(i+j*(this->width))];
					}
					else {
						return false;
					}
				} 
				k++;			
			}
		}

		if (do_even_extension) {
			LOG("Prepare im with even extension...\n");
			double* image_tmp_crop = (double*)malloc(sizeof(double)*(w_kernel)*(h_kernel));
			if (image_tmp == NULL)
				return false;
			memset(image_tmp_crop, 0, sizeof(double)*(w_kernel)*(h_kernel));
			//crop double im
			if (!f_im_maker.copy_center_around(image_tmp_crop, image_tmp, w_kernel, h_kernel, this->width, this->height)) {
				return false;
			}	
			if (!f_im_maker.mirrorPadding(&image, image_tmp_crop, &n, &m, w_kernel, h_kernel)) {
				return false;
			}
			if (image_tmp_crop)
				free(image_tmp_crop);
		}
		else {
			LOG("Prepare im without even extension...\n");
			if (dct_by_raw_filtered) {
				n = this->w_filtered;
				m = this->h_filtered;
			}
			else {
				n = this->width;
				m = this->height;
			}
			image = (double*)malloc(sizeof(double)*(n)*(m));
			if (image == NULL)
				return false;
			memset(image, 0, sizeof(double)*(n)*(m));
			if (!f_im_maker.copy(image, image_tmp, n, m, this->width, this->height)) {
				return false;
			}
		}
		if (image_tmp)
			free(image_tmp);
	}

	LOG("Prepare im done\n");

	LOG("Prepare kernel...\n");
	if (kernel_DCT != NULL) {
		LOG("DCT of kernel assigned... (N:%d, M:%d)\n", n, m);
		H = kernel_DCT;
		DCT dct_kernel;
		if (!dct_kernel.spectrum(&spec_H, H, &w_spec_H, &h_spec_H, false)) {
			LOG("spectrum failed\n");
			return false;
		}
		LOG("DCT of kernel assigned done (w_spec_H:%d, h_spec_H:%d)\n", (int)w_spec_H, (int)h_spec_H);
	}
	else {		
		LOG("Zero-padding on h... (N:%d, M:%d) to be (N:%d, M:%d)\n", w_kernel, h_kernel, n, m);
		Filter f;
		h = (double*)malloc(sizeof(double)*n*m);
		if (h == NULL)
			return false;
		memset(h, 0, sizeof(double)*n*m);
		if (!f.copy(h, kernel, n, m, w_kernel, h_kernel)) {
			LOG("copy failed\n");
			return false;
		}

		LOG("DCT of kernel... (N:%d, M:%d)\n", n, m);
		DCT dct_kernel;
		if (go_fast) {
			if (n != m) {
				LOG("only square matrix can go with dctmtx !\n");
				return false;
			}
			H = (double*)malloc(n*n*sizeof(double));
			if (!dct_kernel.dct2(H, h, n)) {
				LOG("dct2 failed\n");
				return false;
			}
		}
		else {
			if (!dct_kernel.dct2(&H, h, n, m)) {
				LOG("dct2 failed\n");
				return false;
			}
		}

		if (!dct_kernel.spectrum(&spec_H, H, &w_spec_H, &h_spec_H, false)) {
			LOG("spectrum failed\n");
			return false;
		}
		LOG("DCT of kernel done (w_spec_H:%d, h_spec_H:%d)\n", (int)w_spec_H, (int)h_spec_H);
	}
	try {
		file_name = "";
		string file_name_path = file.substr(0, file.find_last_of("/\\"));
		string file_name_file = file.substr(file.find_last_of("/\\"));
		string file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "_filteringByDCT_kernel.raw";
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
		if (h)
			fos.write((char*)h, (double)n*(double)m*sizeof(double));
		fos.close();
		LOG("[%dx%d Kernel image file]:%s\n", n, m, file_name.c_str());
		
		file_name = "";
		file_name_path = file.substr(0, file.find_last_of("/\\"));
		file_name_file = file.substr(file.find_last_of("/\\"));
		file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "_filteringByDCT_kernel_DCT.raw";
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
		if (spec_H)
			fos.write((char*)spec_H, w_spec_H*h_spec_H*sizeof(double));
		fos.close();
		LOG("[%dx%d DCT of kernel image file]:%s\n", (int)w_spec_H, (int)h_spec_H, file_name.c_str());
	}
	catch (exception ex) {
		return false;
	}
	LOG("Prepare kernel done\n");

	LOG("DCT of image... (N:%d, M:%d)\n", n, m);

	LOG("Copy center around of image... (N:%d, M:%d) to be (N:%d, M:%d)\n", this->width, this->height, n, m);
	Filter f;
	double* im = (double*)malloc(sizeof(double)*n*m);
	if (im == NULL)
		return false;
	memset(im, 0, sizeof(double)*n*m);
#if 0 //copy center around to have more symmetric im
	if (!f.copy_center_around(im, image, n, m, (this->width), (this->height))) {
		LOG("copy_center_around failed\n");
		return false;
	}
#else
	if (!f.copy(im, image, n, m, n, m)) {
		LOG("copy failed\n");
		return false;
	}
#endif
	DCT dct_im;

	if (go_fast) {
		if (n != m) {
			LOG("only square matrix can go with dctmtx !\n");
			return false;
		}
		I = (double*)malloc(n*n*sizeof(double));
		if (!dct_im.dct2(I, im, n)) {
			LOG("dct2 failed\n");
			return false;
		}
	}
	else {
		if (!dct_im.dct2(&I, im, n, m)) {
			LOG("dct2 failed\n");
			return false;
		}
	}
	if (!dct_im.spectrum(&spec_I, &w_spec_I, &h_spec_I, false)) {
		LOG("spectrum failed\n");
		return false;
	}
	try {
		file_name = "";
		string file_name_path = file.substr(0, file.find_last_of("/\\"));
		string file_name_file = file.substr(file.find_last_of("/\\"));
		string file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "_filteringByDCT_image.raw";
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
		if (im)
			fos.write((char*)im, (double)n*(double)m*sizeof(double));
		fos.close();
		LOG("[%dx%d image file]:%s\n", n, m, file_name.c_str());
		
		file_name = "";
		file_name_path = file.substr(0, file.find_last_of("/\\"));
		file_name_file = file.substr(file.find_last_of("/\\"));
		file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "_filteringByDCT_image_DCT.raw";
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
		if (spec_H)
			fos.write((char*)spec_I, w_spec_I*h_spec_I*sizeof(double));
		fos.close();
		LOG("[%dx%d DCT of image file]:%s\n", (int)w_spec_I, (int)h_spec_I, file_name.c_str());
	}
	catch (exception ex) {
		return false;
	}

	LOG("DCT of image done (w_spec_I:%d, h_spec_I:%d)\n", (int)w_spec_I, (int)h_spec_I);

	LOG("Product by I(N:%d, M:%d) .* H(N:%d, M:%d)...\n", n, m, n, m);
	Y = (double*)malloc(sizeof(double)*n*m);
	if (Y == NULL)
		return false;
	memset(Y, 0,sizeof(double)*n*m);
	Filter f_prod;
	if (!f_prod.element_wise_multiply(Y, I, H, n, m)) {
		LOG("element_wise_multiply failed\n");
		return false;
	}

	DCT dct_Y(n, m, Y);
	if (!dct_Y.spectrum(&spec_Y, Y, &w_spec_Y, &h_spec_Y, false)) {
		LOG("spectrum failed\n");
		return false;
	}

	LOG("Product done\n");

	LOG("IDCT of product... (N:%d, M:%d)\n", n, m);
	y = (double*)malloc(sizeof(double)*n*m);
	if (y == NULL)
		return false;
	memset(y, 0, sizeof(double)*n*m);
	double w_idct = 0;
	double h_idct = 0;
	DCT dct_forY(n, m, Y);

	if (go_fast) {
		if (n != m) {
			LOG("only square matrix can go with dctmtx !\n");
			return false;
		}
		w_idct = n;
		h_idct = m;
		if (!dct_forY.idct2(y, Y, w_idct)) {
			LOG("idct2 failed\n");
			return false;
		}
	}
	else {
		if (!dct_forY.idct2(y, Y, &w_idct, &h_idct)) {
			LOG("idct2 failed\n");
			return false;
		}
	}
	
	LOG("IDCT of product done (N:%d, M:%d)\n", (int)w_idct, (int)h_idct);

#if 1
	try {
		file_name = "";
		string file_name_path = file.substr(0, file.find_last_of("/\\"));
		string file_name_file = file.substr(file.find_last_of("/\\"));
		string file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "_filteringByDCT_filtered.raw";
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
		if (y)
			fos.write((char*)y, (double)w_idct*(double)h_idct*sizeof(double));
		fos.close();
		LOG("[%dx%d Filtered image file]:%s\n", (int)w_idct, (int)h_idct, file_name.c_str());

		file_name = "";
		file_name_path = file.substr(0, file.find_last_of("/\\"));
		file_name_file = file.substr(file.find_last_of("/\\"));
		file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "_filteringByDCT_filtered_DCT.raw";
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
		if (spec_H)
			fos.write((char*)spec_Y, w_spec_Y*h_spec_Y*sizeof(double));
		fos.close();
		LOG("[%dx%d DCT of filtered image file]:%s\n", (int)w_spec_Y, (int)h_spec_Y, file_name.c_str());
	}
	catch (exception ex) {
		return false;
	}
#endif

	if (h)
		free(h);
	if (H)
		free(H);	
	if (spec)
		free(spec);	
	if (spec_I)
		free(spec_I);	
	if (spec_H)
		free(spec_H);	
	if (image)
		free(image);	
	if (im)
		free(im);
	if (I)
		free(I);	
	if (y)
		free(y);
	if (Y)
		free(Y);
	return true;
}

bool PGM::filtering(double* kernel, int window) {
	return filtering(kernel, window, 0);
}

bool PGM::filtering(double* kernel, int window, int need_centering_crop) {
	if (raw == NULL)	
		return false;
	if (size <= 0)	
		return false;
	if (depth != 1 && depth!= 2) 
		return false;

	Filter filter;
	double* output_double = NULL;
	uint8_t* output = NULL;
	double*	 src    = NULL;

#if 1
	//crop kernel to be a square
	int window_ori = window;
	int w_kern_crop = window;
	int h_kern_crop = window;
	if (window > width) {
		LOG("kernel window:%d is too large to width:%d\n", window_ori, width);
		w_kern_crop = width;
	}
	if (window > height) {
		LOG("kernel window:%d is too large to height:%d\n", window_ori, height);
		h_kern_crop = height;
	}
	window = (h_kern_crop < w_kern_crop)?h_kern_crop:w_kern_crop;
	if (window%2 == 0) {
		window -= 1;
	}
	LOG("kernel window fixed:%d\n", window);
	double* kernel_croped = (double*)malloc(sizeof(double)*window*window);
	if (!kernel_croped) return false;
	memset(kernel_croped, 0, sizeof(double)*window*window);
	int k = 0;
	int w_aliasing = window-((w_kern_crop/2+window/2)-(w_kern_crop/2-window/2));
	int h_aliasing = window-((h_kern_crop/2+window/2)-(h_kern_crop/2-window/2));
	if (w_aliasing || h_aliasing)
		LOG("kernel width aliasing:%d, kernel height aliasing:%d\n", w_aliasing, h_aliasing);
	for (int j=(h_kern_crop/2-window/2); j<(h_kern_crop/2+window/2)+h_aliasing; j++) {
		for (int i=(w_kern_crop/2-window/2); i<(w_kern_crop/2+window/2)+w_aliasing; i++) {	
			kernel_croped[k] = kernel[(i+j*window_ori)];
			if (kernel_croped[k] > 0)
				LOG("[%d]x[%d] kernel_croped[%d][%d]:%f\n", window, window, k%(window), k/(window), kernel_croped[k]);
			k++;
		}
	}
	LOG("Change to use kernel cropped:%p from %p\n", kernel_croped, kernel);
	kernel = kernel_croped;
#endif

	if (need_centering_crop == NO_Centering_NO_Crop) {
		w_filtered = this->width;
		h_filtered = this->height;
		if (w_filtered%2 == 0)
			w_filtered -= 1;
		if (h_filtered%2 == 0)
			h_filtered -= 1;
		src = (double*)malloc(sizeof(double)*w_filtered*h_filtered);
		if (src == NULL) return false;
		memset(src, 0, sizeof(double)*w_filtered*h_filtered);
		for (int j=0; j<h_filtered; j++) {
			for (int i=0; i<w_filtered; i++) {
				if (depth == 2) {
					src[i+j*w_filtered] = ((uint16_t*)raw)[(i+j*width)];
				}
				else if (depth == 1) {
					src[i+j*w_filtered] = ((uint8_t*)raw)[(i+j*width)];
				} 
			}
		}
	}
	else if (need_centering_crop == ONLY_Crop){
		src = (double*)malloc(sizeof(double)*window*window);
		if (src == NULL) return false;
		memset(src, 0, sizeof(double)*window*window);
		for (int j=0; j<window; j++) {
			for (int i=0; i<window; i++) {	
				if (depth == 2) {
					src[k] = ((uint16_t*)raw)[(i+j*width)];
				}
				else if (depth == 1) {
					src[k] = ((uint8_t*)raw)[(i+j*width)];
				} 
				k++;
			}
		}
		w_filtered = window;
		h_filtered = window;
	}
	else if (need_centering_crop == BOTH_Centering_Crop) { 
		int k = 0;
		src = (double*)malloc(sizeof(double)*window*window);
		if (src == NULL) return false;
		memset(src, 0, sizeof(double)*window*window);
		int w_aliasing = window-((width/2+window/2)-(width/2-window/2));
		int h_aliasing = window-((height/2+window/2)-(height/2-window/2));
		if (w_aliasing || h_aliasing)
			LOG("width aliasing:%d, height aliasing:%d\n", w_aliasing, h_aliasing);

		for (int j=(height/2-window/2); j<(height/2+window/2)+h_aliasing; j++) {
			for (int i=(width/2-window/2); i<(width/2+window/2)+w_aliasing; i++) {	
				if (depth == 2) {
					src[k] = ((uint16_t*)raw)[(i+j*width)];
				}
				else if (depth == 1) {
					src[k] = ((uint8_t*)raw)[(i+j*width)];
				} 
				k++;
			}
		}
		w_filtered = window;
		h_filtered = window;
	}
	//convolve
	if (!(filter.convolution(this->raw_filtered, src, kernel, w_filtered, h_filtered, window))) {
		LOG("convolution failed\n");
		return false;
	}
	else {
		LOG("convolution succeeded for [%d]x[%d]\n", w_filtered, h_filtered);
	}

#if 1
	if (!(filter.histogram_equalization(this->raw_filtered, w_filtered*h_filtered))) {
		return false;
	}

	if (!(filter.normalize(output_double, this->raw_filtered, w_filtered, h_filtered, 0, 255))) {
		return false;
	}

	if (!(filter.cast(output, output_double, w_filtered, h_filtered))) {
		return false;
	}
	for (int j=0; j<h_filtered; j++) {
		for (int i=0; i<w_filtered; i++) {	
			this->raw_filtered[i+j*w_filtered] = output[i+j*w_filtered];
		}
	}
#else
	if (!(filter.cast(output, this->raw_filtered, w_filtered, h_filtered))) {
		return false;
	}
#endif

	ofstream fos;
	try {
		string file_name = "";
		string file_name_path = file.substr(0, file.find_last_of("/\\"));
		string file_name_file = file.substr(file.find_last_of("/\\"));
		string file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "_im_Filter.raw";
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
		fos.write((char*)src, (double)w_filtered*(double)h_filtered*sizeof(double));
		fos.close();
		LOG("[%dx%d Filter Raw Image file]:%s\n", w_filtered, h_filtered, file_name.c_str());

		file_name = "";
		file_name_path = file.substr(0, file.find_last_of("/\\"));
		file_name_file = file.substr(file.find_last_of("/\\"));
		file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "Filtered";
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
		fos.write((char*)output, (double)w_filtered*(double)h_filtered*sizeof(char));
		fos.close();
		LOG("[%dx%d Filtered file]:%s\n", w_filtered, h_filtered, file_name.c_str());


		file_name = "";
		file_name_path = file.substr(0, file.find_last_of("/\\"));
		file_name_file = file.substr(file.find_last_of("/\\"));
		file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "_kernel_Filter.raw";
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
		fos.write((char*)kernel, (double)window*(double)window*sizeof(double));
		fos.close();
		LOG("[%dx%d Kernel file]:%s\n", window, window, file_name.c_str());
	}
	catch (exception ex) {
		return false;
	}

	if (src)
		free(src);
	if (output)
		free(output);

	return true;
}

	/******************************************************************************
	**                      DCT
	*******************************************************************************/
bool PGM::dct(double** dst, int n_, int m_) {
	return dct(dst, n_, m_, n_, n_, false);
}

bool PGM::dct(double** dst, int n_, int m_, int window) {
	return dct(dst, n_, m_, window, window, false);
}

bool PGM::dct(double** dst, int n_, int m_, int window, int group, bool dct_by_raw_filtered) {
	if (raw == NULL) 
		return false;
	if (size <= 0)	
		return false;
	if (n_*m_ > size) 
		return false;
	if (depth != 1 && depth!= 2) 
		return false;
	if (dct_by_raw_filtered && raw_filtered == NULL)
		return false;
#if 0 //employ only part of filtered is acceptable
	if (dct_by_raw_filtered && (n_ != w_filtered || m_ != h_filtered))
		return false;	
#endif

	if (depth == 1) {
		im = malloc(sizeof(uint8_t)*n_*m_);
	}
	else if (depth == 2){
		im = malloc(sizeof(uint16_t)*n_*m_);
	}
	memset(im, 0, n_*m_*depth);

	int k = 0;
	for (int j=0; j<m_; j++) {
		for (int i=0; i<n_; i++) {
			if (depth == 2) {
				if ((uint16_t*)im+k) {
					if (!dct_by_raw_filtered)
						((uint16_t*)im)[k] = ((uint16_t*)raw)[(i+j*width)];
					else
						((uint16_t*)im)[k] = raw_filtered[(i+j*w_filtered)];
				}
				else {
					return false;
				}
			}
			else if (depth == 1) {
				if ((uint8_t*)im+k) {
					if (!dct_by_raw_filtered)
						((uint8_t*)im)[k] = ((uint8_t*)raw)[(i+j*width)];
					else
						((uint8_t*)im)[k] = raw_filtered[(i+j*w_filtered)];
				}
				else {
					return false;
				}
			} 
			k++;			
		}
	}

	double n = n_;
	double m = m_;
	double l = n*m;
	double* img = (double*)malloc(sizeof(double)*n*m);
	if (img == NULL) 
		return false;
	for (int i=0; i<l; i++) {
		if (depth == 2) {
			if (!dct_by_raw_filtered)
				img[i] = (double)((uint16_t*)im)[i];
			else
				img[i] = raw_filtered[i];
		}
		else if (depth == 1) {
			if (!dct_by_raw_filtered)
				img[i] = (double)((uint8_t*)im)[i];
			else
				img[i] = raw_filtered[i];
		}
	}

	LOG("DCT...\n");
	DCT dct;
	if (Y) {
		free(Y);
		Y = NULL;
	}
	Y = (double*)malloc(sizeof(double)*n_*m_);
	if (Y == NULL)
		return false;
	if (!dct.dct2(&Y, img, n, m, window, group)) {
		LOG("dct2 failed\n");
		return false;
	}
	
	width_Y = n_;
	height_Y = m_;

	//copy to callee
	if (dst) {
		memcpy(*dst, Y, sizeof(double)*n_*m_);
	}

	LOG("SPCTRUM...\n");
	double w;
	double h;
	dct.spectrum(&spec, &w, &h, false);
	//dct.fftshift(spec, w, h);

	ofstream fos;
	try {
		string file_name = "";
		string file_name_path = file.substr(0, file.find_last_of("/\\"));
		string file_name_file = file.substr(file.find_last_of("/\\"));
		string file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "_im_PGMDct.raw";
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
		fos.write((char*)im, n*m*sizeof(uint8_t)*depth);
		fos.close();
		LOG("[%dx%d Image file]:%s\n", (int)n, (int)m, file_name.c_str());

		file_name = "";
		file_name_path = file.substr(0, file.find_last_of("/\\"));
		file_name_file = file.substr(file.find_last_of("/\\"));
		file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "DCTSpec.raw";
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
			fos.write((char*)spec, l*sizeof(double));
		fos.close();
		LOG("[%dx%d DCT spec file]:%s\n", (int)n, (int)m, file_name.c_str());
	}
	catch (exception ex) {
		return false;
	}

	if (im)
		free(im);
	if (img)
		free(img);
	return true;
}

bool PGM::idct(double* src, int n_, int m_) {
	return idct(src, n_, m_, n_);
}

bool PGM::idct(double* src, int n_, int m_, int group) {
	return idct(src, n_, m_, n_, group);
}
bool PGM::idct(double* src, int n_, int m_, int window, int group) {
	if (raw == NULL)  return false;
	if (size <= 0)  return false;
	if (width_Y <= 0)  return false;
	if (height_Y <= 0)  return false;

	double w = width_Y;
	double h = height_Y;
	int l = m_*n_;
	
	LOG("IDCT...\n");
	double *output_idft = NULL;
	uint8_t* output = NULL;
	if (src == NULL) {
		if (Y == NULL)  return false;  
		DCT dct(w, h, Y);
		dct.idct2(output_idft, &w, &h, window, group);
		if (output_idft == NULL)
		return false;
	}
	else {
		DCT dct(w, h, src);
		dct.idct2(output_idft, &w, &h, window, group);
		if (output_idft == NULL)
		return false;
	}

	Filter filter;
#if 0
	if (!(filter.normalize(output, output_idft, n_, m_, true))) {
		return false;
	}
#else
	if (!(filter.cast(output, output_idft, n_, m_))) {
		return false;
	}
#endif

	ofstream fos;
	try {
		string file_name = "";
		string file_name_path = file.substr(0, file.find_last_of("/\\"));
		string file_name_file = file.substr(file.find_last_of("/\\"));
		string file_name_file_to_matlab = file_name_file.substr(0, file_name_file.find_last_of("/.")) + "IDCT.raw";
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
		fos.write((char*)output, l*sizeof(uint8_t));
		fos.close();
		LOG("[%dx%d IDFT file]:%s\n", n_, m_, file_name.c_str());		
	}
	catch (exception ex) {
		return false;
	}

	if (output)
		free(output);

	if (output_idft)
		free(output_idft);

	return true;
}
