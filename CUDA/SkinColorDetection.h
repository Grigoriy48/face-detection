#pragma once

#include "HSV.h"
#include "LogOpponent.h"
#include "TSL.h"
#include "YIQ.h"
#include "FreeImage.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>


using namespace std;

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

	double logOpponentMinH;
	double logOpponenthsvMaxH;

	double yiqMinI;
	double yiqMaxI;

	double hsvMinH;
	double hsvMaxH;
	double hsvMinS;
	double hsvMaxS;
	double hsvMinV;
	double hsvMaxV;

	double tslMinT;
	double tslMaxT;
	double tslMinS;
	double tslMaxS;

	void readFilters();
	

public:
	SkinColorDetection(int method);
	~SkinColorDetection();
	void SearchSkinByRGB();
	void SearchSkinByLogOpponentAndYIQ();
	void SearchSkinByHSV();
	void SearchSkinByTSLAndHSV();
};

