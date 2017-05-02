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
		SearchSkinByTSL();
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
			FreeImage_GetPixelColor(originalImage, x, y, &color);

			int r, g, b;
			r = color.rgbRed;
			g = color.rgbGreen;
			b = color.rgbBlue;

			//			r *= factor_for_skin;
			//			g *= factor_for_skin;
			//			b *= factor_for_skin;

			if (!(r > 90 && g > 40 && b > 20 && r - g > 15 && r > b)) {
				FreeImage_SetPixelColor(originalImage, x, y, &blackColor);
				notSkinPixel++;
			}
		}
	}

	FreeImage_Save(FreeImage_GetFIFFromFilename(fileSkinRGB), originalImage, fileSkinRGB, 0);
	printf("Save: %s\n", fileSkinRGB);
//	printf("Save: %s\n", notSkinPixel);
	
}

void SkinColorDetection::SearchSkinByLogOpponentAndYIQ()
{
	LogOpponent logOpponent(0, 0, 0);
	YIQ yiq(0, 0, 0);
}

void SkinColorDetection::SearchSkinByHSV()
{
	HSV hsv(0, 0, 0);
}

void SkinColorDetection::SearchSkinByTSL()
{
	TSL tsl(0, 0, 0);
}
