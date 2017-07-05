#include "header.h"
#include "video.h"

IplImage* ROI(IplImage* img, CvRect roi)
{
	IplImage* ROI_img = cvCreateImage(cvSize(roi.width, roi.height), 8, 1);
	cvSetImageROI(img,roi);
	cvCopy(img,ROI_img,0);
	cvResetImageROI(img);


	return ROI_img;
}