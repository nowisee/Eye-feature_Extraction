#include "contour.h"

void FilteringContour(IplImage* smooth_img, CvPoint* left_contour, CvPoint* right_contour, int l_cnt, int r_cnt, CvPoint t_center)
{
	int first_idx = 0, last_idx = 0;

	int l_mididx=0, r_mididx=0;
	int min_x, max_x, min_x_idx, max_x_idx;
	int left_y_val[CROP_HEIGHT] = {0}, right_y_val[CROP_HEIGHT] = {0};
	bool b_interfere;

	// �뷫���� ���� �߽��� y��ǥ���� ���� ���� �ε��� �޾ƿ���
	int tmp = 9999;
	for(int i=0; i<l_cnt; i++)
	{
		int diff = abs(left_contour[i].y - t_center.y);
		if(diff < tmp)
		{
			tmp = diff;
			l_mididx = i;
		}
	}

	min_x = left_contour[l_mididx].x;
	min_x_idx = l_mididx;
	for(int i=-2; i<=2; i++)
	{
		if(left_contour[l_mididx+i].x < min_x)
		{
			min_x = left_contour[l_mididx+i].x;
			min_x_idx = l_mididx + i;
		}
	}
	l_mididx = min_x_idx;


	tmp = 9999;
	for(int i=0; i<r_cnt; i++)
	{
		int diff = abs(right_contour[i].y - t_center.y);
		if(diff < tmp)
		{
			tmp = diff;
			r_mididx = i;
		}
	}

	max_x = right_contour[r_mididx].x;
	max_x_idx = r_mididx;
	for(int i=-2; i<=2; i++)
	{
		if(right_contour[r_mididx+i].x > max_x)
		{
			max_x = right_contour[r_mididx+i].x;
			max_x_idx = r_mididx + i;
		}
	}
	r_mididx = max_x_idx;

	/*for(int i=0; i<smooth_img->height; i++)
	{
		if(i == left_contour[l_mididx].y)
		{
			for(int j=-5; j<=5; j++)
				smooth_img->imageData[i*smooth_img->widthStep + left_contour[l_mididx].x + j] = 255;
		}
		if(i == right_contour[r_mididx].y)
		{
			for(int j=-5; j<=5; j++)
				smooth_img->imageData[i*smooth_img->widthStep + right_contour[r_mididx].x + j] = 255;
		}
	}*/

	cvShowImage("Smoothing ������(���� ��)",smooth_img);

	// Left Contour Filtering
	// (1) �۸�Ʈ ���� �޴� ���� ����
	for(int i=0; i<l_cnt-1; i++)
	{
		if(i < l_mididx)
		{
			if(left_contour[i+1].x - left_contour[i].x > 0)
			{
				first_idx = i;
				break;
			}
		}
		else// if(i > l_mididx+1)
		{
			if(left_contour[i+1].x - left_contour[i].x < 0)
			{
				first_idx = i;
				break;
			}
		}
	}

	for(int i=l_cnt-1; i>0; i--)
	{
		if(i < l_mididx)
		{
			if(left_contour[i-1].x - left_contour[i].x < 0)
			{
				last_idx = i;
				break;
			}
		}
		else// if(i > l_mididx)
		{
			if(left_contour[i-1].x - left_contour[i].x > 0)
			{
				last_idx = i;
				break;
			}
		}
	}

	if(first_idx <= last_idx)
	{
		for(int i=first_idx; i<=last_idx; i++)
		{
			left_contour[i].x = 0;
			left_contour[i].y = 0;
		}
	}
	else
	{
		for(int i=last_idx; i<=first_idx; i++)
		{
			left_contour[i].x = 0;
			left_contour[i].y = 0;
		}
	}

	//// (2) �Ӵ����� ������ �޴� ���� ����
	//min_x = 9999;
	//min_x_idx = 0;
	//for(int i=0; i<l_mididx - 1; i++)
	//{
	//	if(left_contour[i].x != 0)
	//	{
	//		if(left_contour[i].x < min_x)
	//		{
	//			min_x = left_contour[i].x;
	//			min_x_idx = i;
	//		}
	//	}
	//}


	//// �����ִ� ��輱���� y��ǥ�� ����
	//for(int i=0; i<l_cnt; i++)
	//	left_y_val[left_contour[i].y]++;

	//// (3) �۸�Ʈ�� ����Ǯ�� ������ �޾� �ϱ׷��� ��輱 ����
	//// ���� ��ܺ�
	//min_x = 9999;
	//min_x_idx = 0;
	//b_interfere = false;
	//for(int i=0; i<l_mididx-1; i++)
	//{
	//	if(left_contour[i].y != 0)
	//	{
	//		if(left_y_val[left_contour[i].y] > 2)
	//		{
	//			if(left_contour[i].x < min_x)
	//			{
	//				min_x = left_contour[i].x;
	//				min_x_idx = i;
	//			}
	//			b_interfere = true;
	//		}
	//	}
	//}

	//if(b_interfere)
	//{
	//	for(int i=0; i<min_x_idx; i++)
	//	{
	//		left_contour[i].x = 0;
	//		left_contour[i].y = 0;
	//	}
	//}

	//// ���� �ϴܺ�
	//min_x = 9999;
	//min_x_idx = 9999;
	//b_interfere = false;
	//for(int i=l_cnt-1; i>l_mididx+1; i--)
	//{
	//	if(left_contour[i].y != 0)
	//	{
	//		if(left_y_val[left_contour[i].y] > 2)
	//		{
	//			if(left_contour[i].x < min_x)
	//			{
	//				min_x = left_contour[i].x;
	//				min_x_idx = i;
	//			}
	//			b_interfere = true;
	//		}
	//	}
	//}

	//if(b_interfere)
	//{
	//	for(int i=min_x_idx+1; i<l_cnt; i++)
	//	{
	//		left_contour[i].x = 0;
	//		left_contour[i].y = 0;
	//	}
	//}


	// Right Contour Filtering
	// (1) �۸�Ʈ ���� �޴� ���� ����
	first_idx = 0;
	last_idx = 0;
	for(int i=0; i<r_cnt-1; i++)
	{
		if(i < r_mididx)
		{
			if(right_contour[i+1].x - right_contour[i].x < 0)
			{
				first_idx = i;
				break;
			}
		}
		else// if(i > r_mididx)
		{
			if(right_contour[i+1].x - right_contour[i].x > 0)
			{
				first_idx = i;
				break;
			}
		}
	}

	for(int i=r_cnt-1; i>0; i--)
	{
		if(i < r_mididx)
		{
			if(right_contour[i-1].x - right_contour[i].x > 0)
			{
				last_idx = i;
				break;
			}
		}
		else// if(i > r_mididx)
		{
			if(right_contour[i-1].x - right_contour[i].x < 0)
			{
				last_idx = i;
				break;
			}
		}
	}

	if(first_idx != last_idx)
	{
		for(int i=first_idx; i<=last_idx; i++)
		{
			right_contour[i].x = 0;
			right_contour[i].y = 0;
		}
	}

	//// (2) �Ӵ����� ���� �޴� ���� ����
	//max_x = 0;
	//max_x_idx = 0;
	//for(int i=0; i<r_mididx - 1; i++)
	//{
	//	if(right_contour[i].x != 0)
	//	{
	//		if(right_contour[i].x > max_x)
	//		{
	//			max_x = left_contour[i].x;
	//			max_x_idx = i;
	//		}
	//	}
	//}

	//// �����ִ� ��輱���� y��ǥ�� ����
	//for(int i=0; i<r_cnt; i++)
	//	right_y_val[right_contour[i].y]++;

	//// (3) �۸�Ʈ�� ����Ǯ�� ������ �޾� �ϱ׷��� ��輱 ����
	//// ���� ��ܺ�
	//max_x = 0;
	//max_x_idx = 0;
	//b_interfere = false;
	//for(int i=0; i<r_mididx-1; i++)
	//{
	//	if(right_contour[i].y != 0)
	//	{
	//		if(right_y_val[right_contour[i].y] > 2)
	//		{
	//			if(right_contour[i].x > max_x)
	//			{
	//				max_x = right_contour[i].x;
	//				max_x_idx = i;
	//			}
	//			b_interfere = true;
	//		}
	//	}
	//}

	//if(b_interfere)
	//{
	//	for(int i=0; i<max_x_idx; i++)
	//	{
	//		right_contour[i].x = 0;
	//		right_contour[i].y = 0;
	//	}
	//}

	//// ���� �ϴܺ�
	//max_x = 0;
	//max_x_idx = 9999;
	//b_interfere = false;
	//for(int i=r_cnt-1; i>r_mididx+1; i--)
	//{
	//	if(right_contour[i].y != 0)
	//	{
	//		if(right_y_val[right_contour[i].y] > 2)
	//		{
	//			if(right_contour[i].x > max_x)
	//			{
	//				max_x = right_contour[i].x;
	//				max_x_idx = i;
	//			}
	//			b_interfere = true;
	//		}
	//	}
	//}

	//if(b_interfere)
	//{
	//	for(int i=min_x_idx+1; i<r_cnt; i++)
	//	{
	//		right_contour[i].x = 0;
	//		right_contour[i].y = 0;
	//	}
	//}
}