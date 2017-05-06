#include "SkinColorDetection.h"




SkinColorDetection::SkinColorDetection(int method)
{


//	unsigned int start_time = clock();
	char *fileIn = "in.jpg";
	char *fileOut = "out.jpg";
//	char *haar_cascade = "haarcascade.xml";

	originalImage = FreeImage_Load(FIF_JPEG, fileIn, 0);
	widthOriginal = FreeImage_GetWidth(originalImage);
	heightOriginal = FreeImage_GetHeight(originalImage);

	ImageRGB = FreeImage_Clone(originalImage);
	ImageLogOpponentAndYIQ = FreeImage_Clone(originalImage);
	ImageHSV = FreeImage_Clone(originalImage);
	ImageTSLAndHSV = FreeImage_Clone(originalImage);

	
	//SearchSkinByRGB();

	//	printf("Enter factor for detecter skin = ");
	//	scanf("%f", &factor_for_skin);
	//	SearchSkin();


//	LoadImage(file_in);

	switch (method)
	{
	case 1:
		SearchSkinByRGB();
		break;
	case 2:
		SearchSkinByLogOpponentAndYIQ();
		break;
	case 3:
		SearchSkinByHSV();
		break;
	case 4:
		SearchSkinByTSLAndHSV();
		break;
	case 5:
		SearchSkinByRGB();
		SearchSkinByLogOpponentAndYIQ();
		SearchSkinByHSV();
		SearchSkinByTSLAndHSV();
		break;

	}



	system("pause");


//	printf("1 - RGB\n");
//	printf("2 - Log Opponent & YIQ\n");
//	printf("3 - HSV\n");
//	printf("4 - TSL\n");
}


SkinColorDetection::~SkinColorDetection()
{
}


void SkinColorDetection::SearchSkinByRGB() {
	char *fileSkinRGB = "skinRGB.jpg";

	RGBQUAD color;
	RGBQUAD blackColor;
	blackColor.rgbRed = 0;
	blackColor.rgbGreen = 0;
	blackColor.rgbBlue = 0;
	int notSkinPixel = 0;

	for (int x = 0; x <= widthOriginal; x++) {
		for (int y = 0; y <= heightOriginal; y++) {
			FreeImage_GetPixelColor(ImageRGB, x, y, &color);

			int r, g, b;
			r = color.rgbRed;
			g = color.rgbGreen;
			b = color.rgbBlue;

			//			r *= factor_for_skin;
			//			g *= factor_for_skin;
			//			b *= factor_for_skin;

			if (!(r > 90 && g > 40 && b > 20 && r - g > 15 && r > b)) {
				FreeImage_SetPixelColor(ImageRGB, x, y, &blackColor);
				notSkinPixel++;
			}
		}
	}

	FreeImage_Save(FreeImage_GetFIFFromFilename(fileSkinRGB), ImageRGB, fileSkinRGB, 0);
	printf("Save: %s\n", fileSkinRGB);
}

void SkinColorDetection::SearchSkinByLogOpponentAndYIQ()
{
	char *fileSkinLogOpponentAndYIQ = "skinLogOpponentAndYIQ.jpg";

	LogOpponent logOpponent(0, 0, 0);
	YIQ yiq(0, 0, 0);

	RGBQUAD color;
	RGBQUAD blackColor;
	blackColor.rgbRed = 0;
	blackColor.rgbGreen = 0;
	blackColor.rgbBlue = 0;
	int notSkinPixel = 0;

	for (int x = 0; x <= widthOriginal; x++) {
		for (int y = 0; y <= heightOriginal; y++) {
			FreeImage_GetPixelColor(ImageLogOpponentAndYIQ, x, y, &color);

			int r, g, b;
			r = color.rgbRed;
			g = color.rgbGreen;
			b = color.rgbBlue;

			logOpponent.setRed(r);
			logOpponent.setGrean(g);
			logOpponent.setBlue(b);

			yiq.setRed(r);
			yiq.setGrean(g);
			yiq.setBlue(b);

			double H = logOpponent.getH();
			double I = yiq.getI();

		//	if (!(100 <= H && H <= 150) && !(20 <= I && I <= 90)) {
			if (!(65 <= H && H <= 177) && !(9 <= I && I <= 131)) {
				FreeImage_SetPixelColor(ImageLogOpponentAndYIQ, x, y, &blackColor);
				notSkinPixel++;
			}
		}
	}

	FreeImage_Save(FreeImage_GetFIFFromFilename(fileSkinLogOpponentAndYIQ), ImageLogOpponentAndYIQ, fileSkinLogOpponentAndYIQ, 0);
	printf("Save: %s\n", fileSkinLogOpponentAndYIQ);
	//	FreeImage_Unload(original_image);
}

void SkinColorDetection::SearchSkinByHSV()
{
	char *fileSkinHSV = "skinHSV.jpg";

	HSV hsv(0, 0, 0);

	RGBQUAD color;
	RGBQUAD blackColor;
	blackColor.rgbRed = 0;
	blackColor.rgbGreen = 0;
	blackColor.rgbBlue = 0;
	int notSkinPixel = 0;

	for (int x = 0; x <= widthOriginal; x++) {
		for (int y = 0; y <= heightOriginal; y++) {
			FreeImage_GetPixelColor(ImageHSV, x, y, &color);

			int r, g, b;
			r = color.rgbRed;
			g = color.rgbGreen;
			b = color.rgbBlue;

			hsv.setRed(r);
			hsv.setGrean(g);
			hsv.setBlue(b);

			double H = hsv.getH();
			double S = hsv.getS();
			double V = hsv.getV();


			//if (!(0.35 < V && V < 1) && !(0.20 < S && S < 0.68) && !(0 < H && H < 50)) {
			if (!(60 < V && V < 250) && !(0.027 < S && S < 0.83) && !(0 < H && H < 1)) {
				FreeImage_SetPixelColor(ImageHSV, x, y, &blackColor);
				notSkinPixel++;
			} 
			else {
				notSkinPixel++;
			}
		}
	}

	FreeImage_Save(FreeImage_GetFIFFromFilename(fileSkinHSV), ImageHSV, fileSkinHSV, 0);
	printf("Save: %s\n", fileSkinHSV);
	//	FreeImage_Unload(original_image);
}

void SkinColorDetection::SearchSkinByTSLAndHSV()
{
	char *fileSkinTSLAndHSV = "skinTSLAndHSV.jpg";

	TSL tsl(0, 0, 0);
	HSV hsv(0, 0, 0);

	RGBQUAD color;
	RGBQUAD blackColor;
	blackColor.rgbRed = 0;
	blackColor.rgbGreen = 0;
	blackColor.rgbBlue = 0;
	int notSkinPixel = 0;

	for (int x = 0; x <= widthOriginal; x++) {
		for (int y = 0; y <= heightOriginal; y++) {
			FreeImage_GetPixelColor(ImageTSLAndHSV, x, y, &color);

			int r, g, b;
			r = color.rgbRed;
			g = color.rgbGreen;
			b = color.rgbBlue;


			tsl.setRed(r);
			tsl.setGrean(g);
			tsl.setBlue(b);

			hsv.setRed(r);
			hsv.setGrean(g);
			hsv.setBlue(b);

			double T = tsl.getT();
			double S = tsl.getS();
			double H = hsv.getH();

			//if (!(T < 0.6) && !(0.23 < S && S < 0.68) && !(0 < H && H < 50)) {
			if (!(-0.21 < S && T < 0.23) && !(0.025 < S && S < 0.63) && !(0 < H && H < 1)) {
				FreeImage_SetPixelColor(ImageTSLAndHSV, x, y, &blackColor);
				notSkinPixel++;
			}
		}
	}

	FreeImage_Save(FreeImage_GetFIFFromFilename(fileSkinTSLAndHSV), ImageTSLAndHSV, fileSkinTSLAndHSV, 0);
	printf("Save: %s\n", fileSkinTSLAndHSV);
	//	FreeImage_Unload(original_image);
}
