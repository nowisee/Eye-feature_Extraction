#include "contour.h"

CvPoint GetFinalPupilCenter(CvPoint* left_contour, CvPoint* right_contour, CvPoint* f_left_contour, CvPoint* f_right_contour, int l_cnt, int r_cnt, int* left_cnt, int* right_cnt, CvBox2D* f_box, CvSize* f_size, bool* b_RANSAC, bool* gb_find)
{
	int left_num=0, right_num=0;
	int left_fit_size, right_fit_size;
	CvSeq* contour;
	CvSeq* ori_contour;
	CvMemStorage* memorie;
	CvMemStorage* ori_memorie;
	CvBox2D box;
	CvPoint center;
	CvPoint* contour_p;
	CvSize size;
	bool b_ransac = false;
	bool b_find = false;

	CvBox2D max_box;
	CvPoint max_center;
	CvSize max_size;

	max_box.angle = 0;
	max_box.center.x = 0;
	max_box.center.y = 0;
	max_box.size.height = 0;
	max_box.size.width = 0;
	max_size.height = 0;
	max_size.width = 0;
	max_center.x = 0;
	max_center.y = 0;

	memorie = cvCreateMemStorage(0);
	ori_memorie = cvCreateMemStorage(0);
	contour = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint) , memorie);
	ori_contour = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint) , ori_memorie);

	for(int i=0; i<l_cnt; i++)
	{
		if(left_contour[i].x != 0 && left_contour[i].y != 0)
		{
			f_left_contour[left_num].x = left_contour[i].x;
			f_left_contour[left_num].y = left_contour[i].y;
			cvSeqPush(ori_contour, &f_left_contour[left_num]);
			left_num++;
		}
	}

	for(int i=0; i<r_cnt; i++)
	{
		if(right_contour[i].x != 0 && right_contour[i].y != 0)
		{
			f_right_contour[right_num].x = right_contour[i].x;
			f_right_contour[right_num].y = right_contour[i].y;
			cvSeqPush(ori_contour, &f_right_contour[right_num]);
			right_num++;
		}
	}

	left_fit_size = cvRound(0.8 * left_num);
	right_fit_size = cvRound(0.8 * right_num);

	if(left_num > FIT_SIZE/2 && right_num > FIT_SIZE/2)
		b_find = true;

	*gb_find = b_find;

	if(ori_contour->total >= 10 && b_find)
	{
		int tmp = 0;
		int max = 0;
		// RANSAC
		while(!b_ransac)
		{
			cvClearSeq(contour);
			/*for(int i=0; i<FIT_SIZE; i++)
			{
				cvSeqPush(contour, &f_left_contour[rand() % left_num]);
				cvSeqPush(contour, &f_right_contour[rand() % right_num]);
			}*/
			for(int i=0; i<left_fit_size; i++)
				cvSeqPush(contour, &f_left_contour[rand() % left_num]);
			for(int i=0; i<right_fit_size; i++)
				cvSeqPush(contour, &f_right_contour[rand() % right_num]);

			box = cvFitEllipse2(contour);
			center = cvPointFrom32f(box.center);
			size.width = cvRound(box.size.width * 0.5);
			size.height = cvRound(box.size.height * 0.5);

			if(center.x <= 0 || center.y <= 0)
				break;

			IplImage* tmp_3d_img = cvCreateImage(cvSize(CROP_WIDTH, CROP_HEIGHT), 8, 3);// tmp ÀÌ¹ÌÁö

			for(int i=0; i<tmp_3d_img->height; i++)
			{
				uchar* ptr = (uchar*)(tmp_3d_img->imageData + i*tmp_3d_img->widthStep);

				for(int j=0; j<tmp_3d_img->width; j++)
				{
					ptr[3*j + 0] = 0;
					ptr[3*j + 1] = 0;
					ptr[3*j + 2] = 0;
				}
			}

			cvEllipse(tmp_3d_img, center, size, box.angle, 0, 360, CV_RGB(255, 255, 255), 2, CV_AA, 0);

			//cvShowImage("Ellipse Image", tmp_3d_img);

			int cnt = 0;
			CvPoint* contour_p;
			for(int i=0; i<left_fit_size+right_fit_size; i++)
			{
				contour_p = CV_GET_SEQ_ELEM(CvPoint, contour, i);
				//printf("[%d %d] ", contour_p->x, contour_p->y);
				if((unsigned char)tmp_3d_img->imageData[contour_p->y * tmp_3d_img->widthStep + 3*contour_p->x] == 255)
					cnt++;
			}

			cvReleaseImage(&tmp_3d_img);

			if(cnt >= 0.8 * (left_fit_size + right_fit_size))
				b_ransac = true;
			else
			{
				if(cnt > max)
				{
					max = cnt;
					max_box = box;
					max_size = size;
					max_center = center;
				}
				tmp++;
			}
			
			if(tmp>50)
				break;
		}
	}
	cvClearSeq(ori_contour);
	cvClearSeq(contour);
	cvReleaseMemStorage(&memorie);
	cvReleaseMemStorage(&ori_memorie);

	*b_RANSAC = b_ransac;

	*left_cnt = left_num;
	*right_cnt = right_num;

	if(b_ransac)
	{
		f_box->angle = box.angle;
		f_box->center = box.center;
		f_box->size = box.size;
		f_size->height = size.height;
		f_size->width = size.width;
	}
	else
	{
		center.x = 0;
		center.y = 0;
	}

	return center;
}