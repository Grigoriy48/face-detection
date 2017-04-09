#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef __OPENCV_OLD_HIGHGUI_H__
#define __OPENCV_OLD_HIGHGUI_H__

#endif
#include "TreatmentOnCPU.h"

int main()
{
	printf("=============================================================\n");
	printf("===================== FACE DETECTION ========================\n");
	printf("=============================================================\n\n\n");

	TreatmentOnCPU *TOC = new TreatmentOnCPU();
	TOC->Treatment();
}
