	/******************************************************************************
	**                      INCLUDE
	*******************************************************************************/
#include "tiff.h"
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
Tiff::Tiff() {
	ifh_1 = IFH_1();
	ifh_2 = IFH_2();
	image = Image();
	cout << "[Tiff::Tiff()]:" << endl;
	de = NULL;
	raw = NULL;
	size_raw = 0;
}

Tiff::~Tiff() {
	cout << "[Tiff::~Tiff()]:" << endl;
	if (de) free(de);
	if (raw) free(raw);
}

bool Tiff::read_type1(string file_name, uint8_t*& data, int* size) {
	ifstream fis;
	ofstream fos;

	fis.open(file_name.c_str(), ios::in | ios::binary);
	if (fis.fail()) {
		printf("%s\n", strerror(errno));
		return false;
	}
#if 1
	string file_name_raw = "C:\\src\\amo\\DIP\\Debug\\tiff.raw";
	remove(file_name_raw.c_str());
	fos.open(file_name_raw, fstream::in | fstream::out | fstream::trunc | fstream::binary);
	if (fos.fail()) {
		printf("%s\n", strerror(errno));
		return false;
	}
	else fos.clear();
#endif

	//extract head
	fis.read((char*)&ifh_1.order, sizeof(uint16_t));
	fis.read((char*)&ifh_1.type, sizeof(uint16_t));
	fis.read((char*)&ifh_1.offset, sizeof(uint32_t));

	fis.seekg(ifh_1.offset, ios::beg);
	fis.read((char*)&ifh_1.num, sizeof(uint16_t));	
	de = new DE[ifh_1.num];

	//extract de
	for (int i=0; i<ifh_1.num; i++) {
		fis.read((char*)&de[i].tag, sizeof(uint16_t));
		fis.read((char*)&de[i].type, sizeof(uint16_t));
		fis.read((char*)&de[i].len, sizeof(uint32_t));
		fis.read((char*)&de[i].value, sizeof(uint32_t));
	
		if (de[i].tag == TIFF_TAG_ImageWidth) image.width = de[i].value;
		else if (de[i].tag == TIFF_TAG_ImageHeight) image.height = de[i].value;
		else if (de[i].tag == TIFF_TAG_StripOffsets) image.data = de[i].value;
		else if (de[i].tag == TIFF_TAG_StripByteCounts) image.size = de[i].value;
	}

#if 1
	for (int i=0; i<ifh_1.num; i++) {
		switch (de[i].tag) {
			case TIFF_TAG_NewSubFileType: cout <<"NewSubFileType:" << de[i].value << endl; break;
			case TIFF_TAG_SubFileType: cout <<"SubFileType:" << de[i].value << endl; break;
			case TIFF_TAG_ImageWidth: cout <<"ImageWidth:" << de[i].value << endl; break;
			case TIFF_TAG_ImageHeight: cout <<"ImageHeight:" << de[i].value << endl; break;
			case TIFF_TAG_BitsPerSample: cout <<"BitsPerSample:" << de[i].value << endl; break;
			case TIFF_TAG_Compression: cout <<"Compression:" << de[i].value << endl; break;
			case TIFF_TAG_PhotometricInterpretation: cout <<"PhotometricInterpretation:" << de[i].value << endl; break;
			case TIFF_TAG_DocumentName: cout <<"DocumentName:" << de[i].value << endl; break;
			case TIFF_TAG_StripOffsets: cout <<"StripOffsets:" << de[i].value << endl; break;
			case TIFF_TAG_SamplePerPixel: cout <<"SamplePerPixel:" << de[i].value << endl; break;
			case TIFF_TAG_RowPerStrip: cout <<"RowPerStrip:" << de[i].value << endl; break;
			case TIFF_TAG_StripByteCounts: cout <<"StripByteCounts:" << de[i].value << endl; break;
			case TIFF_TAG_XResolution: cout <<"XResolution:" << de[i].value << endl; break;
			case TIFF_TAG_YResolution: cout <<"YResolution:" << de[i].value << endl; break;
			case TIFF_TAG_PlanarConfiguration: cout <<"PlanarConfiguration:" << de[i].value << endl; break;
			case TIFF_TAG_ResolutionUnit: cout <<"ResolutionUnit:" << de[i].value << endl; break;
			case TIFF_TAG_Predictor: cout <<"Predictor:" << de[i].value << endl; break;
			default: cout <<"Other Tag:" << de[i].value << endl; break;		
		}
	}
#endif

	//extract raw data
	fis.seekg(image.data, ios::beg);

	*size = (image.width)*(image.height);
	size_raw = (image.width)*(image.height);
	data = (uint8_t*) malloc(*size);
	memset(data, 0, *size);
	uint8_t* pc = data;

	for (int i=0; i<image.height; i++) {
		for (int j=0; j<image.width; j++) {
			pc = data + i*image.height + j;
			fis.read((char*)pc, 1);			
#if 1
			fos.write((char*)pc, 1);
			if (j == image.width) fos << endl;
			if (i == image.height) fos << endl;
#endif		
		}
	}
	fis.close();
	fos.close();
	if (raw) free(raw);
	raw = (uint8_t*) malloc(*size);
	memset(raw, 0, *size);
	memcpy(raw, data, *size);
	return true;
}

bool Tiff::read_type2(string file_name, uint8_t*& data, int* size) {
	ifstream fis;
	ofstream fos;
	uint16_t num_de = 0;

	fis.open(file_name.c_str(), ios::in | ios::binary);
	if (fis.fail()) {
		printf("%s\n", strerror(errno));
		return false;
	}
#if 1
	string file_name_raw = "C:\\src\\amo\\DIP\\Debug\\tif.raw";
	remove(file_name_raw.c_str());
	fos.open(file_name_raw, fstream::in | fstream::out | fstream::trunc | fstream::binary);
	if (fos.fail()) {
		printf("%s\n", strerror(errno));
		return false;
	}
	else fos.clear();
#endif

	//extract head
	fis.read((char*)&ifh_2.order, sizeof(uint16_t));
	fis.read((char*)&ifh_2.type, sizeof(uint16_t));
	fis.read((char*)&ifh_2.offset, sizeof(uint32_t));

	cout << "IFH offset:" << ifh_2.offset << endl;

	fis.seekg(ifh_2.offset, ios::beg);
	fis.read((char*)&num_de, sizeof(uint16_t));

	de = new DE[num_de];

	//extract de
	for (int i=0; i<num_de; i++) {
		fis.read((char*)&de[i].tag, sizeof(uint16_t));
		fis.read((char*)&de[i].type, sizeof(uint16_t));
		fis.read((char*)&de[i].len, sizeof(uint32_t));
		fis.read((char*)&de[i].value, sizeof(uint32_t));
		if (de[i].tag == TIFF_TAG_ImageWidth) image.width = de[i].value;
		if (de[i].tag == TIFF_TAG_ImageWidth) image.width = de[i].value;
		else if (de[i].tag == TIFF_TAG_ImageHeight) image.height = de[i].value;
		else if (de[i].tag == TIFF_TAG_StripOffsets) image.size = de[i].value;
	}
	image.data = sizeof(IFH_2);

#if 1
	for (int i=0; i<num_de; i++) {
		switch (de[i].tag) {
			case TIFF_TAG_NewSubFileType: cout <<"NewSubFileType:" << de[i].value << endl; break;
			case TIFF_TAG_SubFileType: cout <<"SubFileType:" << de[i].value << endl; break;
			case TIFF_TAG_ImageWidth: cout <<"ImageWidth:" << de[i].value << endl; break;
			case TIFF_TAG_ImageHeight: cout <<"ImageHeight:" << de[i].value << endl; break;
			case TIFF_TAG_BitsPerSample: cout <<"BitsPerSample:" << de[i].value << endl; break;
			case TIFF_TAG_Compression: cout <<"Compression:" << de[i].value << endl; break;
			case TIFF_TAG_PhotometricInterpretation: cout <<"PhotometricInterpretation:" << de[i].value << endl; break;
			case TIFF_TAG_DocumentName: cout <<"DocumentName:" << de[i].value << endl; break;
			case TIFF_TAG_StripOffsets: cout <<"StripOffsets:" << de[i].value << endl; break;
			case TIFF_TAG_SamplePerPixel: cout <<"SamplePerPixel:" << de[i].value << endl; break;
			case TIFF_TAG_RowPerStrip: cout <<"RowPerStrip:" << de[i].value << endl; break;
			case TIFF_TAG_StripByteCounts: cout <<"StripByteCounts:" << de[i].value << endl; break;
			case TIFF_TAG_XResolution: cout <<"XResolution:" << de[i].value << endl; break;
			case TIFF_TAG_YResolution: cout <<"YResolution:" << de[i].value << endl; break;
			case TIFF_TAG_PlanarConfiguration: cout <<"PlanarConfiguration:" << de[i].value << endl; break;
			case TIFF_TAG_ResolutionUnit: cout <<"ResolutionUnit:" << de[i].value << endl; break;
			case TIFF_TAG_Predictor: cout <<"Predictor:" << de[i].value << endl; break;
			default: cout <<"Other Tag:" << de[i].value << endl; break;		
		}
	}
#endif

	//extract raw data
	*size = (image.width)*(image.height);
	size_raw = (image.width)*(image.height);
	data = (uint8_t*) malloc(*size);
	memset(data, 0, *size);
	uint8_t* pc = data;
	int pos = 0;
#if 0  //backward read
	for (int i=image.height-1; i>=0; i--) {
		pos = (i*image.width)+sizeof(IFH_2);
		fis.seekg(pos, ios::beg);
		pc = data+((image.height-1)-i)*image.width;
		cout << "fis.tellg():" << fis.tellg() << endl;
		fis.read((char*)pc, image.width);
		byte_swap(pc, image.width);
#if 1
		fos.write((char*)pc, image.width);
		cout << "fos.tellp():" << fos.tellp() << endl;
#endif
	}
#else //forward read
	for (int i=0; i<image.height; i++) {
		pos = (i*image.width)+sizeof(IFH_2);
		fis.seekg(pos, ios::beg);
		pc = data+i*image.width;
		fis.read((char*)pc, image.width);
		//cout << "fis.tellg():" << fis.tellg() << endl;
#if 1
		fos.write((char*)pc, image.width);
		//cout << "fos.tellp():" << fos.tellp() << endl;
#endif
	}
#endif
	fis.close();
	fos.flush();
	fos.close();
	if (raw) free(raw);
	raw = NULL;
	raw = (uint8_t*) malloc(*size);
	if (raw != NULL) memset(raw, 0, *size);
	if (data != NULL && raw != NULL) memcpy(raw, data, *size);
	return true;
}

void Tiff::byte_swap(uint8_t* data, int w) {
	if (w < 2) return;
	char c;
	for (int i=0; i<w/2; i++) {
		c = data[i];
		data[i] = data[w-1-i];
		data[w-1-i] = c;
	}
}

bool Tiff::bright(int level, uint8_t* dst, int len) {
	if (de == NULL) return false;
	if (raw == NULL) return false;
	if (size_raw <= 0) return false;
	if (dst == NULL) return false;
	if (len > size_raw) return false;
	memset(dst, 0, len);

	for (int i=0; i<len; i++) {
		if (raw[i] >= 0xff) continue;
		else {
			if ((raw[i]+level)>=0xff) dst[i] = 0xff;
			else dst[i] = raw[i] + level;
		}
	}
	int l = size_raw - len;
	if (len<size_raw && l>0) memcpy(dst+len, raw+len, l);

	ofstream fos;
	try {
	string f = "C:\\src\\amo\\DIP\\Debug\\bright.raw";
	remove(f.c_str());
	fos.open(f, fstream::in | fstream::out | fstream::trunc | fstream::binary);
	if (fos.fail()) {
		printf("%s\n", strerror(errno));
		return false;
	}
	else fos.clear();
	fos.write((char*)dst, size_raw);
	fos.close();
	return true;
	}
	catch (exception ex) {
		return false;
	}
}

bool Tiff::inverse(uint8_t* dst, int len) {
	if (de == NULL) return false;
	if (raw == NULL) return false;
	if (size_raw <= 0) return false;
	if (dst == NULL) return false;
	if (len > size_raw) return false;
	memset(dst, 0, len);

	for (int i=0; i<len; i++) {
		dst[i] = 0xff - raw[i];
	}
	int l = size_raw - len;
	if (len<size_raw && l>0) memcpy(dst+len, raw+len, l);

	ofstream fos;
	try {
	string f = "C:\\src\\amo\\DIP\\Debug\\inverse.raw";
	remove(f.c_str());
	fos.open(f, fstream::in | fstream::out | fstream::trunc | fstream::binary);
	if (fos.fail()) {
		printf("%s\n", strerror(errno));
		return false;
	}
	else fos.clear();
	fos.write((char*)dst, size_raw);
	fos.close();
	return true;
	}
	catch (exception ex) {
		return false;
	}
}

bool Tiff::bit_plane_slicing(uint8_t* dst) {
	if (de == NULL) return false;
	if (raw == NULL) return false;
	if (size_raw <= 0) return false;
	int len_plane = image.width*image.height;
#if 1
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
#endif
	uint8_t v = 0x0;
	int k = 0;
	for (int i=0; i<image.width; i++) {
		for (int j=0; j<image.height; j++) {
			k = i*image.width+j;
			v = raw[k];
			//if (v & (0x1<<0)) { 
			if (v/(0x1<<0)%2) { 
				dst_0[k] = 0xff;
			}
			//if (v & (0x1<<1)) {
			if (v/(0x1<<1)%2) {
				dst_1[k] = 0xff;
			}
			//if (v & (0x1<<2)) {
			if (v/(0x1<<2)%2) {
				dst_2[k] = 0xff;
			}
			//if (v & (0x1<<3)) {
			if (v/(0x1<<3)%2) {
				dst_3[k] = 0xff;
			}
			//if (v & (0x1<<4)) {
			if (v/(0x1<<4)%2) {
				dst_4[k] = 0xff;
			}
			//if (v & (0x1<<5)) {
			if (v/(0x1<<5)%2) {
				dst_5[k] = 0xff;
			}
			//if (v & (0x1<<6)) {
			if (v/(0x1<<6)%2) {
				dst_6[k] = 0xff;
			}
			//if (v & (0x1<<7)) {
			if (v/(0x1<<7)%2) {
				dst_7[k] = 0xff;
			}
		}
	}

	for (int p=0; p<8; p++) {
		if (p==0)     
			memcpy(dst+p*image.width*image.height, dst_0, len_plane);
		else if (p==1)
			memcpy(dst+p*image.width*image.height, dst_1, len_plane);
		else if (p==2)
			memcpy(dst+p*image.width*image.height, dst_2, len_plane);
		else if (p==3)
			memcpy(dst+p*image.width*image.height, dst_3, len_plane);
		else if (p==4)
			memcpy(dst+p*image.width*image.height, dst_4, len_plane);
		else if (p==5)
			memcpy(dst+p*image.width*image.height, dst_5, len_plane);
		else if (p==6)
			memcpy(dst+p*image.width*image.height, dst_6, len_plane);
		else if (p==7)
			memcpy(dst+p*image.width*image.height, dst_7, len_plane);
	}

#if 1
	ofstream fos_0;
	try {
	string f0 = "C:\\src\\amo\\DIP\\Debug\\bp0.raw";
	remove(f0.c_str());
	fos_0.open(f0, fstream::in | fstream::out | fstream::trunc | fstream::binary);
	if (fos_0.fail()) {
		printf("%s\n", strerror(errno));
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
	string f1 = "C:\\src\\amo\\DIP\\Debug\\bp1.raw";
	remove(f1.c_str());
	fos_1.open(f1, fstream::in | fstream::out | fstream::trunc | fstream::binary);
	if (fos_1.fail()) {
		printf("%s\n", strerror(errno));
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
	string f2 = "C:\\src\\amo\\DIP\\Debug\\bp2.raw";
	remove(f2.c_str());
	fos_2.open(f2, fstream::in | fstream::out | fstream::trunc | fstream::binary);
	if (fos_2.fail()) {
		printf("%s\n", strerror(errno));
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
	string f3 = "C:\\src\\amo\\DIP\\Debug\\bp3.raw";
	remove(f3.c_str());
	fos_3.open(f3, fstream::in | fstream::out | fstream::trunc | fstream::binary);
	if (fos_3.fail()) {
		printf("%s\n", strerror(errno));
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
	string f4 = "C:\\src\\amo\\DIP\\Debug\\bp4.raw";
	remove(f4.c_str());
	fos_4.open(f4, fstream::in | fstream::out | fstream::trunc | fstream::binary);
	if (fos_4.fail()) {
		printf("%s\n", strerror(errno));
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
	string f5 = "C:\\src\\amo\\DIP\\Debug\\bp5.raw";
	remove(f5.c_str());
	fos_5.open(f5, fstream::in | fstream::out | fstream::trunc | fstream::binary);
	if (fos_5.fail()) {
		printf("%s\n", strerror(errno));
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
	string f6 = "C:\\src\\amo\\DIP\\Debug\\bp6.raw";
	remove(f6.c_str());
	fos_6.open(f6, fstream::in | fstream::out | fstream::trunc | fstream::binary);
	if (fos_6.fail()) {
		printf("%s\n", strerror(errno));
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
	string f7 = "C:\\src\\amo\\DIP\\Debug\\bp7.raw";
	remove(f7.c_str());
	fos_7.open(f7, fstream::in | fstream::out | fstream::trunc | fstream::binary);
	if (fos_7.fail()) {
		printf("%s\n", strerror(errno));
		return false;
	}
	else fos_7.clear();
	fos_7.write((char*)dst_7, len_plane);
	fos_7.close();
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
	return true;
}

bool Tiff::bit_plane_reconstruct(uint8_t* src) {
	if (de == NULL) return false;
	if (size_raw <= 0) return false;	
	if (raw != NULL) {
		free(raw);
		raw = NULL;
	}
	raw = (uint8_t*) malloc(size_raw);
	if (raw == NULL) return false;
	memset(raw, 0, size_raw);

	uint8_t v = 0x0;
	int pos = 0;
	for (int i=0; i<image.height; i++) {
		for (int j=0; j<image.width; j++) {
			v = 0x0;
			v |= *(src+(i*image.width+j)+0*(image.width*image.height))&(0x1<<0);
			v |= *(src+(i*image.width+j)+1*(image.width*image.height))&(0x1<<1);
			v |= *(src+(i*image.width+j)+2*(image.width*image.height))&(0x1<<2);
			v |= *(src+(i*image.width+j)+3*(image.width*image.height))&(0x1<<3);
			v |= *(src+(i*image.width+j)+4*(image.width*image.height))&(0x1<<4);
			v |= *(src+(i*image.width+j)+5*(image.width*image.height))&(0x1<<5);
			v |= *(src+(i*image.width+j)+6*(image.width*image.height))&(0x1<<6);
			v |= *(src+(i*image.width+j)+7*(image.width*image.height))&(0x1<<7);
			raw[i*image.width+j] = v;
		}
	}

	ofstream fos;
	try {
	string file_name = "C:\\src\\amo\\DIP\\Debug\\reconstruct.raw";
	remove(file_name.c_str());
	fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
	if (fos.fail()) {
		printf("%s\n", strerror(errno));
		return false;
	}
	else fos.clear();
	fos.write((char*)raw, size_raw);
	fos.close();
	}
	catch (exception ex) {
		return false;
	}
}

bool Tiff::halftone_dots(uint8_t* dst, int lip) {
	if (de == NULL) return false;
	if (raw == NULL) return false;
	if (size_raw <= 0) return false;
	if (dst == NULL) return false;
	if (dst != NULL) memset(dst, 0, size_raw);
	uint16_t* im = new uint16_t[size_raw];
	memset(im, 0, size_raw);
	uint8_t* hblock = new uint8_t[lip*lip];
	if (hblock == NULL) return false; 
	memset(hblock, 0, lip*lip);
	if (lip == 8) {
		uint8_t dots[64] = {40 ,60 ,150,90 ,10 ,90 ,60 ,40 ,
							80 ,170,240,200,110,200,170,180,
							140,210,250,220,130,220,210,140,
							120,190,230,180,70 ,180,230,120,
							20 ,100,160,50 ,30 ,160,100,20 ,
							120,190,230,180,70 ,230,190,120,
							140,210,250,220,130,220,210,140,
							40 ,60 ,150,90 ,10 ,150,60 ,40};
		memcpy(hblock, dots, 8*8);
	}
	else {
		for (int i=0; i<lip*lip; i++) {
			uint8_t v = 0xff&((i<<2)%0x100);
			hblock[i] = v;
		}
	}

	int trival = 0;
	uint16_t sv = 0;
	for (int i=0; i<image.height; i++) {
		trival++;
		for (int j=0; j<image.width; j++) {
			if ((j+i*image.width) >= size_raw) break;
			sv = raw[j+i*image.width] + hblock[(j%lip)+(i%lip)*lip];
			im[j+i*image.width] = sv;
		}
	}
	
	for (int i=0; i<image.height; i++) {
		for (int j=0; j<image.width; j++) {
			if (j+i*image.width >= size_raw) break;
			sv = im[j+i*image.width];
			if (sv >= 0xff) {
				dst[j+i*image.width] = 0xff;
			}
			else {
				dst[j+i*image.width] = 0x0;
			} 
		}
	}

	ofstream fos;
	try {
	string file_name = "C:\\src\\amo\\DIP\\Debug\\halftone_dots.raw";
	remove(file_name.c_str());
	fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
	if (fos.fail()) {
		printf("%s\n", strerror(errno));
		return false;
	}
	else fos.clear();
	fos.write((char*)dst, size_raw);
	fos.close();
	}
	catch (exception ex) {
		return false;
	}

	//if (hblock) free(hblock);
	if (im != NULL) delete[] im;
	if (hblock != NULL) delete[] hblock;
	return true;
}

bool Tiff::FloydSteinbergDithering(uint8_t* dst) {
	if (de == NULL)		return false;
	if (raw == NULL)	return false;
	if (size_raw <= 0)	return false;
	if (dst == NULL)	return false;
	if (dst != NULL)	memset(dst, 0, size_raw);
	uint8_t* im = new uint8_t[size_raw];
	memcpy(im, raw, size_raw);
	uint8_t matrix[2][3] = {{0,0,7},{3,5,1}};

	int p = 0;
	uint8_t v = 0;
	uint8_t error = 0;
	bool isErrSplitUp = false; //false for split around darker otherwise lighter
	for (int i=0; i<image.height; i++) {
		for (int j=0; j<image.width; j++) {
#if 1
			p = j+i*image.width;
			if ((p)<size_raw) v = im[p]; 
			if (v >= 128) { //set this pixel up and split its intensity around down
				error = 0xff - v;
				isErrSplitUp = false;
				if (p<size_raw) im[p] = 0xff;
			}
			else { //set this pixel down and split its intensity around up
				error = v - 0;
				isErrSplitUp = true;
				if (p<size_raw) im[p] = 0x0;
			}
#if 1
			if (i>=image.height-1)
				continue;
			if (j<=0 || j>=image.width-1)
				continue;
		
			if (isErrSplitUp) {
				if ((p+1)<size_raw) 
					im[p+1] = ((im[p+1]+matrix[0][2]*error/16)>0xff)?0xff:(im[p+1]+matrix[0][2]*error/16);
				if ((p+image.width-1)<size_raw) 
					im[p+image.width-1] = ((im[p+image.width-1]+matrix[1][0]*error/16))>0xff?0xff:(im[p+image.width-1]+matrix[1][0]*error/16);
				if ((p+image.width)<size_raw)   
					im[p+image.width] = ((im[p+image.width]+matrix[1][1]*error/16)>0xff)?0xff:(im[p+image.width]+matrix[1][1]*error/16);
				if ((p+image.width+1)<size_raw) 
					im[p+image.width+1] = ((im[p+image.width+1]+matrix[1][2]*error/16)>0xff)?0xff:(im[p+image.width+1]+matrix[1][2]*error/16);
			}
			else {
				if ((p+1)<size_raw) 
					im[p+1] = ((im[p+1]-matrix[0][2]*error/16)<0)?0:(im[p+1]-matrix[0][2]*error/16);
				if ((p+image.width-1)<size_raw) 
					im[p+image.width-1] = ((im[p+image.width-1]-matrix[1][0]*error/16))<0?0:(im[p+image.width-1]-matrix[1][0]*error/16);
				if ((p+image.width)<size_raw)   
					im[p+image.width] = ((im[p+image.width]-matrix[1][1]*error/16)<0)?0:(im[p+image.width]-matrix[1][1]*error/16);
				if ((p+image.width+1)<size_raw) 
					im[p+image.width+1] = ((im[p+image.width+1]-matrix[1][2]*error/16)<0)?0:(im[p+image.width+1]-matrix[1][2]*error/16);
			}
#endif
#endif
		}
	}
	memcpy(dst, im, size_raw);

	ofstream fos;
	try {
	string file_name = "C:\\src\\amo\\DIP\\Debug\\FloydSteinbergDithering.raw";
	remove(file_name.c_str());
	fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
	if (fos.fail()) {
		printf("%s\n", strerror(errno));
		return false;
	}
	else fos.clear();
	fos.write((char*)dst, size_raw);
	fos.close();
	}
	catch (exception ex) {
		return false;
	}

	if (im != NULL) delete[] im;
	return true;
}

bool Tiff::dft_idft(int m_, int n_) {
	if (de == NULL)		return false;
	if (raw == NULL)	return false;
	if (size_raw <= 0)	return false;
	if (m_*n_ > size_raw) return false;

	uint8_t* im = new uint8_t[m_*n_];
	if (im == NULL)
		return false;
	memset(im, 0, m_*n_);
	int k = 0;
#if 1
	for (int j=(image.height/2-m_/2); j<(image.height/2+m_/2); j++) {
		for (int i=(image.width/2-n_/2); i<(image.width/2+n_/2); i++) {
			if (im+k) {
				*(im+k) = raw[(i+j*image.width)];
				k++;
			}
			else 
				return false;
		}
	}
#else
	for (int j=0; j<m_; j++) {
		for (int i=0; i<n_; i++) {
			*(im+k) = raw[(i+j*image.width)];
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
	for (int v=0; v<(int)n; v++) {
		for (int u=0; u<(int)m; u++) {
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
		string file_name = "C:\\src\\amo\\DIP\\Debug\\ift.raw";
		remove(file_name.c_str());
		fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			printf("%s\n", strerror(errno));
			return false;
		}
		else fos.clear();
		fos.write((char*)output, l*sizeof(uint8_t));
		fos.close();
	
		file_name = "C:\\src\\amo\\DIP\\Debug\\spec.raw";
		remove(file_name.c_str());
		fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			printf("%s\n", strerror(errno));
			return false;
		}
		else fos.clear();
		if (spec)
			fos.write((char*)spec, l*sizeof(double));
		fos.close();

		file_name = "C:\\src\\amo\\DIP\\Debug\\real.raw";
		remove(file_name.c_str());
		fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			printf("%s\n", strerror(errno));
			return false;
		}
		else fos.clear();
		if (real)
			fos.write((char*)real, l*sizeof(double));
		fos.close();
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

bool Tiff::low_pass(int m_, int n_, int window) {
	printf("LOW PASS...\n");
	if (de == NULL)		return false;
	if (raw == NULL)	return false;
	if (size_raw <= 0)	return false;
	if (m_*n_ > size_raw) return false;
	if (window > m_ || window > n_) return false;

	uint8_t* im = new uint8_t[m_*n_];
	if (im == NULL)
		return false;
	memset(im, 0, m_*n_);
	int k = 0;

	for (int j=(image.height/2-m_/2); j<(image.height/2+m_/2); j++) {
		for (int i=(image.width/2-n_/2); i<(image.width/2+n_/2); i++) {
			if (im+k) {
				*(im+k) = raw[(i+j*image.width)];
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
	dft.dft2(img, m, n);

	//low pass - clear if out of window
	printf("Filtering...\n");
	for (double v=0; v<m; v++) {
		for (double u=0; u<n; u++) {
			if ((u<window && v<window) || (u<window && v>(m-1)-window)) continue;
			else if ((u>(n-1)-window && v<window) || (u>(n-1)-window && v>(m-1)-window)) continue;
			dft.matrix[(int)u+(int)v*(int)m] = 0x0;
		}
	}

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
		string file_name = "C:\\src\\amo\\DIP\\Debug\\iftLP.raw";
		remove(file_name.c_str());
		fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			printf("%s\n", strerror(errno));
			return false;
		}
		else fos.clear();
		fos.write((char*)output, l*sizeof(uint8_t));
		fos.close();
	
		file_name = "C:\\src\\amo\\DIP\\Debug\\specLP.raw";
		remove(file_name.c_str());
		fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			printf("%s\n", strerror(errno));
			return false;
		}
		else fos.clear();
		if (spec)
			fos.write((char*)spec, l*sizeof(double));
		fos.close();
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

bool Tiff::low_pass_eff(int m_, int n_, int window) {
	printf("LOW PASS...\n");
	if (de == NULL)		return false;
	if (raw == NULL)	return false;
	if (size_raw <= 0)	return false;
	if (m_*n_ > size_raw) return false;
	if (window > m_ || window > n_) return false;

	uint8_t* im = new uint8_t[m_*n_];
	if (im == NULL)
		return false;
	memset(im, 0, m_*n_);
	int k = 0;

	for (int j=(image.height/2-m_/2); j<(image.height/2+m_/2); j++) {
		for (int i=(image.width/2-n_/2); i<(image.width/2+n_/2); i++) {
			if (im+k) {
				*(im+k) = raw[(i+j*image.width)];
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
		string file_name = "C:\\src\\amo\\DIP\\Debug\\iftLP.raw";
		remove(file_name.c_str());
		fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			printf("%s\n", strerror(errno));
			return false;
		}
		else fos.clear();
		fos.write((char*)output, l*sizeof(uint8_t));
		fos.close();
	
		file_name = "C:\\src\\amo\\DIP\\Debug\\specLP.raw";
		remove(file_name.c_str());
		fos.open(file_name, fstream::in | fstream::out | fstream::trunc | fstream::binary);
		if (fos.fail()) {
			printf("%s\n", strerror(errno));
			return false;
		}
		else fos.clear();
		if (spec)
			fos.write((char*)spec, l*sizeof(double));
		fos.close();
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


