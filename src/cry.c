#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// NOTE: srand(seed) rand() will return different values on different OSs

int cry_buff(const char* in, char* out, size_t bothSize, const int mode, const int shift) {
	for (size_t i = 0; i < bothSize; i++) {
		int c = in[i];
		out[i] = mode ? c + shift : c - shift;
	}
	return 1;
}

int cry_file(const char* filei, const char* fileo, const int mode, const int shift) {
    FILE* fp1 = fopen(filei, "rb");
    if (fp1 == NULL) { return 0; }
	FILE* fp2 = fopen(fileo,"wb");
	if (fp2 == NULL) { return 0; }
	
	int c;
    while ((c = fgetc(fp1)) != EOF) {
		int g = mode ? c + shift : c - shift;
		fputc(g, fp2);
    }
	
    fclose(fp1);
	fclose(fp2);
	
	return 1;
}

int cry_file_r(const char* filei, const char* fileo, const int mode, const int rnd_seed) {
	
	srand(rnd_seed);
	
    FILE* fp1 = fopen(filei, "rb");
    if (fp1 == NULL) { return 0; }
	FILE* fp2 = fopen(fileo,"wb");
	if (fp2 == NULL) { return 0; }
	
	int c;
    while ((c = fgetc(fp1)) != EOF) {
		int r = rand();
		int g = mode ? c + r : c - r;
		fputc(g, fp2);
    }

    fclose(fp1);
	fclose(fp2);
	
	return 1;
}

int cry_str(const char* in, char* out, const int mode, const int shift) {
	for (size_t i = 0; i < strlen(in); i++) {
		int c = in[i];
		out[i] = mode ? c + shift : c - shift;
	}
	return 1;
}

int cry_str_r(const char* in, char* out, const int mode, const int rnd_seed) {
	
	srand(rnd_seed);
	
	for (size_t i = 0; i < strlen(in); i++) {
		int c = in[i];
		int r = rand();
		out[i] = mode ? c + r : c - r;
	}
	return 1;
}