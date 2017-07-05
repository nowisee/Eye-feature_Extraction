#include "contour.h"

void RealContour(IplImage *img, CvPoint t_center)
{
	// 동공 왼쪽과 오른쪽 시작점 찾기
   int p_l_pt = 0; // 왼쪽
   int p_r_pt = 0; // 오른쪽
   int p_u_pt = 0; // 위쪽
   int p_b_pt = 0; // 아래쪽
   // 왼쪽 시작점
   int s_flag = false;
   for(int row = 0; row < img->width/2; row++) // 동공 너비 1/2 지점까지
   {
      if(s_flag == true)
         break;
      for(int col = img->height / 3; col < img->height; col++)  // 동공 높이 1/3 지점부터
      {
         int val = (unsigned char)img->imageData[col * img->widthStep + row];
         if(val == 255)
         {
            p_l_pt = row;
            s_flag = true;
            break;
         }
      }
   }

   // 오른쪽 시작점
   int e_flag = false;
   for(int row = img->width; row > img->width/2; row--) // 동공 너비 1/2 지점까지
   {
      if(e_flag == true)
         break;

      for(int col = img->height / 3; col < img->height; col++)  // 동공 높이 1/3 지점부터
      {
         int val = (unsigned char)img->imageData[col * img->widthStep + row];
         if(val == 255)
         {
            p_r_pt = row;
            e_flag = true;
            break;
         }
      }
   }

   // 위쪽 시작점
   int u_flag = false;
   for(int col = 0; col < img->height/2; col++) 
   {
      if(u_flag == true)
         break;
         for(int row = 0; row < img->width; row++) // 동공 너비 1/2 지점까지
      {
         int val = (unsigned char)img->imageData[col * img->widthStep + row];
         if(val == 255)
         {
            p_u_pt = col;
            u_flag = true;
            break;
         }
      }
   }


   // 아래쪽 시작점
   int b_flag = false;
   for(int col = img->height; col > img->height/2; col--) 
   {
      if(b_flag == true)
         break;
         for(int row = 0; row < img->width; row++) 
      {
         int val = (unsigned char)img->imageData[col * img->widthStep + row];
         if(val == 255)
         {
            p_b_pt = col;
            b_flag = true;
            break;
         }
      }
   }
}

void TempContourDetection(IplImage* smooth_img, IplImage* contour_img, CvPoint t_center, CvPoint* left_contour, CvPoint* right_contour, int upper_range, int lower_range, int* left_cnt, int* right_cnt, CvRect rough_pupil)
{
	CvPoint tmp_contour[MAX_VERTEX];
	CvMemStorage* storage = cvCreateMemStorage(0); // 배열 자료형 : 점의 좌표가 들어감 
	CvSeq* seq = NULL; //경계 개수를 저장할 변수 선언

	CvPoint* contour_p = NULL; 
	int l_cnt=0, r_cnt=0;
	CvSeq* max;

	// ★ 동공 경계를 저장하기 위한 공간들(조건을 통해 생성 또는 비움으로써 메모리 누수 방지)
	if(seq == NULL)
		seq = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint) , storage);
	else
		cvClearSeq(seq);

	int numContour = cvFindContours(contour_img, storage, &seq, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	max = seq;

	//printf("%d\n",numContour);

	CvScalar inColor = CV_RGB(0, 0, 0);
	CvScalar outColor = CV_RGB(255, 255, 255);

	int mask_size = 11;
	int mask_shift = 10;

	for( CvSeq* c = seq; c != NULL; c = c->h_next)
	{
		if(c->total > max->total)
			max = c;
	}
	
	//printf("t_1 ");
	//printf("[%d] ", seq->total);
	if(max != NULL && max->total > 50)
	{
		for(int i = 0; i < max->total; i++)
		{				
			//printf("t_2 ");
			contour_p = CV_GET_SEQ_ELEM(CvPoint, max, i);
			CvPoint p;

			contour_p->x += rough_pupil.x;
			contour_p->y += rough_pupil.y;

			p.x = contour_p->x;
			p.y = contour_p->y;

			if(p.y < t_center.y - upper_range /*|| p.y > t_center.y + lower_range*/)
			{
				contour_p->x = 0;
				contour_p->y = 0;
			}
			else
			{
				//printf("[%d %d] ",p.x, p.y);

				//printf("t_3 ");
				int gradient_max = 0;
				int gradient_max_loc = 0;

				for(int shift = p.x - mask_shift; shift < p.x + mask_shift; shift++)
				{
					bool b_glint = false;
					//printf("t_4 ");
					if(shift - mask_shift - mask_size > 0 && shift + mask_shift + mask_size < smooth_img->width )
					{
						int left_sum = 0;
						int right_sum = 0;

						//printf("t_5 ");
						for(int gr_shift = shift - mask_size; gr_shift < shift; gr_shift++)
						{
							if((unsigned char)smooth_img->imageData[p.y * smooth_img->widthStep + gr_shift] < 180)
							{
								left_sum += (unsigned char)smooth_img->imageData[p.y * smooth_img->widthStep + gr_shift];
							}
							else
							{
								/*if(p.x < t_center.x)
									gradient_max_loc = p.x-2;
								else
									gradient_max_loc = p.x+2;*/
								contour_p->x = 0;
								contour_p->y = 0;
								b_glint = true;
								break;
							}
						}
						if(b_glint) 
							break;

						for(int gr_shift = shift + 1; gr_shift < shift + mask_size; gr_shift++)
						{							
							if((unsigned char)smooth_img->imageData[p.y * smooth_img->widthStep + gr_shift] < 180)
							{
								right_sum += (unsigned char)smooth_img->imageData[p.y * smooth_img->widthStep + gr_shift];
							}
							else
							{
								/*if(p.x < t_center.x)
									gradient_max_loc = p.x-2;
								else
									gradient_max_loc = p.x+2;*/
								contour_p->x = 0;
								contour_p->y = 0;
								b_glint = true;
								break;
							}
						}
						if(b_glint) 
							break;

						//printf("t_6 ");
						//printf("[%d %d] ",left_sum,right_sum);
						int tmp_gradient = (int)((abs)((double)left_sum - (double)right_sum));
						//printf("[%d ] ",tmp_gradient);

						if(gradient_max < tmp_gradient)
						{
							gradient_max = tmp_gradient;
							gradient_max_loc = shift;
						}

					}
					else
						contour_p->x = p.x;		
				}
				//printf("%d ",p.y);
				//if(gradient_max_loc <= smooth_img->width)
				//	smooth_img->imageData[p.y * smooth_img->widthStep + gradient_max_loc] = 255;
				contour_p->x = gradient_max_loc;
				//smooth_img->imageData[contour_p->y * smooth_img->widthStep + contour_p->x] = 255; 
			}
			
			if(contour_p->x != 0)
			{
				if(contour_p->x < t_center.x)
				{
					left_contour[l_cnt].x = contour_p->x;
					left_contour[l_cnt].y = contour_p->y;
					l_cnt++;
				}
				else if(contour_p->x > t_center.x)
				{
					tmp_contour[r_cnt].x = contour_p->x;
					tmp_contour[r_cnt].y = contour_p->y;
					r_cnt++;
				}
			}
		}

		//printf("%d %d\n", l_cnt, r_cnt);
	}
	else
	{
		//Delete Contour
	}


	for(int i=0; i<r_cnt; i++)
		right_contour[i] = tmp_contour[r_cnt-1 - i];

	*left_cnt = l_cnt;
	*right_cnt = r_cnt;

	cvReleaseMemStorage(&storage);
}