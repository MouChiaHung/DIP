#ifndef _PGM_
#define _PGM_
#include <fstream>
#include <string>
#include <memory>
#include <cstdarg>
#include <iostream>
#include <filesystem>
#include "stdlib.h"
using namespace std;
//#define PRINT_DFT_IDFT


class PGM {
protected:

private:
	
public:
	PGM(); 
	~PGM();
	bool read(string file_name, uint8_t*& data, int* size);
	bool dft_idft(int m_, int n_);
	bool low_pass_eff(int m_, int n_, int window);
	bool filtering(double* kernel, int window);
	string file;
	int type;
	int width;
	int height;
	int depth;
	int size;
	uint8_t* raw;
};

#endif