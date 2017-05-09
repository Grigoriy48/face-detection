#include "SkinColorDetection.h"





SkinColorDetection::SkinColorDetection(int method)
{
	char *fileIn = "in.jpg";
	char *fileOut = "out.jpg";


	originalImage = FreeImage_Load(FIF_JPEG, fileIn, 0);
	widthOriginal = FreeImage_GetWidth(originalImage);
	heightOriginal = FreeImage_GetHeight(originalImage);

	ImageRGB = FreeImage_Clone(originalImage);
	ImageLogOpponentAndYIQ = FreeImage_Clone(originalImage);
	ImageHSV = FreeImage_Clone(originalImage);
	ImageTSLAndHSV = FreeImage_Clone(originalImage);
	FreeImage_Unload(originalImage);


	printf("filters:\n");
	printf("1 - file\n");
	printf("2 - source\n");
	int src;
	scanf("%d", &src);
	
	switch (src)
	{
	case 1:
		readFilters();
		break;
	case 2:
		logOpponentMinH = 100;
		logOpponenthsvMaxH = 150;
		yiqMinI = 20;
		yiqMaxI = 90;
		//hsvMinH = 0;
		//hsvMaxH = 50;
		//hsvMinS = 0.20;
		//hsvMaxS = 0.68;
		//hsvMinV = 0.35;
		//hsvMaxV = 1;
		
		hsvMinH = 0;
		hsvMaxH = 25;
		hsvMinS = 0.20;
		hsvMaxS = 0.75;
		hsvMinV = 0.35;
		hsvMaxV = 1;
		tslMinT = 0;
		tslMaxT = 0.6;
		tslMinS = 0.23;
		tslMaxS = 0.68;
		break;
	}
	//	if (!(100 <= H && H <= 150) && !(20 <= I && I <= 90)) {
	//if (!(0.35 < V && V < 1) && !( < S && S < ) && !(0 < H && H <)) {
	//if (!(T < 0.6) && !( < S && S < ) && !(0 < H && H < 50)) {

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

			if (!(r > 90 && g > 40 && b > 20 && r - g > 15 && r > b)) {
				FreeImage_SetPixelColor(ImageRGB, x, y, &blackColor);
				notSkinPixel++;
			}
		}
	}

	FreeImage_Save(FreeImage_GetFIFFromFilename(fileSkinRGB), ImageRGB, fileSkinRGB, 0);
	FreeImage_Unload(ImageRGB);
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
			
			
			if (!(logOpponentMinH < H & H < logOpponenthsvMaxH) & !(yiqMinI < I & I < yiqMaxI)) {
				FreeImage_SetPixelColor(ImageLogOpponentAndYIQ, x, y, &blackColor);
				notSkinPixel++;
			}
		}
	}

	FreeImage_Save(FreeImage_GetFIFFromFilename(fileSkinLogOpponentAndYIQ), ImageLogOpponentAndYIQ, fileSkinLogOpponentAndYIQ, 0);
	FreeImage_Unload(ImageLogOpponentAndYIQ);
	printf("Save: %s\n", fileSkinLogOpponentAndYIQ);

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

			//if ((S >= 0.20 && S <= 0.75) && V > 0.35 && H >= 0 &&H <= 25 && I <= 90 && I >= 20)
			//if (!(hsvMinV < V && V < hsvMaxV && hsvMinS < S && S < hsvMaxS && hsvMinH < H && H < hsvMaxH)) {
			if (!(hsvMinV < V && V < hsvMaxV) && !(hsvMinS < S && S < hsvMaxS) && !(hsvMinH < H && H < hsvMaxH)) {
				FreeImage_SetPixelColor(ImageHSV, x, y, &blackColor);
				notSkinPixel++;
			} 
		}
	}

	FreeImage_Save(FreeImage_GetFIFFromFilename(fileSkinHSV), ImageHSV, fileSkinHSV, 0);
	FreeImage_Unload(ImageHSV);
	printf("Save: %s\n", fileSkinHSV);
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


			if (!(tslMinT < T & T < tslMaxT) & !(tslMinS < S & S < tslMaxS) & !(hsvMinH < H & H < hsvMaxH)) {
				FreeImage_SetPixelColor(ImageTSLAndHSV, x, y, &blackColor);
				notSkinPixel++;
			}
		}
	}

	FreeImage_Save(FreeImage_GetFIFFromFilename(fileSkinTSLAndHSV), ImageTSLAndHSV, fileSkinTSLAndHSV, 0);
	FreeImage_Unload(ImageTSLAndHSV);
	printf("Save: %s\n", fileSkinTSLAndHSV);
}

void SkinColorDetection::readFilters()
{
	ifstream fout("Filters.txt");
	string str;
	
	double *arrayFilters = new double[13];

	int i = 0;
	while (getline(fout, str))
	{                        
		arrayFilters[i] = stod(str);
		i++;
	}

	fout.close();

	logOpponentMinH = arrayFilters[0];
	logOpponenthsvMaxH = arrayFilters[1];
	yiqMinI = arrayFilters[2];
	yiqMaxI	= arrayFilters[3];
	hsvMinH	= arrayFilters[4];
	hsvMaxH	= arrayFilters[5];
	hsvMinS	= arrayFilters[6];
	hsvMaxS	= arrayFilters[7];
	hsvMinV	= arrayFilters[8];
	hsvMaxV	= arrayFilters[9];
	tslMinT	= arrayFilters[10];
	tslMaxT	= arrayFilters[11];
	tslMinS	= arrayFilters[12];
	tslMaxS	= arrayFilters[13];

	cout << "===========LOG OPPONENT===========" << endl;
	cout << "logOpponentMinH is: " << logOpponentMinH << endl;
	cout << "logOpponenthsvMaxH is: " << logOpponenthsvMaxH << endl;

	cout << "===========YIQ===========" << endl;
	cout << "yiqMinI is: " << yiqMinI << endl;
	cout << "yiqMaxI is: " << yiqMaxI << endl;

	cout << "===========HSV===========" << endl;
	cout << "hsvMinH is: " << hsvMinH << endl;
	cout << "hsvMaxH is: " << hsvMaxH << endl;
	cout << "hsvMinS is: " << hsvMinS << endl;
	cout << "hsvMaxS is: " << hsvMaxS << endl;
	cout << "hsvMinV is: " << hsvMinV << endl;
	cout << "hsvMaxV is: " << hsvMaxV << endl;

	cout << "===========TSL===========" << endl;
	cout << "tslMinT is: " << tslMinT << endl;
	cout << "tslMaxT is: " << tslMaxT << endl;
	cout << "tslMinS is: " << tslMinS << endl;
	cout << "tslMaxS is: " << tslMaxS << endl;
}

