#include <math.h>  
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "types.h"
#include "FreeImage.h"

void ParallelizationElements(TXMLCascade* cascade, TStage* stages, TFeature* features, TRect* rects,
	unsigned short width_original, unsigned short height_original, unsigned char* char_picture_original,
	unsigned short byte_pixel_original, unsigned short bit_width_original, float step_factor,
	int* pixel_intensity, int* pixel_intensity_sqr);
__global__ void ParallelizationElementsKernel(TXMLCascade* cascade, TStage* stages, TFeature* features, TRect* rects,
	unsigned short* width_original, unsigned short* height_original, unsigned char* char_picture_original,
	unsigned short* byte_pixel_original, unsigned short* bit_width_original, float* step_factor,
	int* pixel_intensity, int* pixel_intensity_sqr, float* factor);
void ParallelizationScale(TXMLCascade* cascade, TStage* stages, TFeature* features, TRect* rects,
	unsigned short width_original, unsigned short height_original, unsigned char* char_picture_original,
	unsigned short byte_pixel_original, unsigned short bit_width_original, float step_factor,
	int* pixel_intensity, int* pixel_intensity_sqr, int* mas_pointer, int** mas);
__global__ void ParallelizationScaleKernel(TXMLCascade* cascade, TStage* stages, TFeature* features, TRect* rects,
	unsigned short* width_original, unsigned short* height_original, unsigned char* char_picture_original,
	unsigned short* byte_pixel_original, unsigned short* bit_width_original, float* step_factor,
	int* pixel_intensity, int* pixel_intensity_sqr, float* factor, float* dev_scale,int* mas_pointer, int* mas);

__device__ int intensity_window_device(int x, int y, int w_window, int h_window, int* pixel_intensity, unsigned short width_original);
__device__ int sqr_intensity_window_device(int x, int y, int w_window, int h_window, int* pixel_intensity_sqr, unsigned short width_original);
