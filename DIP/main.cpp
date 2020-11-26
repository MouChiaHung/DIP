	/******************************************************************************
	**                      INCLUDE
	*******************************************************************************/
#include <fstream>
#include <string>
#include <memory>
#include <cstdarg>
#include <iostream>
#include <filesystem>
#include "stdlib.h"
#include "conio.h"
#include "tiff.h"
#include "dft.h"
#include <time.h>  

using namespace std;
	/******************************************************************************
	**                      DEFINE
	*******************************************************************************/
#define Blue 1
#define Green 2
#define Cyan 3
#define Red 4
#define Purple 5
#define Yellow_Dark 6
#define White_Default 7
#define Gray 8
#define Bright_Blue 9
#define Brigth_Green 10
#define Bright_Cyan 11
#define Bright_Red 12
#define Pink 13
#define Yellow 14
#define Bright_White 15

	/******************************************************************************
	**                      GLOBAL VARIABLE
	*******************************************************************************/

	/******************************************************************************
	**                      DECLARATION
	*******************************************************************************/
bool print_bit_plane(uint8_t* src, int w, int h);	

	/******************************************************************************
	**                      DECLARATION OF FUNCTION
	*******************************************************************************/

	/******************************************************************************
	**                      IMPLEMENTATION FOR UTIL
	*******************************************************************************/
#define WHEREARG  __func__
#define LOG(...) debug_print_impl(WHEREARG, ##__VA_ARGS__)
#define LOGV(...) debug_print_verbose_impl(WHEREARG, ##__VA_ARGS__)
#define LOGD(...) debug_print_debug_impl(WHEREARG, ##__VA_ARGS__)
#define LOGE(...) debug_print_error_impl(WHEREARG, ##__VA_ARGS__)
#ifdef NO_LOG
#define printf(...) debug_print_impl(WHEREARG, ##__VA_ARGS__)
#endif
	void debug_print_impl(const char* func, _Printf_format_string_ char const* const _Format, ...) {
#ifdef NO_LOG
		return;
#endif 
		setbuf(stdout, NULL);
		va_list args;
		va_start(args, _Format);
		char db_buf[128];
		sprintf(db_buf, "[%s]", func);
		int db_len = 0;
		db_len = strlen(db_buf);
		vsprintf(db_buf + db_len, _Format, args);
		printf(db_buf);
		va_end(args);

#ifdef EXPORT_LOG
		FILE* file = fopen("log.txt", "a+");
		fputs(db_buf, file);
		fclose(file);
#endif
	}

	void debug_print_verbose_impl(const char* func, _Printf_format_string_ char const* const _Format, ...) {
#ifdef NO_LOG
		//return;
#endif 
		setbuf(stdout, NULL);
		va_list args;
		va_start(args, _Format);
		char db_buf[128] = "";
		sprintf(db_buf, "");
		int db_len = 0;
		db_len = strlen(db_buf);
		vsprintf(db_buf + db_len, _Format, args);
		printf(db_buf);
		va_end(args);

#ifdef EXPORT_LOG
		FILE* file = fopen("log.txt", "a+");
		fputs(db_buf, file);
		fclose(file);
#endif
	}

	void debug_print_debug_impl(const char* func, _Printf_format_string_ char const* const _Format, ...) {
		setbuf(stdout, NULL);
		va_list args;
		va_start(args, _Format);
		char db_buf[128];
		sprintf(db_buf, "[%s]", func);
		int db_len = 0;
		db_len = strlen(db_buf);
		vsprintf(db_buf + db_len, _Format, args);
		std::cout << (db_buf) << std::endl;
		va_end(args);

#ifdef EXPORT_LOG
		FILE* file = fopen("log.txt", "a+");
		fputs(db_buf, file);
		fclose(file);
#endif
	}

	void debug_print_error_impl(const char* func, _Printf_format_string_ char const* const _Format, ...) {
#ifdef NO_LOG
		return;
#endif 
		setbuf(stdout, NULL);
		va_list args;
		va_start(args, _Format);
		char db_buf[128];
		sprintf(db_buf, "[%s]", func);
		int db_len = 0;
		db_len = strlen(db_buf);
		vsprintf(db_buf + db_len, _Format, args);
		printf(db_buf);
		va_end(args);

#ifdef EXPORT_LOG
		FILE* file = fopen("log.txt", "a+");
		fputs(db_buf, file);
		fclose(file);
#endif
	}

ostream& Foo(ostream &stream) {
	stream << "-*-*-*-*- Hello Digital Image Processing -*-*-*-*-\n" << endl;
	return stream;
}

ostream& Qoo(ostream &stream) {
	stream << "\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-" << endl;
	return stream;
}

void byte_swap_uint8_data(uint16_t* data, int w) {
	if (w < 2) return;
	if (w%2 != 0) {
		cout << "byte swap window is odd" << endl;
	}

	for (int i=0; i<w/2; i++) {
		data[i] += data[w-1-i];
		data[w-1-i] = data[i] - data[w-1-i];
		data[i] = data[i] - data[w-1-i];
	}
}

bool print_bit_plane(uint8_t* src, int w, int h) {
	int len_plane = w*h;
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
	for (int p=0; p<8; p++) {
		if (p==0)     
			memcpy(dst_0, src+p*len_plane, len_plane);
		else if (p==1)
			memcpy(dst_1, src+p*len_plane, len_plane);
		else if (p==2)
			memcpy(dst_2, src+p*len_plane, len_plane);
		else if (p==3)
			memcpy(dst_3, src+p*len_plane, len_plane);
		else if (p==4)
			memcpy(dst_4, src+p*len_plane, len_plane);
		else if (p==5)
			memcpy(dst_5, src+p*len_plane, len_plane);
		else if (p==6)
			memcpy(dst_6, src+p*len_plane, len_plane);
		else if (p==7)
			memcpy(dst_7, src+p*len_plane, len_plane);
	}

#if 1
	ofstream fos_0;
	try {
	string file_name_raw = "C:\\src\\amo\\DIP\\Debug\\bprint0.raw";
	remove(file_name_raw.c_str());
	fos_0.open(file_name_raw, fstream::in | fstream::out | fstream::trunc | fstream::binary);
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
	string file_name_raw = "C:\\src\\amo\\DIP\\Debug\\bprint1.raw";
	remove(file_name_raw.c_str());
	fos_1.open(file_name_raw, fstream::in | fstream::out | fstream::trunc | fstream::binary);
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
	string file_name_raw = "C:\\src\\amo\\DIP\\Debug\\bprint2.raw";
	remove(file_name_raw.c_str());
	fos_2.open(file_name_raw, fstream::in | fstream::out | fstream::trunc | fstream::binary);
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
	string file_name_raw = "C:\\src\\amo\\DIP\\Debug\\bprint3.raw";
	remove(file_name_raw.c_str());
	fos_3.open(file_name_raw, fstream::in | fstream::out | fstream::trunc | fstream::binary);
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
	string file_name_raw = "C:\\src\\amo\\DIP\\Debug\\bprint4.raw";
	remove(file_name_raw.c_str());
	fos_4.open(file_name_raw, fstream::in | fstream::out | fstream::trunc | fstream::binary);
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
	string file_name_raw = "C:\\src\\amo\\DIP\\Debug\\bprint5.raw";
	remove(file_name_raw.c_str());
	fos_5.open(file_name_raw, fstream::in | fstream::out | fstream::trunc | fstream::binary);
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
	string file_name_raw = "C:\\src\\amo\\DIP\\Debug\\bprint6.raw";
	remove(file_name_raw.c_str());
	fos_6.open(file_name_raw, fstream::in | fstream::out | fstream::trunc | fstream::binary);
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
	string file_name_raw = "C:\\src\\amo\\DIP\\Debug\\bprint7.raw";
	remove(file_name_raw.c_str());
	fos_7.open(file_name_raw, fstream::in | fstream::out | fstream::trunc | fstream::binary);
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
}

int main()
{
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	printf("start time:%s", asctime(timeinfo) );
#if 0 //test
#if 0 //previous test
	complex<double> c1(2, 1);
	printf("c1:%f+i%f\n", real(c1), imag(c1));
	complex<double> c2(1, 4);
	printf("c2:%f+i%f\n", real(c2), imag(c2));
	//complex<double> c3(real(c1)+real(c2), imag(c1)+imag(c2));
	complex<double> c3 = c1*c2;
	printf("c3:%f+i%f\n", real(c3), imag(c3));
	uint8_t tmp[9] = {0x0};
	for (int i=9-1; i>=0 ; i--) *(tmp+i) = 0xff-i;
	uint16_t tmp2[9];
	for (int i=0; i<9; i++) tmp2[i] = tmp[i];
	byte_swap_uint8_data(tmp2, 9);
#endif
	//uint64_t d1 = 0x12345678;
	double d1 = -10.5;
	uint8_t* pc = (uint8_t*)&d1;
	printf("sizeof(double):%d\n", sizeof(double));
	printf("sizeof(d1):%d\n", sizeof(d1));
	printf("d1:%lf\n", d1);
	for (int i=0; i<sizeof(d1); i++)
		printf("[%x]", *(pc+i));
	printf("\n");

	return 0;
#endif

#if 0 //DFT test
	DFT dft;
	int width = 4;
	int height = 3;
	double mat[] = { 1, 1, 3, 2, 3, 4, 123, 154, 55, 2, 22, 233 };
	dft.dft2(mat, 4, 3);
	double* image = NULL;
	double* spec = NULL;
	double w;
	double h;
	dft.spectrum(spec, &w, &h, true);
	dft.fftshift(spec, (int)w, (int)h);
	dft.idft2(image, &w, &h);
	return 0;
#endif

	//ostream& (*pf)(ostream&) = Foo; 
	//cout << pf;

	uint8_t* data = NULL;
	int size = 0;
	Tiff t;
	//t.read_type2("C:\\src\\amo\\DIP\\Debug\\barbara.tif", data, &size);
	t.read_type2("C:\\src\\amo\\DIP\\Debug\\finger.tif", data, &size);
	
#if 1 //current topics	
	t.dft_idft(200, 200);
#endif

#if 0 //previous topics

	uint8_t* data_bright = new uint8_t[t.image.width*t.image.height];
	t.bright(0x55, data_bright, t.image.width*256);

	uint8_t* data_inverse = new uint8_t[t.image.width*t.image.height];
	t.inverse(data_inverse, 512*256);
#if 0 //double inverse test
	uint8_t* ptmp = t.raw;
	memcpy(t.raw, data_inverse, t.image.width*t.image.height);
	t.inverse(data_inverse, 512*256);
	t.raw = ptmp;
#endif


	uint8_t* planes = new uint8_t[t.image.width*t.image.height*8];
	if (planes!= NULL) memset(planes, 0, t.image.width*t.image.height*8);
	t.bit_plane_slicing(planes);
	print_bit_plane(planes, t.image.width, t.image.height);
	t.bit_plane_reconstruct(planes);

	uint8_t* ht_dots = new uint8_t[t.image.width*t.image.height];
	if (ht_dots != NULL) memset(ht_dots, 0, t.image.width*t.image.height);
	t.halftone_dots(ht_dots, 8);

	uint8_t* bufFloydSteinbergDithering = new uint8_t[t.image.width*t.image.height];
	if (bufFloydSteinbergDithering != NULL) memset(bufFloydSteinbergDithering, 0, t.image.width*t.image.height);
	t.FloydSteinbergDithering(bufFloydSteinbergDithering);

	if (data_bright) delete[] data_bright; 
	if (data_inverse) delete[] data_inverse;
	if (planes) delete[] planes;
	if (ht_dots) delete[] ht_dots;
	if (bufFloydSteinbergDithering) delete[] bufFloydSteinbergDithering;
#endif
	//pf = Qoo;
	//cout << pf;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	printf("end time:%s", asctime(timeinfo) );
	system("pause");
}
