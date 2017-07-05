#include "header.h"
#include "threshold.h"

int GetPupilThresh(IplImage* smooth_img, int frame_cnt, int* thresh_sum, bool* blink_flag)
{
	int pupil_thresh;
	int width = smooth_img->width;
	int height = smooth_img->height;
	IplImage* horz_hist_img = NULL; // histogram �̹���(���� �и� �Ӱ谪�� ���ϱ� ���� ������׷�)

	int img_hist[256] = {0,};

	for(int col = 0; col < height; col++)
		for(int row = 0; row < width; row++)
			img_hist[(unsigned char)smooth_img->imageData[col * smooth_img->widthStep + row]]++;

	//printf("[%d %d] ", smooth_img->widthStep, ROI_img->widthStep);

	// ������׷� ������
	int win_size = 15; // ������ ������ ������
	if(win_size % 2 == 0) // ������ ������� Ȧ��
		win_size += 1;

	int win_half_size = win_size / 2;
	int win_center = ceil((double)win_size / 2.0);
	int smooth_img_hist[256] = {0,};

	// ���� ������׷� ������
	for(int i = win_half_size; i < 256 - win_size; i++)
	{
		float mean = 0;

		for(int j = i - win_half_size; j < i ; j++)
		{
			mean += (float)img_hist[j];
		}

		for(int j = i + 1 ; j < i + win_half_size ; j++)
		{
			mean += (float)img_hist[j];
		}
		mean += (float)img_hist[i];

		smooth_img_hist[i] = (int) (mean / win_size);

	}

	// ������׷� �ִ밪 ã��
	int img_hist_max_val = 0;
	int max_loc = 0;
	for(int i = 0; i < 256; i++)
	{
		if(img_hist_max_val < smooth_img_hist[i])
		{
			img_hist_max_val = smooth_img_hist[i];
			max_loc = i;
		}
	}

	int tmp_smooth_img_hist[256] = {0,};
	int end_of_pupil_range = (max_loc < 256 / 2) ? max_loc : 256/2;

	for(int i = 0; i < end_of_pupil_range; i++)
	{
		if(smooth_img_hist[i] < img_hist_max_val / 2)
			tmp_smooth_img_hist[i] = smooth_img_hist[i];
	}

	// ù��° ��ũ ã��
	int pupil_win_size = 11;
	int pupil_win_half_size = pupil_win_size / 2;
	int left_sum = 0;
	int right_sum = 0;
	int left_avg = 0;
	int right_avg = 0;
	int first_peak = 0;
	int first_peak_loc = 0;
	int tmp_peak = 0;



	for(int i = pupil_win_half_size ; i < end_of_pupil_range - pupil_win_half_size; i++)
	{
		left_sum = 0;
		right_sum = 0;
		first_peak = 0;
		left_avg = 0;
		right_avg = 0;
		first_peak_loc = 0;

		if(tmp_smooth_img_hist[i] != 0)
		{
			for(int j = i-1; j > i - pupil_win_half_size; j--)
				left_sum += tmp_smooth_img_hist[j];
			for(int j = i+1; j < i + pupil_win_half_size; j++)
				right_sum += tmp_smooth_img_hist[j];

			left_avg = left_sum / pupil_win_half_size;
			right_avg = right_sum / pupil_win_half_size;

			tmp_peak = tmp_smooth_img_hist[i];

			if(tmp_peak > left_avg && tmp_peak > right_avg)
			{
				if(first_peak < tmp_peak)
				{
					first_peak = tmp_peak;
					first_peak_loc = i;
					break;
				}
			}
		}
	}

	//printf("[%d  %d] ",first_peak_loc, end_of_pupil_range);

	int pupil_thres_val = first_peak;
	int pupil_thres = 0;

	for(int i = first_peak_loc; i < end_of_pupil_range; i++)
	{
		if(pupil_thres_val > tmp_smooth_img_hist[i] && tmp_smooth_img_hist[i] > 5)
		{
			pupil_thres_val = tmp_smooth_img_hist[i];
			pupil_thres = i;
		}
	}

	int tmp = *thresh_sum / frame_cnt;

	if(frame_cnt < 10)
	{
		(*thresh_sum) += pupil_thres;
	}
	else
	{
		int pupil_total = 0;
		for(int i = 0; i < tmp; i++)
			pupil_total += tmp_smooth_img_hist[i];


		if(pupil_total < 100)
		{
			*blink_flag = true; // ���� �����̰ų� ���� ����
			(*thresh_sum) += pupil_thres;
		}
		else
		{
			*blink_flag = false; // ���� �߰� �ִ� ����
			if(tmp_smooth_img_hist[tmp] > 10) 
			{
				if( (tmp - pupil_thres) < 10)
					(*thresh_sum) += pupil_thres;
				else // ��ȭ������ ���� ������������ ���� threshold�� ���� ����   
					(*thresh_sum) += tmp;

			}
			else // �����ӿ� ���� ���� ó�� ����
			{
				(*thresh_sum) += tmp;
			}
		}
	}

	pupil_thresh = (*thresh_sum) / frame_cnt;

	//printf("%d \n", pupil_thres);


	//// �̹��� ��ü ������׷� �����ֱ�
	//int hist_max_height = 500;

	//horz_hist_img = cvCreateImage(cvSize(256, hist_max_height),8,1);
	//cvZero(horz_hist_img);

	//for(int i = 0; i < 256; i++)
	//{
	//	int tmp = smooth_img_hist[i] * hist_max_height / img_hist_max_val;
	//	cvLine(horz_hist_img,cvPoint(i,horz_hist_img->height), cvPoint(i,horz_hist_img->height - tmp),cvScalar(255,255,255),1);
	//}
	//cvShowImage("���� ��ü ������׷�",horz_hist_img);
	//cvReleaseImage(&horz_hist_img);

	return pupil_thresh;
}