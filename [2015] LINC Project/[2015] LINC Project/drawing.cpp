#include "contour.h"

void DrawContour(IplImage* smooth_img, CvPoint* final_left_contour, CvPoint* final_right_contour, int left_num, int right_num)
{
	for(int i=0; i<left_num; i++)
		smooth_img->imageData[final_left_contour[i].y * smooth_img->widthStep + final_left_contour[i].x] = 255;

	for(int i=0; i<right_num; i++)
		smooth_img->imageData[final_right_contour[i].y * smooth_img->widthStep + final_right_contour[i].x] = 255;
}

void DrawPupilInfo(IplImage* result_img, CvPoint final_pupil_center, CvBox2D box, CvSize size)
{
	cvEllipse(result_img, final_pupil_center, size, box.angle, 0, 360, CV_RGB(0, 255, 0), 2, CV_AA, 0);
	cvLine(result_img, cvPoint(final_pupil_center.x - 1,final_pupil_center.y),cvPoint(final_pupil_center.x + 1,final_pupil_center.y),CV_RGB(255,0,0),2);
	cvLine(result_img, cvPoint(final_pupil_center.x,final_pupil_center.y - 1),cvPoint(final_pupil_center.x,final_pupil_center.y + 1),CV_RGB(255,0,0),2);
}

void DrawPupilCenterLine(IplImage* gray_img, CvPoint center)
{
	for(int i=0; i<gray_img->height; i++)
	{
		if(i == center.y)
		{
			for(int j=0; j<gray_img->widthStep; j++)
				gray_img->imageData[i*gray_img->widthStep + j] = 255;
		}
		gray_img->imageData[i*gray_img->widthStep + center.x] = 255;
	}
}