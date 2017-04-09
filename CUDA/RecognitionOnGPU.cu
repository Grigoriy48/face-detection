#include "RecognitionOnGPU.cuh"
#include <cuda.h>
#include <stdio.h>
#include <cuda_runtime_api.h>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <assert.h>
#include <stdlib.h>
#include <math.h>


void ParallelizationElements(TXMLCascade* cascade, TStage* stages, TFeature* features, TRect* rects,
	unsigned short width_original, unsigned short height_original, unsigned char* char_picture_original,
	unsigned short byte_pixel_original, unsigned short bit_width_original, float step_factor,
	int* pixel_intensity, int* pixel_intensity_sqr) {
	float factor = 1.0;
	float *dev_factor;
	cudaMalloc((void**)&dev_factor, sizeof(float));
	cudaMemcpy(dev_factor, &factor, sizeof(float), cudaMemcpyHostToDevice);
	TXMLCascade* dev_cascade;
	cudaMalloc((void**)&dev_cascade, sizeof(TXMLCascade));
	cudaMemcpy(dev_cascade, cascade, sizeof(TXMLCascade), cudaMemcpyHostToDevice);
	TStage* dev_stages;
	cudaMalloc((void**)&dev_stages, sizeof(TStage) * 30);
	cudaMemcpy(dev_stages, stages, sizeof(TStage) * 30, cudaMemcpyHostToDevice);
	TFeature* dev_features;
	cudaMalloc((void**)&dev_features, sizeof(TFeature) * 7000);
	cudaMemcpy(dev_features, features, sizeof(TFeature) * 7000, cudaMemcpyHostToDevice);
	TRect* dev_rects;
	cudaMalloc((void**)&dev_rects, sizeof(TRect) * 11000);
	cudaMemcpy(dev_rects, rects, sizeof(TRect) * 11000, cudaMemcpyHostToDevice);
	unsigned short* dev_width_original;
	cudaMalloc((void**)&dev_width_original, sizeof(unsigned short));
	cudaMemcpy(dev_width_original, &width_original, sizeof(unsigned short), cudaMemcpyHostToDevice);
	unsigned short* dev_height_original;
	cudaMalloc((void**)&dev_height_original, sizeof(unsigned short));
	cudaMemcpy(dev_height_original, &height_original, sizeof(unsigned short), cudaMemcpyHostToDevice);
	unsigned char* dev_char_picture_original;
	cudaMalloc((void**)&dev_char_picture_original, sizeof(unsigned char) * 3 * height_original*width_original);
	cudaMemcpy(dev_char_picture_original, char_picture_original, sizeof(unsigned char) * 3 * height_original*width_original, cudaMemcpyHostToDevice);
	unsigned short* dev_byte_pixel_original;
	cudaMalloc((void**)&dev_byte_pixel_original, sizeof(unsigned short));
	cudaMemcpy(dev_byte_pixel_original, &byte_pixel_original, sizeof(unsigned short), cudaMemcpyHostToDevice);
	unsigned short* dev_bit_width_original;
	cudaMalloc((void**)&dev_bit_width_original, sizeof(unsigned short));
	cudaMemcpy(dev_bit_width_original, &bit_width_original, sizeof(unsigned short), cudaMemcpyHostToDevice);
	float* dev_step_factor;
	cudaMalloc((void**)&dev_step_factor, sizeof(float));
	cudaMemcpy(dev_step_factor, &step_factor, sizeof(float), cudaMemcpyHostToDevice);
	int* dev_pixel_intensity;
	cudaMalloc((void**)&dev_pixel_intensity, width_original*height_original * 4);
	cudaMemcpy(dev_pixel_intensity, pixel_intensity, width_original*height_original * 4, cudaMemcpyHostToDevice);
	int* dev_pixel_intensity_sqr;
	cudaMalloc((void**)&dev_pixel_intensity_sqr, width_original*height_original * 4);
	cudaMemcpy(dev_pixel_intensity_sqr, pixel_intensity_sqr, width_original*height_original * 4, cudaMemcpyHostToDevice);
	int window_w = cascade->window_w_mini;
	int window_h = cascade->window_h_mini;
	do {
		ParallelizationElementsKernel <<< 512, 512 >>>(dev_cascade, dev_stages, dev_features, dev_rects,
			dev_width_original, dev_height_original, dev_char_picture_original, dev_byte_pixel_original,
			dev_bit_width_original, dev_step_factor, dev_pixel_intensity, dev_pixel_intensity_sqr,
			dev_factor);
		factor *= step_factor;
		cudaMemcpy(dev_factor, &factor, sizeof(float), cudaMemcpyHostToDevice);
		window_w = floor(cascade->window_w_mini*factor);
		window_h = floor(cascade->window_h_mini*factor);
	} while (min(width_original, height_original) >= min(window_w, window_h));

	printf("Error: %s\n", cudaGetErrorString(cudaGetLastError()));
	
	cudaMemcpy(char_picture_original, dev_char_picture_original, sizeof(unsigned char) * 3 * height_original*width_original, cudaMemcpyDeviceToHost);
}

__global__ void ParallelizationElementsKernel(TXMLCascade* cascade, TStage* stages, TFeature* features, TRect* rects,
	unsigned short* width_original, unsigned short* height_original, unsigned char* char_picture_original,
	unsigned short* byte_pixel_original, unsigned short* bit_width_original, float* step_factor,
	int* pixel_intensity, int* pixel_intensity_sqr, float* factor) {
	int window_w = floor(cascade->window_w_mini**factor);
	int window_h = floor(cascade->window_h_mini**factor);
	unsigned short x1, y1;
	int x_step = ((1) >= (((4)<((window_w) / 10) ? (4) : ((window_w) / 10))) ? (1) : (((4)<((window_w) / 10) ? (4) : ((window_w) / 10))));
	int y_step = ((1) >= (((4)<((window_h) / 10) ? (4) : ((window_h) / 10))) ? (1) : (((4)<((window_h) / 10) ? (4) : ((window_h) / 10))));
	y1 = y_step *  blockIdx.x;
	if (y1 < *height_original - window_h) {
		int	x_thread_step = 0;
		for (x1 = 0; x1 < *width_original - window_w; x_thread_step += 512) {
			x1 = x_thread_step + x_step *  threadIdx.x;
			if (x1 < 512 + x_thread_step) {
				float mean = intensity_window_device(x1, y1, window_w, window_h, pixel_intensity, *width_original) * 1 / float(window_w*window_h);
				float variance = sqr_intensity_window_device(x1, y1, window_w, window_h, pixel_intensity_sqr, *width_original) * 1 / float(window_w*window_h) - (mean*mean);
				float stddev = 1.0;
				stddev = sqrt(variance);
				if (stddev > 10.0) {
					bool f_failed = false;
					for (int i_stage = 0; i_stage < cascade->n_stages; i_stage = i_stage + 1) {
						float sum_stage = 0.0;
						for (int i_feature = stages[i_stage].i_feature_start;
							i_feature <= stages[i_stage].i_feature_finish;
							i_feature = i_feature + 1) {
							int sum_feature = 0.0;
							for (int i_rect = features[i_feature].i_rect_start;
								i_rect <= features[i_feature].i_rect_finish;
								i_rect = i_rect + 1) {
								sum_feature += (intensity_window_device(x1 + rects[i_rect].x**factor, y1 + rects[i_rect].y**factor,
									rects[i_rect].w**factor, rects[i_rect].h**factor, pixel_intensity, *width_original)*rects[i_rect].weight);
							}
							if (sum_feature * 1 / float(window_w*window_h) < features[i_feature].feature_threshold * stddev)
								sum_stage += features[i_feature].left_val;
							else
								sum_stage += features[i_feature].right_val;
						}
						if (sum_stage < stages[i_stage].stage_threshold) {
							f_failed = true;
							break;
						}
					}
					if (f_failed == false) {
						printf("%d %d %d %d  \n", x1, y1, x1 + window_w, y1 + window_h);
						unsigned short x2 = x1 + window_w;
						unsigned short y2 = y1 + window_h;
						for (int x = x1; x <= x2; x++) {
							char_picture_original[*bit_width_original*(*height_original - y1) + *byte_pixel_original*x + 0] = 0x00;
							char_picture_original[*bit_width_original*(*height_original - y1) + *byte_pixel_original*x + 1] = 0x00;
							char_picture_original[*bit_width_original*(*height_original - y1) + *byte_pixel_original*x + 2] = 0x00;
						}
						for (int x = x1; x <= x2; x++) {
							char_picture_original[*bit_width_original*(*height_original - y2) + *byte_pixel_original*x + 0] = 0x00;
							char_picture_original[*bit_width_original*(*height_original - y2) + *byte_pixel_original*x + 1] = 0x00;
							char_picture_original[*bit_width_original*(*height_original - y2) + *byte_pixel_original*x + 2] = 0x00;
						}
						for (int y = y1; y <= y2; y++) {
							char_picture_original[*bit_width_original*(*height_original - y) + *byte_pixel_original*x1 + 0] = 0x00;
							char_picture_original[*bit_width_original*(*height_original - y) + *byte_pixel_original*x1 + 1] = 0x00;
							char_picture_original[*bit_width_original*(*height_original - y) + *byte_pixel_original*x1 + 2] = 0x00;
						}
						for (int y = y1; y <= y2; y++) {
							char_picture_original[*bit_width_original*(*height_original - y) + *byte_pixel_original*x2 + 0] = 0x00;
							char_picture_original[*bit_width_original*(*height_original - y) + *byte_pixel_original*x2 + 1] = 0x00;
							char_picture_original[*bit_width_original*(*height_original - y) + *byte_pixel_original*x2 + 2] = 0x00;
						}
					}
				}
			}
		}
	}
}

void ParallelizationScale(TXMLCascade* cascade, TStage* stages, TFeature* features, TRect* rects,
	unsigned short width_original, unsigned short height_original, unsigned char* char_picture_original,
	unsigned short byte_pixel_original, unsigned short bit_width_original, float step_factor,
	int* pixel_intensity, int* pixel_intensity_sqr, int* mas_pointer, int** mas) {

	unsigned char* dev_char_picture_original;
	cudaMalloc((void**)&dev_char_picture_original, sizeof(unsigned char) * 3 * height_original*width_original);
	cudaMemcpy(dev_char_picture_original, char_picture_original, sizeof(unsigned char) * 3 * height_original*width_original, cudaMemcpyHostToDevice);

	int* dev_mas_pointer;
	cudaMalloc((void**)&dev_mas_pointer, sizeof(int));

	int* dev_mas;
	cudaMalloc((void**)&dev_mas, sizeof(int) * 512);

	float factor = 1.0;
	float *dev_factor;
	cudaMalloc((void**)&dev_factor, sizeof(float));
	cudaMemcpy(dev_factor, &factor, sizeof(float), cudaMemcpyHostToDevice);
	TXMLCascade* dev_cascade;
	cudaMalloc((void**)&dev_cascade, sizeof(TXMLCascade));
	cudaMemcpy(dev_cascade, cascade, sizeof(TXMLCascade), cudaMemcpyHostToDevice);
	TStage* dev_stages;
	cudaMalloc((void**)&dev_stages, sizeof(TStage) * 30);
	cudaMemcpy(dev_stages, stages, sizeof(TStage) * 30, cudaMemcpyHostToDevice);
	TFeature* dev_features;
	cudaMalloc((void**)&dev_features, sizeof(TFeature) * 7000);
	cudaMemcpy(dev_features, features, sizeof(TFeature) * 7000, cudaMemcpyHostToDevice);
	TRect* dev_rects;
	cudaMalloc((void**)&dev_rects, sizeof(TRect) * 11000);
	cudaMemcpy(dev_rects, rects, sizeof(TRect) * 11000, cudaMemcpyHostToDevice);
	unsigned short* dev_width_original;
	cudaMalloc((void**)&dev_width_original, sizeof(unsigned short));
	cudaMemcpy(dev_width_original, &width_original, sizeof(unsigned short), cudaMemcpyHostToDevice);
	unsigned short* dev_height_original;
	cudaMalloc((void**)&dev_height_original, sizeof(unsigned short));
	cudaMemcpy(dev_height_original, &height_original, sizeof(unsigned short), cudaMemcpyHostToDevice);
	unsigned short* dev_byte_pixel_original;
	cudaMalloc((void**)&dev_byte_pixel_original, sizeof(unsigned short));
	cudaMemcpy(dev_byte_pixel_original, &byte_pixel_original, sizeof(unsigned short), cudaMemcpyHostToDevice);
	unsigned short* dev_bit_width_original;
	cudaMalloc((void**)&dev_bit_width_original, sizeof(unsigned short));
	cudaMemcpy(dev_bit_width_original, &bit_width_original, sizeof(unsigned short), cudaMemcpyHostToDevice);
	float* dev_step_factor;
	cudaMalloc((void**)&dev_step_factor, sizeof(float));
	cudaMemcpy(dev_step_factor, &step_factor, sizeof(float), cudaMemcpyHostToDevice);
	int* dev_pixel_intensity;
	cudaMalloc((void**)&dev_pixel_intensity, width_original*height_original * 4);
	cudaMemcpy(dev_pixel_intensity, pixel_intensity, width_original*height_original * 4, cudaMemcpyHostToDevice);
	int* dev_pixel_intensity_sqr;
	cudaMalloc((void**)&dev_pixel_intensity_sqr, width_original*height_original * 4);
	cudaMemcpy(dev_pixel_intensity_sqr, pixel_intensity_sqr, width_original*height_original * 4, cudaMemcpyHostToDevice);
	int window_w, window_h;
	float scale[100];
	int k = 0;
	do {
		window_w = (int)floor(cascade->window_w_mini*factor);
		window_h = (int)floor(cascade->window_h_mini*factor);
		scale[k] = factor;
		factor *= step_factor;
		k++;
	} while (min(width_original, height_original) >= min(window_w, window_h));
	printf("block = %d;  \n", k);
	float* dev_scale;
	cudaMalloc((void**)&dev_scale, sizeof(float) * 100);
	cudaMemcpy(dev_scale, scale, sizeof(float) * 100, cudaMemcpyHostToDevice);


	ParallelizationScaleKernel <<< k, 512 >>>(dev_cascade, dev_stages, dev_features, dev_rects,
		dev_width_original, dev_height_original, dev_char_picture_original, dev_byte_pixel_original,
		dev_bit_width_original, dev_step_factor, dev_pixel_intensity, dev_pixel_intensity_sqr,
		dev_factor, dev_scale, dev_mas_pointer, dev_mas);
	

	cudaThreadSynchronize();

	printf("Error 1: %s\n", cudaGetErrorString(cudaGetLastError()));
	
	cudaMemcpy(char_picture_original, dev_char_picture_original, sizeof(unsigned char) * 3 * height_original*width_original, cudaMemcpyDeviceToHost);

		/*printf("Error 2: %s\n", cudaGetErrorString(cudaGetLastError()));

	cudaMemcpy( &mas, dev_mas, sizeof(int)*512*k,cudaMemcpyDeviceToHost);

	printf("Error 3: %s\n", cudaGetErrorString(cudaGetLastError()));

	cudaMemcpy(mas_pointer, dev_mas_pointer, sizeof(int), cudaMemcpyDeviceToHost);

	printf("Error 4: %s\n", cudaGetErrorString(cudaGetLastError()));*/

}

__global__ void ParallelizationScaleKernel(TXMLCascade* cascade, TStage* stages, TFeature* features, TRect* rects,
	unsigned short* width_original, unsigned short* height_original, unsigned char* char_picture_original,
	unsigned short* byte_pixel_original, unsigned short* bit_width_original, float* step_factor,
	int* pixel_intensity, int* pixel_intensity_sqr, float* factor, float* scale, int* mas_pointer, int* mas) {
	int window_w = floor(cascade->window_w_mini*scale[blockIdx.x]);
	int window_h = floor(cascade->window_h_mini*scale[blockIdx.x]);
	unsigned short x1, y1;
	int x_step = ((1) >= (((4)<((window_w) / 10) ? (4) : ((window_w) / 10))) ? (1) : (((4)<((window_w) / 10) ? (4) : ((window_w) / 10))));
	int y_step = ((1) >= (((4)<((window_h) / 10) ? (4) : ((window_h) / 10))) ? (1) : (((4)<((window_h) / 10) ? (4) : ((window_h) / 10))));
	for (y1 = 0; y1 <= *height_original - 1 - window_h; y1 += y_step) {
		x1 = x_step *  threadIdx.x;
		if (x1 < *width_original - 1 - window_w) {
			float mean = intensity_window_device(x1, y1, window_w, window_h, pixel_intensity, *width_original) * 1 / float(window_w*window_h);
			float variance = sqr_intensity_window_device(x1, y1, window_w, window_h, pixel_intensity_sqr, *width_original) * 1 / float(window_w*window_h) - (mean*mean);
			float stddev = 1.0;
			stddev = sqrt(variance);
			if (stddev < 10.0)
				continue;
			int f_failed = 0;
			for (int i_stage = 0; i_stage < cascade->n_stages; i_stage = i_stage + 1) {
				float sum_stage = 0.0;
				for (int i_feature = stages[i_stage].i_feature_start;
					i_feature <= stages[i_stage].i_feature_finish;
					i_feature = i_feature + 1) {
					int sum_feature = 0.0;
					for (int i_rect = features[i_feature].i_rect_start;
						i_rect <= features[i_feature].i_rect_finish;
						i_rect = i_rect + 1) {
						sum_feature += (intensity_window_device(x1 + rects[i_rect].x*scale[blockIdx.x], y1 + rects[i_rect].y*scale[blockIdx.x],
							rects[i_rect].w*scale[blockIdx.x], rects[i_rect].h*scale[blockIdx.x], pixel_intensity,
							*width_original)*rects[i_rect].weight);
					}
					float leafth = features[i_feature].feature_threshold * stddev;
					if (sum_feature * 1 / float(window_w*window_h) < leafth)
						sum_stage += features[i_feature].left_val;
					else
						sum_stage += features[i_feature].right_val;
				}
				if (sum_stage < stages[i_stage].stage_threshold) {
					f_failed = 1;
					break;
				}
			}
			if (f_failed == false) {
				printf("%d %d %d %d  \n", x1, y1, x1 + window_w, y1 + window_h);
				unsigned short x2 = x1 + window_w;
				unsigned short y2 = y1 + window_h;

				/*		atomicAdd( &mas[*mas_pointer], x1);
				atomicAdd( &mas[*mas_pointer+1] , y1 );
				atomicAdd( &mas[*mas_pointer+2] , x2 );
				atomicAdd( &mas[*mas_pointer+3] , y2 );

				atomicAdd(mas_pointer, 4);
				printf("dev_mas_pointer=%d \n", *mas_pointer);*/


				for (int x = x1; x <= x2; x++) {
					char_picture_original[*bit_width_original*(*height_original - y1) + *byte_pixel_original*x + 0] = 0x00;
					char_picture_original[*bit_width_original*(*height_original - y1) + *byte_pixel_original*x + 1] = 0x00;
					char_picture_original[*bit_width_original*(*height_original - y1) + *byte_pixel_original*x + 2] = 0x00;
				}
				for (int x = x1; x <= x2; x++) {
					char_picture_original[*bit_width_original*(*height_original - y2) + *byte_pixel_original*x + 0] = 0x00;
					char_picture_original[*bit_width_original*(*height_original - y2) + *byte_pixel_original*x + 1] = 0x00;
					char_picture_original[*bit_width_original*(*height_original - y2) + *byte_pixel_original*x + 2] = 0x00;
				}
				for (int y = y1; y <= y2; y++) {
					char_picture_original[*bit_width_original*(*height_original - y) + *byte_pixel_original*x1 + 0] = 0x00;
					char_picture_original[*bit_width_original*(*height_original - y) + *byte_pixel_original*x1 + 1] = 0x00;
					char_picture_original[*bit_width_original*(*height_original - y) + *byte_pixel_original*x1 + 2] = 0x00;
				}
				for (int y = y1; y <= y2; y++) {
					char_picture_original[*bit_width_original*(*height_original - y) + *byte_pixel_original*x2 + 0] = 0x00;
					char_picture_original[*bit_width_original*(*height_original - y) + *byte_pixel_original*x2 + 1] = 0x00;
					char_picture_original[*bit_width_original*(*height_original - y) + *byte_pixel_original*x2 + 2] = 0x00;
				}
			} 
		}
	}
}

__device__ int intensity_window_device(int x, int y, int w_window, int h_window, int* pixel_intensity, unsigned short width_original) {
	int pixel_4 = pixel_intensity[width_original*(y + h_window) + (x + w_window)];
	int pixel_1 = pixel_intensity[width_original*(y + 0) + (x + 0)];
	int pixel_2 = pixel_intensity[width_original*(y + 0) + (x + w_window)];
	int pixel_3 = pixel_intensity[width_original*(y + h_window) + (x + 0)];
	return (pixel_4 + pixel_1 - pixel_2 - pixel_3);
}

__device__ int sqr_intensity_window_device(int x, int y, int w_window, int h_window, int* pixel_intensity_sqr, unsigned short width_original) {
	int pixel_4 = pixel_intensity_sqr[width_original*(y + h_window) + (x + w_window)];
	int pixel_1 = pixel_intensity_sqr[width_original*(y + 0) + (x + 0)];
	int pixel_2 = pixel_intensity_sqr[width_original*(y + 0) + (x + w_window)];
	int pixel_3 = pixel_intensity_sqr[width_original*(y + h_window) + (x + 0)];
	return (pixel_4 + pixel_1 - pixel_2 - pixel_3);
}
