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
	unsigned short widthOriginal;
	unsigned short heightOriginal;

public:
	SkinColorDetection(int method);
	~SkinColorDetection();
	void SearchSkinByRGB();
	void SearchSkinByLogOpponentAndYIQ();
	void SearchSkinByHSV();
	void SearchSkinByTSL();
};

