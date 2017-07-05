#include "contour.h"

void GrayToRGB(IplImage* src, IplImage* dst)
{
	for(int i=0; i<src->height; i++)
	{
		for(int j=0; j<src->widthStep; j++)
		{
			dst->imageData[i*dst->widthStep + (3*j+0)] = (unsigned char)src->imageData[i*src->widthStep + j];
			dst->imageData[i*dst->widthStep + (3*j+1)] = (unsigned char)src->imageData[i*src->widthStep + j];
			dst->imageData[i*dst->widthStep + (3*j+2)] = (unsigned char)src->imageData[i*src->widthStep + j];
		}
	}
}

void SetRange(int* upper_range, int* lower_range, int tmp_pupil_diameter)
{
	*upper_range = abs(tmp_pupil_diameter/2 * sin(THETA1));
	*lower_range = abs(tmp_pupil_diameter/2 * sin(THETA2));
}