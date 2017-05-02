#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef __OPENCV_OLD_HIGHGUI_H__
#define __OPENCV_OLD_HIGHGUI_H__

#endif
#include "TreatmentOnCPU.h"
#include "SkinColorDetection.h"

int main()
{
	printf("=============================================================\n");
	printf("===================== FACE DETECTION ========================\n");
	printf("=============================================================\n\n\n");

	

	printf("=============================================================\n\n\n");
	printf("skin color detection method:\n");
	printf("1 - RGB\n");
	printf("2 - Log Opponent & YIQ\n");
	printf("3 - HSV\n");
	printf("4 - TSL\n");
	printf("5 - all methods\n");

	int method;
	scanf("%d", &method);
	SkinColorDetection skinColorDetection(method);

	
	//TreatmentOnCPU *TOC = new TreatmentOnCPU();
	//TOC->Treatment();
}
