#pragma once;

#define max(a,b)	((a)>=(b)?(a):(b))
#define min(a,b)	((a)<(b)?(a):(b))
#define sqr(a)	((a)*(a))

typedef struct {
	int window_w_mini;
	int window_h_mini;
	int n_stages;
	int n_rects;
} TXMLCascade;

typedef struct {
	int n_features;
	int n_rects;
	float stage_threshold;
	int i_feature_start;
	int i_feature_finish;
} TStage;

typedef struct {
	int i_stage;
	int n_rects;
	float left_val;
	float right_val;
	float feature_threshold;
	int i_rect_start;
	int i_rect_finish;
} TFeature;

typedef struct {
	int i_stage;
	int i_feature;
	int i_rect;
	int x;
	int y;
	int w;
	int h;
	int weight;
} TRect;
