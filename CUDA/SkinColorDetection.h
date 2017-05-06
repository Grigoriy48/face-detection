#pragma once
#include "HSV.h"
#include "LogOpponent.h"
#include "TSL.h"
#include "YIQ.h"
#include "FreeImage.h"
#include <iostream>

class SkinColorDetection
{
private:
	FIBITMAP *originalImage;
	FIBITMAP *ImageRGB;
	FIBITMAP *ImageLogOpponentAndYIQ;
	FIBITMAP *ImageHSV;
	FIBITMAP *ImageTSLAndHSV;
	unsigned short widthOriginal;
	unsigned short heightOriginal;

public:
	SkinColorDetection(int method);
	~SkinColorDetection();
	void SearchSkinByRGB();
	void SearchSkinByLogOpponentAndYIQ();
	void SearchSkinByHSV();
	void SearchSkinByTSLAndHSV();
};

