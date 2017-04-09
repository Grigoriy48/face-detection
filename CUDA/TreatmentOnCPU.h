#include "RecognitionOnGPU.cuh"
#include "types.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "FreeImage.h"
#include <stdio.h>  
#include <stdlib.h>   
#include <string.h> 
#include <math.h>   
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

class TreatmentOnCPU {
public:
	FIBITMAP *original_image;
	unsigned short width_original, height_original;
	unsigned char *char_picture_original;
	unsigned short bit_pixel_original;
	unsigned short byte_pixel_original;
	unsigned short bit_width_original;
	FIBITMAP *gray_image;
	unsigned char *char_picture_gray;
	unsigned short bit_pixel_gray;
	unsigned short byte_pixel_gray;
	unsigned short bit_width_gray;
	int *pixel_intensity;
	int *pixel_intensity_sqr;
	const char *filename_i_txt;
	FIBITMAP *skin_image;
	float factor_for_skin;

	int mas_pointer;
	int* mas;

	TXMLCascade cascade;
	TStage stages[30];
	TFeature features[7000];
	TRect rects[11000];
	TreatmentOnCPU();
	~TreatmentOnCPU();
	void LoadImage(const char *file_in);
	void LoadCascade(const char *haar_cascade);
	void Recognize(float step_factor);
	int IntensityWindow(int x, int y, int w_window, int h_window);
	int SqrIntensityWindow(int x, int y, int w_window, int h_window);
	void Treatment();
	void allocation();
	void SearchSkinByRGB();
	void SearchSkinByYIQ();
};
