#include "header.h"
#include "video.h"

int GetGlintThresh(IplImage* img)
{
		int width = img->width;
		int height = img->height;

		int img_hist_max_val = 0; // 이미지 히스토그램 최대값  
		int max_loc = 0;// 이미지 히스토그램 최대값의 인덱스(명도값)

		int img_hist[256] = {0,}; // 이미지 히스토그램 저장
		
		int win_size = 15; // 스무딩 윈도우 사이즈
		int win_half_size = win_size / 2;
		//int win_center = ceil((double)win_size / 2.0);
		int smooth_img_hist[256] = {0,}; // 스무딩 히스토그램 저장


		for(int col = 0; col < height; col++)
			for(int row = 0; row < width; row++)
				img_hist[(unsigned char)img->imageData[col * img->widthStep + row]]++;


		// 히스토그램 스무딩
		if(win_size % 2 == 0) // 윈도우 사이즈는 홀수
			win_size += 1;


		// 수평 히스토그램 스무딩
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

		// 히스토그램 최대값 찾기
		for(int i = 0; i < 256; i++)
		{
			if(img_hist_max_val < smooth_img_hist[i])
			{
				img_hist_max_val = smooth_img_hist[i];
				max_loc = i;
			}
		}

		// 글린트 찾기(히스토그램의 최대값으로부터 최초 0이 되는 지점)
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
