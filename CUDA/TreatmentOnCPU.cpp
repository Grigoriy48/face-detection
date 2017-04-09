#include "TreatmentOnCPU.h"
#include "rapidxml.hpp"
#include "ctime"
#include <math.h>
using namespace std;
using namespace rapidxml;

int TreatmentOnCPU::IntensityWindow(int x, int y, int w_window, int h_window) {
	int pixel_4 = pixel_intensity[width_original*(y + h_window) + (x + w_window)];
	int pixel_1 = pixel_intensity[width_original*(y + 0) + (x + 0)];
	int pixel_2 = pixel_intensity[width_original*(y + 0) + (x + w_window)];
	int pixel_3 = pixel_intensity[width_original*(y + h_window) + (x + 0)];
	return (pixel_4 + pixel_1 - pixel_2 - pixel_3);
}

int TreatmentOnCPU::SqrIntensityWindow(int x, int y, int w_window, int h_window) {
	int pixel_4 = pixel_intensity_sqr[width_original*(y + h_window) + (x + w_window)];
	int pixel_1 = pixel_intensity_sqr[width_original*(y + 0) + (x + 0)];
	int pixel_2 = pixel_intensity_sqr[width_original*(y + 0) + (x + w_window)];
	int pixel_3 = pixel_intensity_sqr[width_original*(y + h_window) + (x + 0)];
	return (pixel_4 + pixel_1 - pixel_2 - pixel_3);
}

void TreatmentOnCPU::SearchSkinByYIQ() {
	char *file_with_skin = "skin.jpg";


	RGBQUAD color; 
	RGBQUAD blackColor;
	blackColor.rgbRed = 0;
	blackColor.rgbGreen = 0;
	blackColor.rgbBlue = 0;
	int notSkinPixel = 0;

	for (int x = 0; x <= width_original; x++) {
		for (int y = 0; y <= height_original; y++) {
			FreeImage_GetPixelColor(original_image, x, y, &color);

			int r, g, b;
			r = color.rgbRed;
			g = color.rgbGreen;
			b = color.rgbBlue;

//			int O1 = (r - g) / sqrt(2);
//			int O2 = (r + g - 2 * b) / sqrt(6);
//			int O3 = (r + g + b) / sqrt(3);

			double I = log(g);
			double R = log(r) - log(g);
			double B = log(b) - (log(g) + log(r)) / 2;


			double H = atan2(R,B);
			int S = sqrt(sqr(R) + sqr(B));

			if (!(1 <= H && H <= 2.47) && !(5.199 <= I && I <= 5.2)) {
				FreeImage_SetPixelColor(original_image, x, y, &blackColor);				notSkinPixel++;
			}
		}
	}

	FreeImage_Save(FreeImage_GetFIFFromFilename(file_with_skin), original_image, file_with_skin, 0);
	printf("Save: %s\n", file_with_skin);
//	FreeImage_Unload(original_image);
}


void TreatmentOnCPU::SearchSkinByRGB() {
	char *file_with_skin = "skin.jpg";


	RGBQUAD color;
	RGBQUAD blackColor;
	blackColor.rgbRed = 0;
	blackColor.rgbGreen = 0;
	blackColor.rgbBlue = 0;
	int notSkinPixel = 0;

	for (int x = 0; x <= width_original; x++) {
		for (int y = 0; y <= height_original; y++) {
			FreeImage_GetPixelColor(original_image, x, y, &color);

			int r, g, b;
			r = color.rgbRed;
			g = color.rgbGreen;
			b = color.rgbBlue;

//			r *= factor_for_skin;
//			g *= factor_for_skin;
//			b *= factor_for_skin;

			if (!(r > 90 && g > 40 && b > 20 && r - g > 15 && r > b)) {
				FreeImage_SetPixelColor(original_image, x, y, &blackColor);				notSkinPixel++;
			}
		}
	}

	FreeImage_Save(FreeImage_GetFIFFromFilename(file_with_skin), original_image, file_with_skin, 0);
	printf("Save: %s\n", file_with_skin);
	//	FreeImage_Unload(original_image);
}

TreatmentOnCPU::TreatmentOnCPU() {
}

TreatmentOnCPU::~TreatmentOnCPU() {
}

void TreatmentOnCPU::LoadImage(const char *file_in) {
	original_image = FreeImage_Load(FIF_JPEG, file_in, 0);
	width_original = FreeImage_GetWidth(original_image);
	height_original = FreeImage_GetHeight(original_image);


	SearchSkinByYIQ();
	//SearchSkinByRGB();


	char_picture_original = FreeImage_GetBits(original_image);
	bit_pixel_original = FreeImage_GetBPP(original_image);
	byte_pixel_original = bit_pixel_original / 8;
	bit_width_original = FreeImage_GetPitch(original_image);
	gray_image = FreeImage_ConvertToGreyscale(original_image);
	FreeImage_FlipVertical(gray_image);
	char_picture_gray = FreeImage_GetBits(gray_image);
	bit_pixel_gray = FreeImage_GetBPP(gray_image);
	byte_pixel_gray = bit_pixel_gray / 8;
	bit_width_gray = FreeImage_GetPitch(gray_image);
	pixel_intensity = (int *)malloc(height_original*width_original * sizeof(int));
	pixel_intensity_sqr = (int *)malloc(height_original*width_original * sizeof(int));
	pixel_intensity[0] = char_picture_gray[0];
	skin_image = original_image;
	for (int i = 1; i < width_original; i++) {
		pixel_intensity[0 + i] = pixel_intensity[0 + (i - 1)] + char_picture_gray[0 + i];
	}
	for (int j = 1; j < height_original; j++) {
		pixel_intensity[width_original*j + 0] = pixel_intensity[width_original*(j - 1) + 0] + char_picture_gray[width_original*j + 0];
		for (int i = 1; i < width_original; i++) {
			pixel_intensity[width_original*j + i] =
				char_picture_gray[width_original*j + i] +
				pixel_intensity[width_original*(j - 1) + (i - 0)] +
				pixel_intensity[width_original*(j - 0) + (i - 1)] -
				pixel_intensity[width_original*(j - 1) + (i - 1)];
		}
	}
	pixel_intensity_sqr[0] = sqr(char_picture_gray[0]);
	for (int i = 1; i < width_original; i++) {
		pixel_intensity_sqr[0 + i] = pixel_intensity_sqr[0 + (i - 1)] + sqr(char_picture_gray[0 + i]);
	}
	for (int j = 1; j < height_original; j++) {
		pixel_intensity_sqr[width_original*j + 0] = pixel_intensity_sqr[width_original*(j - 1) + 0] + sqr(char_picture_gray[width_original*j + 0]);
		for (int i = 1; i < width_original; i++) {
			pixel_intensity_sqr[width_original*(j - 0) + (i - 0)] =
				sqr(char_picture_gray[width_original*j + i]) +
				pixel_intensity_sqr[width_original*(j - 1) + (i - 0)] +
				pixel_intensity_sqr[width_original*(j - 0) + (i - 1)] -
				pixel_intensity_sqr[width_original*(j - 1) + (i - 1)];
		}
	}
}

void TreatmentOnCPU::LoadCascade(const char *haar_cascade) {
	ifstream stream(haar_cascade);
	string cascade, line;
	while (getline(stream, line))
		cascade += line;
	vector<char> xml_copy(cascade.begin(), cascade.end());
	xml_copy.push_back('\0');
	xml_document<> xml_doc;
	xml_doc.parse<0>(&xml_copy[0]);
	xml_node<> *node1 = xml_doc.first_node("opencv_storage");
	xml_node<> *node2 = node1->first_node();
	xml_node<> *node3 = node2->first_node("size");
	char *str;
	str = strtok(node3->value(), " ");
	this->cascade.window_w_mini = atoi(str);
	str = strtok(0x00, " ");
	this->cascade.window_h_mini = atoi(str);
	this->cascade.n_stages = 0;
	this->cascade.n_rects = 0;
	int i_stage = 0;
	int i_feature = 0;
	int i_rect = 0;
	xml_node<> *node4 = node2->first_node("stages");
	xml_node<> *node5 = node4->first_node("_");
	do {
		xml_node<> *node6 = node5->first_node("trees");
		xml_node<> *node9 = node5->first_node("stage_threshold");
		float stage_threshold = atof(node9->value());
		this->stages[i_stage].stage_threshold = stage_threshold;
		this->stages[i_stage].n_features = 0;
		this->stages[i_stage].n_rects = 0;
		this->stages[i_stage].i_feature_start = 65535;
		this->stages[i_stage].i_feature_finish = -1;
		xml_node<> *node7 = node6->first_node("_");
		xml_node<> *node8 = node7->first_node("_");
		do {
			if (this->stages[i_stage].i_feature_start > i_feature)
				this->stages[i_stage].i_feature_start = i_feature;
			if (this->stages[i_stage].i_feature_finish < i_feature)
				this->stages[i_stage].i_feature_finish = i_feature;
			xml_node<> *node13 = node8->first_node("feature");
			xml_node<> *node14 = node13->first_node("rects");
			xml_node<> *node15 = node14->first_node("_");
			xml_node<> *node10 = node8->first_node("threshold");
			float feature_threshold = atof(node10->value());
			this->features[i_feature].feature_threshold = feature_threshold;
			xml_node<> *node11 = node8->first_node("left_val");
			float left_val = -1.0;
			left_val = atof(node11->value());
			xml_node<> *node12 = node8->first_node("right_val");
			float right_val = -1.0;
			right_val = atof(node12->value());
			this->features[i_feature].i_stage = i_stage;
			this->features[i_feature].left_val = left_val;
			this->features[i_feature].right_val = right_val;
			this->stages[i_stage].n_features++;
			this->features[i_feature].i_rect_start = 65535;
			this->features[i_feature].i_rect_finish = -1;
			do {
				if (this->features[i_feature].i_rect_start > i_rect)
					this->features[i_feature].i_rect_start = i_rect;
				if (this->features[i_feature].i_rect_finish < i_rect)
					this->features[i_feature].i_rect_finish = i_rect;
				int temp;
				char *temp_str_haar_cascade;
				temp_str_haar_cascade = strtok(node15->value(), " ");
				temp = atoi(temp_str_haar_cascade);
				this->rects[i_rect].x = temp;
				temp_str_haar_cascade = strtok(0x00, " ");
				temp = atoi(temp_str_haar_cascade);
				this->rects[i_rect].y = temp;
				temp_str_haar_cascade = strtok(0x00, " ");
				temp = atoi(temp_str_haar_cascade);
				this->rects[i_rect].w = temp;
				temp_str_haar_cascade = strtok(0x00, " ");
				temp = atoi(temp_str_haar_cascade);
				this->rects[i_rect].h = temp;
				temp_str_haar_cascade = strtok(0x00, " ");
				temp = atoi(temp_str_haar_cascade);
				this->rects[i_rect].weight = temp;
				this->features[i_feature].n_rects++;
				this->stages[i_stage].n_rects++;
				this->cascade.n_rects++;
				i_rect++;
				node15 = node15->next_sibling("_");
				if (!node15)
					break;
			} while (1);
			i_feature++;
			node7 = node7->next_sibling("_");
			if (!node7)
				break;
			node8 = node7->first_node("_");
			if (!node8)
				break;
		} while (1);
		this->cascade.n_stages++;
		i_stage++;
		node5 = node5->next_sibling("_");
		if (!node5)
			break;
	} while (1);
}

void TreatmentOnCPU::Recognize(float step_factor) {
	int count_face = 0;
	float factor = 1.0;
	int window_w = cascade.window_w_mini;
	int window_h = cascade.window_h_mini;
	do {
		float sum_cascade = 0;
		window_w = (int)floor(cascade.window_w_mini*factor);
		window_h = (int)floor(cascade.window_h_mini*factor);
		int x1, y1, x2, y2;
		int x_step = max(1, min(4, window_w / 10));
		int y_step = max(1, min(4, window_h / 10));
		for (y1 = 0; y1 < height_original - window_h; y1 += y_step) {
			y2 = y1 + window_h;
			for (x1 = 0; x1 < width_original - window_w; x1 += x_step) {
				x2 = x1 + window_w;
				float mean = IntensityWindow(x1, y1, window_w, window_h) * 1 / float(window_w*window_h);
				float variance = SqrIntensityWindow(x1, y1, window_w, window_h) * 1 / float(window_w*window_h) - sqr(mean);
				float stddev = sqrt(variance);
				if (stddev < 10.0)
					continue;
				bool failed = false;
				for (int i_stage = 0; i_stage < cascade.n_stages; i_stage++) {
					float sum_stage = 0.0;
					for (int i_feature = this->stages[i_stage].i_feature_start;
						i_feature <= this->stages[i_stage].i_feature_finish;
						i_feature++) {
						int sum_feature = 0.0;
						for (int i_rect = this->features[i_feature].i_rect_start;
							i_rect <= this->features[i_feature].i_rect_finish;
							i_rect++) {
							int weight = (this->rects[i_rect].weight);
							int x_haar_primitive = this->rects[i_rect].x*factor;
							int y_haar_primitive = this->rects[i_rect].y*factor;
							int w_haar_primitive = this->rects[i_rect].w*factor;
							int h_haar_primitive = this->rects[i_rect].h*factor;
							int x_haar_primitive_in_image = x1 + x_haar_primitive;
							int y_haar_primitive_in_image = y1 + y_haar_primitive;
							sum_feature += (IntensityWindow(x_haar_primitive_in_image, y_haar_primitive_in_image, w_haar_primitive, h_haar_primitive)*weight);
						}
						if (sum_feature * 1 / float(window_w*window_h) < this->features[i_feature].feature_threshold * stddev)
							sum_stage += this->features[i_feature].left_val;
						else
							sum_stage += this->features[i_feature].right_val;
					}
					if (sum_stage < this->stages[i_stage].stage_threshold) {
						failed = true;
						break;
					}
				}
				if (failed == false) {
					for (int x = x1; x <= x2; x++) {
						char_picture_original[bit_width_original*(height_original - y1) + byte_pixel_original*x + 0] = 0xff;
						char_picture_original[bit_width_original*(height_original - y1) + byte_pixel_original*x + 1] = 0xff;
						char_picture_original[bit_width_original*(height_original - y1) + byte_pixel_original*x + 2] = 0xff;
					}
					for (int x = x1; x <= x2; x++) {
						char_picture_original[bit_width_original*(height_original - y2) + byte_pixel_original*x + 0] = 0xff;
						char_picture_original[bit_width_original*(height_original - y2) + byte_pixel_original*x + 1] = 0xff;
						char_picture_original[bit_width_original*(height_original - y2) + byte_pixel_original*x + 2] = 0xff;
					}
					for (int y = y1; y <= y2; y++) {
						char_picture_original[bit_width_original*(height_original - y) + byte_pixel_original*x1 + 0] = 0xff;
						char_picture_original[bit_width_original*(height_original - y) + byte_pixel_original*x1 + 1] = 0xff;
						char_picture_original[bit_width_original*(height_original - y) + byte_pixel_original*x1 + 2] = 0xff;
					}
					for (int y = y1; y <= y2; y++) {
						char_picture_original[bit_width_original*(height_original - y) + byte_pixel_original*x2 + 0] = 0xff;
						char_picture_original[bit_width_original*(height_original - y) + byte_pixel_original*x2 + 1] = 0xff;
						char_picture_original[bit_width_original*(height_original - y) + byte_pixel_original*x2 + 2] = 0xff;
					}
					count_face++;
					printf("%d %d %d %d %d \n", x1, y1, x2, y2, count_face);
				}
			}
		}
		factor *= step_factor;
	} while (min(width_original, height_original) >= min(window_w, window_h));
}

void TreatmentOnCPU::allocation() {
	for (int k = 0; k < mas_pointer; k = k + 4) {
		int x, y, x1, y1, x2, y2;
		x1 = mas[k];
		y1 = mas[k + 1];
		x2 = mas[k + 2];
		y2 = mas[k + 3];
		y = y1;
		for (x = x1; x <= x2; x++) {
			char_picture_original[bit_width_original*(height_original - y1) + byte_pixel_original*x + 0] = 0x00;
			char_picture_original[bit_width_original*(height_original - y1) + byte_pixel_original*x + 1] = 0x00;
			char_picture_original[bit_width_original*(height_original - y1) + byte_pixel_original*x + 2] = 0x00;
		}
		y = y2;
		for (x = x1; x <= x2; x++) {
			char_picture_original[bit_width_original*(height_original - y2) + byte_pixel_original*x + 0] = 0x00;
			char_picture_original[bit_width_original*(height_original - y2) + byte_pixel_original*x + 1] = 0x00;
			char_picture_original[bit_width_original*(height_original - y2) + byte_pixel_original*x + 2] = 0x00;
		}
		x = x1;
		for (y = y1; y <= y2; y++) {
			char_picture_original[bit_width_original*(height_original - y) + byte_pixel_original*x1 + 0] = 0x00;
			char_picture_original[bit_width_original*(height_original - y) + byte_pixel_original*x1 + 1] = 0x00;
			char_picture_original[bit_width_original*(height_original - y) + byte_pixel_original*x1 + 2] = 0x00;
		}
		x = x2;
		for (y = y1; y <= y2; y++) {
			char_picture_original[bit_width_original*(height_original - y) + byte_pixel_original*x2 + 0] = 0x00;
			char_picture_original[bit_width_original*(height_original - y) + byte_pixel_original*x2 + 1] = 0x00;
			char_picture_original[bit_width_original*(height_original - y) + byte_pixel_original*x2 + 2] = 0x00;
		}
	}

}

void TreatmentOnCPU::Treatment() {

	unsigned int start_time = clock();
	char *file_in = "in.jpg";
	char *file_out = "out.jpg";
	char *haar_cascade = "haarcascade.xml";

//	printf("Enter factor for detecter skin = ");
//	scanf("%f", &factor_for_skin);
	//	SearchSkin();


	LoadImage(file_in);


	LoadCascade(haar_cascade);
	unsigned int end_time = clock();
	unsigned int search_time = end_time - start_time;
	printf("Preparation time = %d\n", search_time);
	printf("Select the method of recognition \n  1 - CPU \n  2 - GPU parallelization scale \n  3 - GPU parallelization elements\n");
	int command;
	scanf("%d", &command);
	switch (command) {
	case 1:
		start_time = clock();
		Recognize(1.2);
		end_time = clock();
		search_time = end_time - start_time;
		printf("Recognition time = %d\n", search_time);
		break;
	case 2:

		mas = (int*)malloc(sizeof(int) * 1000);
		mas_pointer = 0;

		start_time = clock();
		ParallelizationScale(&cascade, stages, features, rects, width_original, height_original,
			char_picture_original, byte_pixel_original, bit_width_original, 1.2,
			pixel_intensity, pixel_intensity_sqr, &mas_pointer, &mas);
		end_time = clock();
		search_time = end_time - start_time;
		printf("Recognition time = %d\n", search_time);

	//	allocation();

		break;
	case 3:
		start_time = clock();
		ParallelizationElements(&cascade, stages, features, rects, width_original, height_original,
			char_picture_original, byte_pixel_original, bit_width_original, 1.2,
			pixel_intensity, pixel_intensity_sqr);
		end_time = clock();
		search_time = end_time - start_time;
		printf("Recognition time = %d\n", search_time);
		break;
	}


	FreeImage_Save(FreeImage_GetFIFFromFilename(file_out), original_image, file_out, 0);
	printf("Save: %s\n", file_out);
	FreeImage_Unload(original_image);
	FreeImage_Unload(gray_image);
	cin.ignore();
	cin.ignore();
}
