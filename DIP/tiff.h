#ifndef _TIFF_
#define _TIFF_
#include <fstream>
#include <string>
#include <memory>
#include <cstdarg>
#include <iostream>
#include <filesystem>
#include "stdlib.h"
using namespace std;
//#define PRINT_DFT_IDFT

#define TIFF_TAG_NewSubFileType  254
#define TIFF_TAG_SubFileType     255
#define TIFF_TAG_ImageWidth      256
#define TIFF_TAG_ImageHeight     257
#define TIFF_TAG_BitsPerSample   258
#define TIFF_TAG_Compression     259
#define TIFF_TAG_PhotometricInterpretation 262
#define TIFF_TAG_DocumentName    269
#define TIFF_TAG_StripOffsets    273
#define TIFF_TAG_SamplePerPixel  277
#define TIFF_TAG_RowPerStrip     278
#define TIFF_TAG_StripByteCounts 279
#define TIFF_TAG_XResolution     282
#define TIFF_TAG_YResolution     283
#define TIFF_TAG_PlanarConfiguration       284
#define TIFF_TAG_ResolutionUnit  296
#define TIFF_TAG_Predictor       317

struct IFH_1 {
	uint16_t order;  //intel or motorola 
	uint16_t type;   //file type
	uint32_t offset; //IFD address
	uint16_t num;    //number of DE
} ;
	
struct IFH_2 {
	uint16_t order;  //intel or motorola 
	uint16_t type;   //file type
	uint32_t offset; //IFD address
} ;

struct DE {          //Directory Entry
	uint16_t tag;
	uint16_t type;
	uint32_t len;
	uint32_t value;
};

struct Image {
	uint32_t width;
	uint32_t height;
	uint32_t data;
	uint32_t size;
};

class Tiff {
protected:

private:
	//Image image;
	DE*   de;
	IFH_1 ifh_1;
	IFH_2 ifh_2;
	//uint8_t* raw;
	int size_raw;
public:
	Tiff(); 
	~Tiff();
	Image image;
	uint8_t* raw;
	void byte_swap(uint8_t* data, int w);
	bool read_type1(string file_name, uint8_t*& data, int* size);
	bool read_type2(string file_name, uint8_t*& data, int* size);
	bool bright(int level, uint8_t* dst, int len);
	bool inverse(uint8_t* dst, int len);
	bool bit_plane_slicing(uint8_t* dst);
	bool bit_plane_reconstruct(uint8_t* dst);
	bool halftone_dots(uint8_t* dst, int lip);
	bool FloydSteinbergDithering(uint8_t* dst);
	bool dft_idft(int m_, int n_);
};

#endif