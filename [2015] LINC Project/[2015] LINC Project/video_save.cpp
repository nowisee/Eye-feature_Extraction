#include "header.h"
#include "video.h"

/***************************************************************
[Function Name] 
void Video();

[Function]
�������� �������� IplImage* ���� �������� ����

[Note]
�̹����� ������ ��ȭ, �÷� �̹����� �׷��� ȭ

****************************************************************/
IplImage* Video(CvCapture* capture)
{
	//cvGrabFrame( capture );
	//img = cvRetrieveFrame( capture );

	IplImage* img = cvQueryFrame( capture );
	
	if(!img)
	{
		printf("������ ����!!\n");
		return 0;
	}
	IplImage* resize = cvCreateImage(cvSize(WIDTH, HEIGHT),img->depth,img->nChannels);
	cvResize(img, resize, 1);

	
	IplImage* gray = cvCreateImage(cvGetSize(resize), 8, 1);

	if(resize -> nChannels == 3)
		cvCvtColor(resize, gray, CV_BGR2GRAY);
	else
		cvCopy(resize,gray,0);
		
	cvReleaseImage(&resize);

	return gray;
}

void VideoSave()
{
		IplImage* img = NULL;
		CvCapture* capture = NULL;
		CvVideoWriter* VideoOut = NULL;
		capture = cvCaptureFromCAM(0);
		int point_num = 1;

		/* ��¥�� �ǽ������� �̸��� ���ϸ����� ���� */
		char time_arr[50] = {0,};
		char sub_name[20] = {0,};
		char sub_sex[2];
		char sub_age[5];
		time_t now = time(NULL);
		strftime(time_arr, 20, "%Y-%m-%d", localtime(&now));
		printf("Subject name : ");
		scanf("%s", sub_name);
		printf("Sex(M / F) : ");
		scanf("%s", sub_sex);
		if(sub_sex[0] >= 97) sub_sex[0] -= 32;
		printf("Age : ");
		scanf("%s", sub_age);

		strcat(time_arr," ");
		strcat(time_arr,sub_name);
		strcat(time_arr,"(");
		strcat(time_arr,sub_sex);
		strcat(time_arr,",");
		strcat(time_arr,sub_age);
		strcat(time_arr,").avi");

		//IplImage *screen_img;
		//screen_img = cvCreateImage(cvSize(S_WIDTH, S_HEIGHT), 8, 3);
		//cvZero(screen_img);
		//cvThreshold(screen_img, screen_img, 128, 255, CV_THRESH_BINARY_INV);

		//// Init Calibration Point
		//CvPoint point_pos[11];
		//int blue_point = 0;

		//for(int k=1; k<=10; k++)
		//{
		//	switch(k)
		//	{
		//	case 1: point_pos[k].x = 0; point_pos[k].y = 0; break;
		//	case 2: point_pos[k].x = S_WIDTH/2 - 10; point_pos[k].y = 0; break;
		//	case 3: point_pos[k].x = S_WIDTH - 20; point_pos[k].y = 0; break;
		//	case 4: point_pos[k].x = S_WIDTH - 20; point_pos[k].y = S_HEIGHT/2 - 10; break;
		//	case 5: point_pos[k].x = S_WIDTH/2 - 10; point_pos[k].y = S_HEIGHT/2 - 10; break;
		//	case 6: point_pos[k].x = 0; point_pos[k].y = S_HEIGHT/2 - 10; break;
		//	case 7: point_pos[k].x = 0; point_pos[k].y = S_HEIGHT - 20; break;
		//	case 8: point_pos[k].x = S_WIDTH/2 - 10; point_pos[k].y = S_HEIGHT - 20; break;
		//	case 9: point_pos[k].x = S_WIDTH - 20; point_pos[k].y = S_HEIGHT - 20; break;
		//	case 10: point_pos[k].x = S_WIDTH/2 - 10; point_pos[k].y = S_HEIGHT/2 - 10; break;
		//	default : break;
		//	}

		//	for(int i=0; i<20; i++)
		//	{
		//		for(int j=0; j<20; j++)
		//		{
		//			screen_img->imageData[(point_pos[k].y+i) * screen_img->widthStep + 3*(point_pos[k].x+j) + 0] = 0;
		//			screen_img->imageData[(point_pos[k].y+i) * screen_img->widthStep + 3*(point_pos[k].x+j) + 1] = 0;
		//			screen_img->imageData[(point_pos[k].y+i) * screen_img->widthStep + 3*(point_pos[k].x+j) + 2] = 255;
		//		}
		//	}
		//}

		/* ������ ���� */
		while(1)
		{
			img = Video(capture);

			if(VideoOut == NULL)
				VideoOut = cvCreateVideoWriter(time_arr, CV_FOURCC('D', 'I', 'V', '3'), 
				30, cvGetSize(img), 0); // ������ ���ڴ� color ���� ���� (1 = �÷�, 2 = �׷���)

			cvWriteFrame(VideoOut, img);

			for(int i=CROP_H_STRT; i<CROP_H_STRT+CROP_HEIGHT; i++)
			{
				if(i == CROP_H_STRT || i == CROP_H_STRT+CROP_HEIGHT - 1)
				{
					for(int j=CROP_W_STRT; j<CROP_W_STRT+CROP_WIDTH; j++)
						img->imageData[i * img->widthStep + j] = 255;
				}
				else
				{
					img->imageData[i * img->widthStep + CROP_W_STRT] = 255;
					img->imageData[i * img->widthStep + CROP_W_STRT+CROP_WIDTH-1] = 255;
				}
			}

			cvShowImage("test", img);

			/*if(cvWaitKey(10) >= 0)
			{
				if(blue_point == 0)
				{
					blue_point++;
					for(int i=0; i<20; i++)
					{
						for(int j=0; j<20; j++)
						{
							screen_img->imageData[(point_pos[blue_point].y+i) * screen_img->widthStep + 3*(point_pos[blue_point].x+j) + 0] = 255;
							screen_img->imageData[(point_pos[blue_point].y+i) * screen_img->widthStep + 3*(point_pos[blue_point].x+j) + 1] = 0;
							screen_img->imageData[(point_pos[blue_point].y+i) * screen_img->widthStep + 3*(point_pos[blue_point].x+j) + 2] = 0;
						}
					}
				}
				else if(0 < blue_point && blue_point < 10)
				{
					for(int i=0; i<20; i++)
					{
						for(int j=0; j<20; j++)
						{
							screen_img->imageData[(point_pos[blue_point].y+i) * screen_img->widthStep + 3*(point_pos[blue_point].x+j) + 0] = 0;
							screen_img->imageData[(point_pos[blue_point].y+i) * screen_img->widthStep + 3*(point_pos[blue_point].x+j) + 1] = 0;
							screen_img->imageData[(point_pos[blue_point].y+i) * screen_img->widthStep + 3*(point_pos[blue_point].x+j) + 2] = 255;
						}
					}
					blue_point++;
					for(int i=0; i<20; i++)
					{
						for(int j=0; j<20; j++)
						{
							screen_img->imageData[(point_pos[blue_point].y+i) * screen_img->widthStep + 3*(point_pos[blue_point].x+j) + 0] = 255;
							screen_img->imageData[(point_pos[blue_point].y+i) * screen_img->widthStep + 3*(point_pos[blue_point].x+j) + 1] = 0;
							screen_img->imageData[(point_pos[blue_point].y+i) * screen_img->widthStep + 3*(point_pos[blue_point].x+j) + 2] = 0;
						}
					}
				}
				else
					break;
			}

			cvShowImage("Screen", screen_img);*/

			if(cvWaitKey(10) >= 0) break;
		}

		/*cvDestroyWindow("Screen");
		cvReleaseImage(&screen_img);*/

		cvDestroyWindow("test");
		cvReleaseCapture( &capture );
		cvReleaseVideoWriter(&VideoOut);

		/* End of ������ ���� */
}