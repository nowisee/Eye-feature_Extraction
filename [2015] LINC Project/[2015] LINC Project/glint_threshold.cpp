#include "header.h"
#include "video.h"

int GetGlintThresh(IplImage* img)
{
		int width = img->width;
		int height = img->height;

		int img_hist_max_val = 0; // �̹��� ������׷� �ִ밪  
		int max_loc = 0;// �̹��� ������׷� �ִ밪�� �ε���(����)

		int img_hist[256] = {0,}; // �̹��� ������׷� ����
		
		int win_size = 15; // ������ ������ ������
		int win_half_size = win_size / 2;
		//int win_center = ceil((double)win_size / 2.0);
		int smooth_img_hist[256] = {0,}; // ������ ������׷� ����


		for(int col = 0; col < height; col++)
			for(int row = 0; row < width; row++)
				img_hist[(unsigned char)img->imageData[col * img->widthStep + row]]++;


		// ������׷� ������
		if(win_size % 2 == 0) // ������ ������� Ȧ��
			win_size += 1;


		// ���� ������׷� ������
		for(int i = win_half_size; i < 256 - win_size; i++)
		{
			float mean = 0;
			for(int j = i - win_half_size; j < i ; j++)		
				mean += (float)img_hist[j];
			
			for(int j = i + 1 ; j < i + win_half_size ; j++)		
				mean += (float)img_hist[j];
			
			mean += (float)img_hist[i];
			smooth_img_hist[i] = (int) (mean / win_size);

		}

		// ������׷� �ִ밪 ã��
		for(int i = 0; i < 256; i++)
		{
			if(img_hist_max_val < smooth_img_hist[i])
			{
				img_hist_max_val = smooth_img_hist[i];
				max_loc = i;
			}
		}

		// �۸�Ʈ ã��(������׷��� �ִ밪���κ��� ���� 0�� �Ǵ� ����)
		int glint_thres = 0;
		for(int i = max_loc; i < 256; i++)
		{
			if(smooth_img_hist[i] == 0)
			{
				glint_thres = i;
				break;
			}
		}


		return glint_thres;

}
