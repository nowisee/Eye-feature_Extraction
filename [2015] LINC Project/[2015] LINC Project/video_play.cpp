#include "header.h"
#include "video.h"
#include "threshold.h"
#include "contour.h"
//#include "labeling.h"
#include <stdlib.h>
#include <math.h>

/***************************************************************
[Function Name] 
void VideoPlay();

[Function]
���� �ȿ� ����� �������� �����ϴ� ����

[Note]
��κ��� �ֿ� �Լ��� ������ ����

****************************************************************/

void VideoPlay(CvMat* trans_matrix[4], CvPoint calib_point[][3], bool calib)
{
	FILE *dataofp;
	if(!calib) 
		dataofp = fopen("data.txt", "wt");

	int v_sel = 0; // ������ ���� ����
	char key = 0; // ������ ���� ����
	char (*video_name)[FILENAME_LEN] = {0,}; // ������ �̸����� ����

	CvCapture* capture = NULL; // ������ �б� ����
	IplImage* frame = NULL; // ������ ������ ����

	IplImage* gray_img = cvCreateImage(cvSize(WIDTH, HEIGHT), 8, 1);
	IplImage* ROI_img = cvCreateImage(cvSize(CROP_WIDTH, CROP_HEIGHT), 8, 1);// ROI �̹���
	IplImage* smooth_img = cvCreateImage(cvSize(CROP_WIDTH, CROP_HEIGHT), 8, 1);// smoothing �̹���
	IplImage* binary_img = cvCreateImage(cvSize(CROP_WIDTH, CROP_HEIGHT), 8, 1);// binary �̹��� 
	IplImage* contour_img = cvCreateImage(cvSize(CROP_WIDTH, CROP_HEIGHT), 8, 1);// contour �̹��� 

	// Calibration Mapping ����
	IplImage* gaze_map = cvCreateImage(cvSize(calib_point[1][2].x - calib_point[1][0].x, calib_point[2][1].y - calib_point[0][1].y), 8, 1);;
	IplImage* screen_map = cvCreateImage(cvSize(S_WIDTH, S_HEIGHT), 8, 1);
	CvPoint trans_center;

	/* ������ ���� */
	//char (*video_name)[FILENAME_LEN] = FindVideos();
	video_name = FindVideos();

	printf("���� ���� : ");
	scanf("%d",&v_sel);
	video_name += (v_sel - 1); //�ּ� �̵�
	printf("���õ� ���� : %s\n",*video_name);
	/* End of ������ ���� */

	capture = cvCreateFileCapture(*video_name);

	CvRect eye_region_roi = cvRect(CROP_W_STRT, CROP_H_STRT, CROP_WIDTH, CROP_HEIGHT );

	// ���� ���� ����ȭ �Ӱ谪 ���� ���� ����(������ ��հ�)
	int frame_cnt = 0;
	int thresh_sum = 0;
	int pupil_thresh;

	// �뷫���� ���� ���� ���� ����
	int tmp_pupil_diameter = 0;
	CvPoint t_center;

	// ���� ��輱 ���� ����
	int upper_range, lower_range;
	CvPoint left_contour[MAX_VERTEX], right_contour[MAX_VERTEX];
	CvPoint final_left_contour[MAX_VERTEX], final_right_contour[MAX_VERTEX];
	int l_cnt=0, r_cnt=0;
	int left_num, right_num;

	// �� ������ ���� ����
	bool blink = false;
	int blink_count = 0;
	bool b_draw = false;;

	// ���� ���� ����
	CvPoint center;
	CvPoint final_pupil_center;
	CvBox2D box;
	CvSize size;
	bool b_RANSAC;
	bool b_find;
	int draw_cnt=0;
	double area;

	int calib_num = 1;
	int key_cnt=0;

	while(1)
	{
		//frame = Video(capture);
		frame = cvQueryFrame( capture );

		if(!frame)
		{
			cvReleaseImage(&frame);
			printf("������ ����!!\n");
			break;
		}

		//printf("[%d %d %d] ", frame->imageData[0], frame->imageData[1], frame->imageData[2]);
		if((unsigned int)frame->imageData[0] == 0 && (unsigned int)frame->imageData[1] == 0 && (unsigned int)frame->imageData[2] == 0 && !calib)
		{
			fprintf(dataofp, "\n%d %d\n", blink_count, draw_cnt);
			key_cnt++;
			blink_count = 0;
			draw_cnt = 0;
			cvZero(screen_map);
			fprintf(dataofp, "\n%d\n", key_cnt); 
			printf("%d\n", key_cnt);
			continue;
		}

		IplImage* resize = cvCreateImage(cvSize(WIDTH, HEIGHT), frame->depth, frame->nChannels);
		cvResize(frame, resize, 1);


		//IplImage* gray = cvCreateImage(cvGetSize(resize), 8, 1);

		if(resize -> nChannels == 3)
			cvCvtColor(resize, gray_img, CV_BGR2GRAY);
		else
			cvCopy(resize,gray_img,0);

		cvReleaseImage(&resize);

		//cvShowImage("gray", gray);

		//cvCopy(binary_img, contour_img,0);

		if(!frame || key >= 27)
			break;

		// (1) �̹��� ���� RoI
		cvSetImageROI(gray_img, eye_region_roi);
		cvCopy(gray_img, ROI_img,0);
		cvResetImageROI(gray_img);

		// (2) �̹��� ������ 
		cvSmooth(ROI_img, smooth_img, CV_MEDIAN, 5,0,0,0);

		frame_cnt++;
		pupil_thresh = GetPupilThresh(smooth_img, frame_cnt, &thresh_sum, &blink);

		if(thresh_sum > 2000000000) // �Ӱ谪 ���� �ִ� ����
		{
			thresh_sum /= 10;
			frame_cnt /= 10;
		}			

		//cvErode(smooth_img,smooth_img,0,1);
		cvThreshold(smooth_img, binary_img, pupil_thresh, 255,CV_THRESH_BINARY_INV);
		//cvShowImage("binary_img ������",binary_img);

		//printf("%d %d %d %d\n",rough_pupil.x,rough_pupil.y,rough_pupil.width,rough_pupil.height);
		//cvRectangle(smooth_img,cvPoint(rough_pupil.x, rough_pupil.y), cvPoint(rough_pupil.x + rough_pupil.width , rough_pupil.y + rough_pupil.height),cvScalar(255,255,255),1);

		//printf("[%d %d] ", glint_range.y, glint_range.height);

		// �� ������ ����
		if(blink == true) // �� ������ �߻�
		{
			blink_count++;
			continue;
		}
		else
		{
			// �뷫���� ���� ���� ȹ��
			int extra_region = 5;
			CvRect rough_pupil = RoughPupilRegion(binary_img);
			if(rough_pupil.x <= 0 || rough_pupil.y <= 0 || rough_pupil.width <= 0 || rough_pupil.height <= 0)
				continue;
			rough_pupil.x -= extra_region;
			rough_pupil.y -= extra_region;
			rough_pupil.width += (2*extra_region);
			rough_pupil.height += (2*extra_region);

			// ���� ����(���� ������ �̹��� ������ ���� �� �̹��� ũ��� ����)
			if(rough_pupil.x <= 0) rough_pupil.x = 0;
			if(rough_pupil.y <= 0) rough_pupil.y = 0;
			if(rough_pupil.x + rough_pupil.width  >= binary_img->width)
				rough_pupil.width = binary_img->width - rough_pupil.x - 1;
			if(rough_pupil.y + rough_pupil.height  >= binary_img->height)
				rough_pupil.height = binary_img->height - rough_pupil.y - 1;

			IplImage* rough_pupil_region_img = cvCreateImage(cvSize(rough_pupil.width,rough_pupil.height),8,1);
			IplImage* edge_img = cvCreateImage(cvGetSize(rough_pupil_region_img), 8, 1);
			IplImage* pupil_binary_img = cvCreateImage(cvSize(rough_pupil.width,rough_pupil.height),8,1); //�뷫���� ���� ���� ����ȭ

			// �뷫���� ���� ���� ����
			cvSetImageROI(smooth_img,rough_pupil);
			cvCopy(smooth_img,rough_pupil_region_img,0);
			cvResetImageROI(smooth_img);

			cvThreshold(rough_pupil_region_img,pupil_binary_img,pupil_thresh ,255,CV_THRESH_BINARY_INV);

			//cvShowImage("rough_pupil_region_img",rough_pupil_region_img);
			//cvShowImage("edge_img",edge_img);

			t_center.x = rough_pupil.x + rough_pupil.width/2;
			t_center.y = rough_pupil.y + rough_pupil.height/2;
			tmp_pupil_diameter = rough_pupil.width - 2*extra_region;

			cvThreshold(rough_pupil_region_img, pupil_binary_img,pupil_thresh ,255,CV_THRESH_BINARY_INV);

			// �۸�Ʈ ���� ã��
			//(1) ���� �۸�Ʈ ����
			RemoveGlint(pupil_binary_img);

			// ���� ������ ã��
			RealContour(pupil_binary_img, t_center);

			// ���� �߽��� �������� ��, �Ʒ� �� line�� ���� �������� ���� ������ ����
			SetRange(&upper_range, &lower_range, tmp_pupil_diameter);

			IplImage* contour_img = cvCreateImage(cvGetSize(rough_pupil_region_img), 8, 1);
			//cvCopy(rough_pupil_region_img, contour_img,0);
			cvCopy(pupil_binary_img, contour_img,0);
			//cvShowImage("pupil_binary_img", contour_img);

			// 1�� ���� �¿� ��輱 ���� �� ����
			TempContourDetection(smooth_img, contour_img, t_center, left_contour, right_contour, upper_range, lower_range, &l_cnt, &r_cnt, rough_pupil);
			cvReleaseImage(&contour_img);

			/*IplImage* smooth_img2 = cvCreateImage(cvSize(CROP_WIDTH, CROP_HEIGHT), 8, 1);
			cvCopy(smooth_img, smooth_img2, 0);
			DrawContour(smooth_img2, left_contour, right_contour, l_cnt, r_cnt);
			cvShowImage("Smoothing ������(���� ��)",smooth_img2);*/

			// �۸�Ʈ ���� �޴� ��輱 ���� ����
			//FilteringContour(smooth_img2, left_contour, right_contour, l_cnt, r_cnt, t_center);
			//cvReleaseImage(&smooth_img2);

			// ���� ��輱 ȹ�� �� RANSAC �̿��� ���� ���� �߽� Ž��
			if(l_cnt > 7 && r_cnt > 7)
			{
				center = GetFinalPupilCenter(left_contour, right_contour, final_left_contour, final_right_contour, l_cnt, r_cnt, &left_num, &right_num, &box, &size, &b_RANSAC, &b_find);

				// �� ROI ���� ��ǥ ���ϱ�
				if(b_RANSAC && b_find && (0.7 <= (float)size.height / size.width && (float)size.height / size.width <= 1.3) )
				{
					final_pupil_center.x = center.x + CROP_W_STRT;
					final_pupil_center.y = center.y + CROP_H_STRT;
				}
			}

			// Draw Pupil Center Line and Pupil Contours
			DrawPupilCenterLine(smooth_img, t_center);
			DrawContour(smooth_img, final_left_contour, final_right_contour, left_num, right_num);
			/*if(l_cnt > 16 && r_cnt > 16)
			DrawContour(smooth_img, final_left_contour, final_right_contour, l_cnt + 20, r_cnt + 20);*/
			
			b_draw = false;
			// ���� ����
			// ������ �߽��� ������ ���� ���� �־�� �Ѵ�.
			if(final_pupil_center.x > 0 && final_pupil_center.x < frame->width && final_pupil_center.y > 0 && final_pupil_center.y < frame->height)
			{
				if(size.width > 10 && size.height > 10 && b_find)
				{
					DrawPupilInfo(frame, final_pupil_center, box, size);
					b_draw = true;
					area = size.width * size.height * PI;
				}
				else
				{
					final_pupil_center.x = 0;
					final_pupil_center.y = 0;
				}
			}

			key = cvWaitKey(33);

			if(key == 27) 
				break;

			if(calib) // Ķ���극�̼�
			{
				// Calibration() �̿��Ͽ� trans_matrix �ʱ�ȭ
				if(key == 13)
				{
					if(final_pupil_center.x != 0 && final_pupil_center.x != 0)
					{
						if(calib_num < 10)
						{
							switch(calib_num)
							{
							case 1: calib_point[0][0].x = final_pupil_center.x; calib_point[0][0].y = final_pupil_center.y; break;
							case 2: calib_point[0][1].x = final_pupil_center.x; calib_point[0][1].y = final_pupil_center.y; break;
							case 3: calib_point[0][2].x = final_pupil_center.x; calib_point[0][2].y = final_pupil_center.y; break;
							case 4: calib_point[1][2].x = final_pupil_center.x; calib_point[1][0].y = final_pupil_center.y; break;
							case 5: calib_point[1][1].x = final_pupil_center.x; calib_point[1][1].y = final_pupil_center.y; break;
							case 6: calib_point[1][0].x = final_pupil_center.x; calib_point[1][2].y = final_pupil_center.y; break;
							case 7: calib_point[2][0].x = final_pupil_center.x; calib_point[2][0].y = final_pupil_center.y; break;
							case 8: calib_point[2][1].x = final_pupil_center.x; calib_point[2][1].y = final_pupil_center.y; break;
							case 9: calib_point[2][2].x = final_pupil_center.x; calib_point[2][2].y = final_pupil_center.y; break;
							default : break;
							}
							printf("Get Point Position!(%d)\n", calib_num);
							calib_num++;
						}
						else
						{
							/*printf("----------Calibration Point----------\n");
							printf("(%d, %d)  (%d, %d)  (%d, %d)\n", calib_point[0][0].x, calib_point[0][0].y, calib_point[0][1].x, calib_point[0][1].y, calib_point[0][2].x, calib_point[0][2].y);
							printf("(%d, %d)  (%d, %d)  (%d, %d)\n", calib_point[1][0].x, calib_point[1][0].y, calib_point[1][1].x, calib_point[1][1].y, calib_point[1][2].x, calib_point[1][2].y);
							printf("(%d, %d)  (%d, %d)  (%d, %d)\n", calib_point[2][0].x, calib_point[2][0].y, calib_point[2][1].x, calib_point[2][1].y, calib_point[2][2].x, calib_point[2][2].y);
							printf("\n\n");*/

							//Calibration(trans_matrix, calib_point);

							for(int i=0; i<4; i++)
								TransMatrix(trans_matrix, calib_point, i);

							break;
						}
					}
					else
					{
						printf("Center of Pupil is not Found!!\n");
					}
				}
			}
			else // ����
			{
				// �� Ư¡ ����				
				//cvZero(gaze_map);
				//cvZero(screen_map);

				/*if(final_pupil_center.y-calib_point[0][1].y >= 0 && final_pupil_center.x - calib_point[1][0].x >= 0)
				{
					gaze_map->imageData[(final_pupil_center.y - calib_point[0][1].y)*gaze_map->widthStep + (final_pupil_center.x - calib_point[1][0].x)] = 255;
					gaze_map->imageData[(final_pupil_center.y - calib_point[0][1].y + 1)*gaze_map->widthStep + (final_pupil_center.x - calib_point[1][0].x)] = 255;
					gaze_map->imageData[(final_pupil_center.y - calib_point[0][1].y)*gaze_map->widthStep + (final_pupil_center.x - calib_point[1][0].x + 1)] = 255;
					gaze_map->imageData[(final_pupil_center.y - calib_point[0][1].y + 1)*gaze_map->widthStep + (final_pupil_center.x - calib_point[1][0].x + 1)] = 255;
				}*/

				trans_center = GazeMapping(trans_matrix, calib_point, final_pupil_center);

				if(trans_center.x < 0) trans_center.x = 0;
				if(trans_center.x >= S_WIDTH-2) trans_center.x = S_WIDTH-2;
				if(trans_center.y < 0) trans_center.y = 0;
				if(trans_center.y >= S_HEIGHT-2) trans_center.y = S_HEIGHT-2;

				//if(0 <= trans_center.x && trans_center.x-1 < S_WIDTH && 0 <= trans_center.y && trans_center.y-1 < S_HEIGHT)
				{
					screen_map->imageData[trans_center.y * screen_map->widthStep + trans_center.x] = 255;
					screen_map->imageData[(trans_center.y + 1) * screen_map->widthStep + trans_center.x] = 255;
					screen_map->imageData[trans_center.y * screen_map->widthStep + (trans_center.x + 1)] = 255;
					screen_map->imageData[(trans_center.y + 1) * screen_map->widthStep + (trans_center.x + 1)] = 255;
				}

				//cvShowImage("Gaze Map", gaze_map);
				cvShowImage("Screen Map", screen_map);

				if(b_draw)
				{
					fprintf(dataofp, "%d %d %f\n", trans_center.x, trans_center.y, area);
					draw_cnt++;
				}
			}

			cvReleaseImage(&rough_pupil_region_img);
			cvReleaseImage(&edge_img);
		} // end of ���������� �ƴ� ��� 


		//cvShowImage("Smoothing ������(���� ��)",smooth_img);
		//cvShowImage("binary_img ������",binary_img);
		cvShowImage("result ������",frame);
	}

	//cvDestroyWindow("pupil_binary_img");
	//cvDestroyWindow("Ellipse Image");
	//cvDestroyWindow("Smoothing ������(���� ��)");
	//cvDestroyWindow("Smoothing ������(���� ��)");
	//cvDestroyWindow("binary_img ������");
	cvDestroyWindow("result ������");
	cvDestroyWindow("Gaze Map");
	cvDestroyWindow("Screen Map");

	if(!calib)
		cvReleaseImage(&gaze_map);
	
	cvReleaseImage(&screen_map);	
	cvReleaseImage(&binary_img);
	cvReleaseImage(&ROI_img);
	cvReleaseImage(&smooth_img);
	cvReleaseImage(&gray_img);

	cvReleaseCapture(&capture);

	if(!calib)
		fclose(dataofp);
}