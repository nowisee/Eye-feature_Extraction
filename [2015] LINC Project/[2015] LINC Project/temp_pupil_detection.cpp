//#include "video.h"
//#include "header.h"
#include "labeling.h"

void RemoveGlint(IplImage* img)
{
	
	// ���� �۸�Ʈ ����
	// ���� �۸�Ʈ ������ ũ�� Ȯ���� ���� tmp_img ����
	/*
	IplImage * tmp_img = cvCreateImage(cvGetSize(img),8,1);
	cvCopy(img,tmp_img);
	cvDilate(tmp_img,tmp_img,0,1); // �۸�Ʈ ���� Ȯ��

	
	CBlobLabeling glint_blob;
	glint_blob.SetParam(tmp_img, 3); // ���̺� �� ����� �ּ� �ȼ� �� 
	glint_blob.DoLabeling();

	cvReleaseImage(&tmp_img);
	
	for(int i = 1; i < glint_blob.m_nBlobs; i ++) // 1���� �����ϴ� ������ ���� ū ����� ���� �ϱ� ����
	{
		CvPoint s_point = cvPoint(glint_blob.m_recBlobs[i].x,glint_blob.m_recBlobs[i].y);
		CvPoint e_point = cvPoint(glint_blob.m_recBlobs[i].x + glint_blob.m_recBlobs[i].width,glint_blob.m_recBlobs[i].y + glint_blob.m_recBlobs[i].height);
		cvSetImageROI(img,cvRect(glint_blob.m_recBlobs[i].x,glint_blob.m_recBlobs[i].y ,glint_blob.m_recBlobs[i].width ,glint_blob.m_recBlobs[i].height ));
		cvZero(img);
		cvResetImageROI(img);
	}
	
	cvNot(img,img); // ���� �۸�Ʈ ���� �� ����(������ �ٽ� ������� ����)
	*/

	// �������� �۸�Ʈ ä���
	for(int row = 0; row < img->width; row++)
	{
		for(int col = 0; col < img->height; col++)
		{
			int val = (unsigned char)img->imageData[col * img->widthStep + row];
			if(val != 0) // ó�� ����� ������ ������
			{
				int s_pt = 0;
				int e_pt = 0;
				for(int up_col = col; up_col < img->height; up_col++)
				{
					int up_val = (unsigned char)img->imageData[up_col * img->widthStep + row];
					if( up_val == 0) // ���� �������� �����ٸ�
					{
						s_pt = up_col - 1;
						
						for(int dw_col = up_col; dw_col < img->height; dw_col++)
						{
							int dw_val = (unsigned char)img->imageData[dw_col * img->widthStep + row];
							if(dw_val == 255) // ���� ����� �ٽ� �����ٸ�
								e_pt = dw_col;
						}
					}
					if(s_pt != 0 && e_pt != 0)
					{
						for(int fill_col = s_pt; fill_col <= e_pt; fill_col++)
							img->imageData[fill_col * img->widthStep + row] = 255;
					}
				}
			}
		}
	}

	// �������� �۸�Ʈ ä���
	for(int col = 0; col < img->height; col++)
	{
		int s_pt = 0;
		int e_pt = 0;
		for(int row = 0; row < img->width; row++)
		{
			int val = (unsigned char)img->imageData[col * img->widthStep + row];
			if(val == 255) // ����϶�
			{
				s_pt = row;
				break;
			}
		}

		for(int row = img->width; row > s_pt; row--)
		{
			int val = (unsigned char)img->imageData[col * img->widthStep + row];
			if(val == 255) // ����϶�
			{
				e_pt = row;
				break;
			}
		}

		if(s_pt != 0 && e_pt != 0)
		{
			for(int fill_row = s_pt; fill_row <= e_pt; fill_row++)
				img->imageData[col * img->widthStep + fill_row] = 255;
		}

	}

	/*
	// �ֿܰ� ���� ã��
	CvMemStorage* storage = cvCreateMemStorage(0); // �迭 �ڷ��� : ���� ��ǥ�� �� 
	CvSeq* seq = NULL; //��� ������ ������ ���� ����

	// �� ���� ��踦 �����ϱ� ���� ������(������ ���� ���� �Ǵ� ������ν� �޸� ���� ����)
	if(seq == NULL)
		seq = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint) , storage);
	else
		cvClearSeq(seq);

	cvFindContours(img, storage, &seq, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	
	
	for(int i = 0; i < seq->total; i++)
	{				
			//printf("t_2 ");
			CvPoint *contour_p = CV_GET_SEQ_ELEM(CvPoint, seq, i);
			printf("[%d %d] ",contour_p->x,contour_p->y);
	}
	printf("\n\n");

	IplImage*test = cvCreateImage(cvGetSize(img), 8,1);
	cvZero(test);

	cvDrawContours(test,seq,cvScalar(255,255,255),cvScalar(0,0,0),0,1);

	cvShowImage("contours",test);

	cvReleaseImage(&test);
	
	cvReleaseMemStorage(&storage);
	*/



}



CvRect RoughPupilRegion(IplImage* img)
{

	
	// �뷫���� ���� ������ ��ǥ����
	CvRect rough_region = cvRect(0,0,0,0);
	//CvRect prev_rough_region = cvRect(0,0,0,0);

	int max_y = 0;
	int max_y_idx = 0; // y ���� ���� ������ ���� �� x ��ǥ
	for(int row = 0; row < img->width; row++)
	{
		int tmp = 0;

		for(int col = 0; col < img->height; col++)
			tmp += (unsigned char)img->imageData[col * img->widthStep + row];

		if(tmp > max_y)
		{
			max_y = tmp;
			max_y_idx = row;
		}
	}

	//cvLine(img,cvPoint(max_y_idx,0), cvPoint(max_y_idx,img->height),cvScalar(255,255,255),1);

	CBlobLabeling blob;
	blob.SetParam(img, 500); // ���̺� �� ����� �ּ� �ȼ� �� 
	blob.DoLabeling();

	int min_dist = img->width;
	int min_dist_idx = 0;
	if(blob.m_nBlobs != 1)
	{
		for(int i = 0; i < blob.m_nBlobs; i ++)
		{
			int rough_x_coord = (blob.m_recBlobs[i].width / 2) + blob.m_recBlobs[i].x;
			int dist = (int)((abs)((double)rough_x_coord - (double) max_y_idx));

			if(min_dist > dist)
			{
				min_dist = dist;
				min_dist_idx = i;
			}
		}

		rough_region.x = blob.m_recBlobs[min_dist_idx].x;
		rough_region.y = blob.m_recBlobs[min_dist_idx].y;
		rough_region.width = blob.m_recBlobs[min_dist_idx].width;
		rough_region.height = blob.m_recBlobs[min_dist_idx].height;
	}
	else
	{
		// �Ӵ��� ������ ������ �̾��� ��� (�� ���� ���̰� 1.3 �� �̻�)
		if(blob.m_recBlobs[0].width >= blob.m_recBlobs[0].height * 1.5)
		{
			//printf("in!!!\n"); 
			IplImage * tmp_roi = NULL;

			int win_size = blob.m_recBlobs[0].height;

			tmp_roi = cvCreateImage(cvSize(blob.m_recBlobs[0].width, blob.m_recBlobs[0].height),8,1);
			
			cvSetImageROI(img,cvRect(blob.m_recBlobs[0].x,blob.m_recBlobs[0].y,blob.m_recBlobs[0].width,blob.m_recBlobs[0].height));

			cvCopy(img,tmp_roi,0);
			cvResetImageROI(img);
			//printf("%d %d \n",tmp_roi->width, tmp_roi->height);

			int max_sum = 0;
			int max_sum_idx = 0;
			for(int win_mv = 0; win_mv < tmp_roi->width - win_size; win_mv++)
			{
				int tmp_sum = 0;
				for(int col = 0; col < tmp_roi->height; col++)
				{
					
					for(int row = win_mv; row < win_mv + win_size - 1; row++)
					{
						tmp_sum += tmp_roi->imageData[col * tmp_roi->widthStep + row];
					}
					
				}
				if(max_sum < tmp_sum)
				{
					max_sum = tmp_sum;
					max_sum_idx = win_mv;
				}
			}
			cvReleaseImage(&tmp_roi);

			rough_region.x = max_sum_idx + blob.m_recBlobs[0].x;
			rough_region.y = blob.m_recBlobs[0].y ;
			rough_region.width = win_size;
			rough_region.height = blob.m_recBlobs[0].height;


		}
		else
		{
			rough_region.x = blob.m_recBlobs[0].x;
			rough_region.y = blob.m_recBlobs[0].y;
			rough_region.width = blob.m_recBlobs[0].width;
			rough_region.height = blob.m_recBlobs[0].height;
		}

	}

	// �߸��� ���� �� ��� ���� ���� ������ ��ü
	if(rough_region.x >= 0 && rough_region.y >= 0 && rough_region.width >= 0 && rough_region.height >=0)
	{
		prev_rough_region = rough_region;
		return rough_region;
	}
	else
		return prev_rough_region;

}










CvPoint GetTempPupilCenter(IplImage* binary_img, int* tmp_pupil_diameter)
{
	CvPoint res = cvPoint(0, 0);
	
	int max = 0;
	// �뷫���� ������ X��ǥ ã��(1)
	for(int i = 0; i < binary_img->width; i++)
	{
		int tmp = 0;

		for(int j = 0; j < binary_img->height; j++)
			tmp += (unsigned char)binary_img->imageData[j*binary_img->widthStep + i];

		if(tmp > max)
		{
			max = tmp;
			res.x = i;
		}
	}

	// �뷫���� ������ X��ǥ ã��(2)
	bool b_find = false;
	int first_index=0, last_index=0;
	for(int i = res.x - 50; i <= res.x + 50; i++)
	{
		int tmp = 0;

		for(int j = 0; j < binary_img->height; j++)
			tmp += (unsigned char)binary_img->imageData[j*binary_img->widthStep + i];

		if(tmp != 0)
		{
			if(!b_find)
			{
				b_find = true;
				first_index = i;
				max = tmp;
			}
			else
			{
				if(tmp > max)
					last_index = i;
			}
		}
	}

	//first, last line �׸���
	/*for(int i=0; i<binary_img->width; i++)
	{
		if(i == first_index || i == last_index)
		{
			for(int j=0; j<binary_img->height; j++)
				binary_img->imageData[j*binary_img->widthStep + i] = 255;
		}
	}*/

	res.x = (first_index + last_index) / 2;
	*tmp_pupil_diameter = last_index - first_index;

	// �뷫���� ������ Y��ǥ ã��
	CBlobLabeling blob;
	blob.SetParam(binary_img, 30); // ���̺� �� ����� �ּ� �ȼ� �� 
	blob.DoLabeling();
	//cvReleaseImage(&rough_pupil_region_img);

	//printf("%d ",blob.m_nBlobs);
	// ���� ����(1) : ���� ���� ������ 2:1�� ���� �ʴ´�
	// ���� ����(2) : ���� ū ������ �����Ѵ�.
	int pupil_width_max = 0;
	int pupil_max_idx = 0;

	CvPoint p1 = {0,0};
	CvPoint p2 = {0,0};

	int extra_region = 10;

	if(blob.m_nBlobs != 1)
	{
		for(int i = 0; i < blob.m_nBlobs; i++)
		{
			// ���� ����(1)
			if( (blob.m_recBlobs[i].width / 1.8) <= blob.m_recBlobs[i].height)
			{
				//���� ���� (2)
				if(pupil_width_max < blob.m_recBlobs[i].width)
				{
					pupil_width_max = blob.m_recBlobs[i].width;
					pupil_max_idx = i;
				}
			}
		}
		p1 = cvPoint(blob.m_recBlobs[pupil_max_idx].x,blob.m_recBlobs[pupil_max_idx].y);
		p2 = cvPoint(p1.x + blob.m_recBlobs[pupil_max_idx].width, p1.y + blob.m_recBlobs[pupil_max_idx].height);
	}
	else
	{
		p1 = cvPoint(blob.m_recBlobs[0].x,blob.m_recBlobs[0].y);
		p2 = cvPoint(p1.x + blob.m_recBlobs[0].width, p1.y + blob.m_recBlobs[0].height);
	}

	// blob ���� Ȯ��(extra_region ��ŭ��)
	if(p1.y - extra_region <= 0) p1.y = 0;
	else                         p1.y -= extra_region;

	if(p2.y + extra_region >= binary_img->height) p2.y = binary_img->height;
	else                                          p2.y += extra_region;

	res.y = (p1.y + p2.y) / 2;

	return res;
}



void remove_residual_binary_region(IplImage* binary)
{
	//cvShowImage("test", binary);
	CBlobLabeling r_blob;
	r_blob.SetParam(binary, 30); // ���̺� �� ����� �ּ� �ȼ� �� 
	r_blob.DoLabeling();
	
	int max_area = 0;
	int max_area_idx = 0;
	if(r_blob.m_nBlobs >= 2)
	{

		// ���� ū ������ ���� ����� �ε��� ����
		for(int i = 0; i < r_blob.m_nBlobs; i++)
		{
			int tmp_area = r_blob.m_recBlobs[i].width * r_blob.m_recBlobs[i].height;

			if(tmp_area > max_area)
			{
				max_area =  tmp_area;
				max_area_idx = i;
			}
		}

		cvSetImageROI(binary,cvRect(r_blob.m_recBlobs[max_area_idx].x, r_blob.m_recBlobs[max_area_idx].y, 
			r_blob.m_recBlobs[max_area_idx].x + r_blob.m_recBlobs[max_area_idx].width, r_blob.m_recBlobs[max_area_idx].y + r_blob.m_recBlobs[max_area_idx].height));

		cvShowImage("test", binary);
		cvResetImageROI(binary);

		/*
		printf("%d %d \n", r_blob.m_recBlobs[max_area_idx].x , r_blob.m_recBlobs[max_area_idx].x + r_blob.m_recBlobs[max_area_idx].width);

		for(int col = 0; col < binary->height; col++)
		{
			for(int row = 0; row < binary->width; row++)
			{
				if(row >= r_blob.m_recBlobs[max_area_idx].x && row <= r_blob.m_recBlobs[max_area_idx].x + r_blob.m_recBlobs[max_area_idx].width)
					continue;
				else
					binary->imageData[col * binary->widthStep + row ] = 0;

			}
		}
		*/	
	}





	/*
	CvPoint *blob_centers = NULL;
	int pupil_cent_x = 0;
	int pupil_cent_y = 0;

	int min_x = binary->width;
	int min_y = binary->height;

	if(r_blob.m_nBlobs >=2) // ����� 2�� �̻��̸� ������ ����
	{
		blob_centers = (CvPoint*)malloc(sizeof(CvPoint) * r_blob.m_nBlobs);
		for(int i = 0; i < r_blob.m_nBlobs; i++)
		{
			blob_centers->x = 0;
			blob_centers->y = 0;	
			
			
			// ���� (1) : �̹��� �߽ɿ� ����� ���� ������ �����ϰ� ����
			blob_centers[i].x = r_blob.m_recBlobs[i].x + (r_blob.m_recBlobs[i].width / 2);
			blob_centers[i].y = r_blob.m_recBlobs[i].y + (r_blob.m_recBlobs[i].height / 2);

			if((abs)((double)binary->width/2 - (double)blob_centers[i].x) < min_x)
			{
				min_x = (abs)((double)binary->width/2 - (double)blob_centers[i].x);
				pupil_cent_x = i;
			}
			if((abs)((double)binary->height/2 - (double)blob_centers[i].y) < min_y)
			{
				min_y = (abs)((double)binary->height/2 - (double)blob_centers[i].y);
				pupil_cent_y = i;
			}
		}

		for(int i = 0; i < r_blob.m_nBlobs; i++)
		{
			if(blob_centers[i].x != pupil_cent_x && blob_centers[i].y != pupil_cent_y )
			{
				for(int col = r_blob.m_recBlobs[i].y; col < r_blob.m_recBlobs[i].y + r_blob.m_recBlobs[i].height; col++)
					for(int row = r_blob.m_recBlobs[i].x; row < r_blob.m_recBlobs[i].x + r_blob.m_recBlobs[i].width; row++)
						binary->imageData[col * binary->widthStep + row] = 0;
	
			}
		}

	}
	free(blob_centers);
	*/

}