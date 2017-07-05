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
폴더 안에 저장된 동영상을 실행하는 파일

[Note]
대부분의 주요 함수를 적용할 파일

****************************************************************/

void VideoPlay(CvMat* trans_matrix[4], CvPoint calib_point[][3], bool calib)
{
	FILE *dataofp;
	if(!calib) 
		dataofp = fopen("data.txt", "wt");

	int v_sel = 0; // 동영상 선택 변수
	char key = 0; // 동영상 정지 변수
	char (*video_name)[FILENAME_LEN] = {0,}; // 동영상 이름저장 변수

	CvCapture* capture = NULL; // 동영상 읽기 변수
	IplImage* frame = NULL; // 동영상 프레임 변수

	IplImage* gray_img = cvCreateImage(cvSize(WIDTH, HEIGHT), 8, 1);
	IplImage* ROI_img = cvCreateImage(cvSize(CROP_WIDTH, CROP_HEIGHT), 8, 1);// ROI 이미지
	IplImage* smooth_img = cvCreateImage(cvSize(CROP_WIDTH, CROP_HEIGHT), 8, 1);// smoothing 이미지
	IplImage* binary_img = cvCreateImage(cvSize(CROP_WIDTH, CROP_HEIGHT), 8, 1);// binary 이미지 
	IplImage* contour_img = cvCreateImage(cvSize(CROP_WIDTH, CROP_HEIGHT), 8, 1);// contour 이미지 

	// Calibration Mapping 변수
	IplImage* gaze_map = cvCreateImage(cvSize(calib_point[1][2].x - calib_point[1][0].x, calib_point[2][1].y - calib_point[0][1].y), 8, 1);;
	IplImage* screen_map = cvCreateImage(cvSize(S_WIDTH, S_HEIGHT), 8, 1);
	CvPoint trans_center;

	/* 동영상 선택 */
	//char (*video_name)[FILENAME_LEN] = FindVideos();
	video_name = FindVideos();

	printf("파일 선택 : ");
	scanf("%d",&v_sel);
	video_name += (v_sel - 1); //주소 이동
	printf("선택된 파일 : %s\n",*video_name);
	/* End of 동영상 선택 */

	capture = cvCreateFileCapture(*video_name);

	CvRect eye_region_roi = cvRect(CROP_W_STRT, CROP_H_STRT, CROP_WIDTH, CROP_HEIGHT );

	// 동공 영역 이진화 임계값 추출 관련 변수(누적한 평균값)
	int frame_cnt = 0;
	int thresh_sum = 0;
	int pupil_thresh;

	// 대략적인 동공 추출 관련 변수
	int tmp_pupil_diameter = 0;
	CvPoint t_center;

	// 동공 경계선 관련 변수
	int upper_range, lower_range;
	CvPoint left_contour[MAX_VERTEX], right_contour[MAX_VERTEX];
	CvPoint final_left_contour[MAX_VERTEX], final_right_contour[MAX_VERTEX];
	int l_cnt=0, r_cnt=0;
	int left_num, right_num;

	// 눈 깜빡임 관련 변수
	bool blink = false;
	int blink_count = 0;
	bool b_draw = false;;

	// 최종 동공 변수
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
			printf("동영상 종료!!\n");
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

		// (1) 이미지 최초 RoI
		cvSetImageROI(gray_img, eye_region_roi);
		cvCopy(gray_img, ROI_img,0);
		cvResetImageROI(gray_img);

		// (2) 이미지 스무딩 
		cvSmooth(ROI_img, smooth_img, CV_MEDIAN, 5,0,0,0);

		frame_cnt++;
		pupil_thresh = GetPupilThresh(smooth_img, frame_cnt, &thresh_sum, &blink);

		if(thresh_sum > 2000000000) // 임계값 누적 최대 제한
		{
			thresh_sum /= 10;
			frame_cnt /= 10;
		}			

		//cvErode(smooth_img,smooth_img,0,1);
		cvThreshold(smooth_img, binary_img, pupil_thresh, 255,CV_THRESH_BINARY_INV);
		//cvShowImage("binary_img 동영상",binary_img);

		//printf("%d %d %d %d\n",rough_pupil.x,rough_pupil.y,rough_pupil.width,rough_pupil.height);
		//cvRectangle(smooth_img,cvPoint(rough_pupil.x, rough_pupil.y), cvPoint(rough_pupil.x + rough_pupil.width , rough_pupil.y + rough_pupil.height),cvScalar(255,255,255),1);

		//printf("[%d %d] ", glint_range.y, glint_range.height);

		// 눈 깜빡임 제외
		if(blink == true) // 눈 깜빡임 발생
		{
			blink_count++;
			continue;
		}
		else
		{
			// 대략적인 동공 영역 획득
			int extra_region = 5;
			CvRect rough_pupil = RoughPupilRegion(binary_img);
			if(rough_pupil.x <= 0 || rough_pupil.y <= 0 || rough_pupil.width <= 0 || rough_pupil.height <= 0)
				continue;
			rough_pupil.x -= extra_region;
			rough_pupil.y -= extra_region;
			rough_pupil.width += (2*extra_region);
			rough_pupil.height += (2*extra_region);

			// 영역 제어(동공 영역이 이미지 밖으로 나갈 때 이미지 크기로 제어)
			if(rough_pupil.x <= 0) rough_pupil.x = 0;
			if(rough_pupil.y <= 0) rough_pupil.y = 0;
			if(rough_pupil.x + rough_pupil.width  >= binary_img->width)
				rough_pupil.width = binary_img->width - rough_pupil.x - 1;
			if(rough_pupil.y + rough_pupil.height  >= binary_img->height)
				rough_pupil.height = binary_img->height - rough_pupil.y - 1;

			IplImage* rough_pupil_region_img = cvCreateImage(cvSize(rough_pupil.width,rough_pupil.height),8,1);
			IplImage* edge_img = cvCreateImage(cvGetSize(rough_pupil_region_img), 8, 1);
			IplImage* pupil_binary_img = cvCreateImage(cvSize(rough_pupil.width,rough_pupil.height),8,1); //대략적인 동공 영역 이진화

			// 대략적인 동공 영역 설정
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

			// 글린트 영역 찾기
			//(1) 내부 글린트 제거
			RemoveGlint(pupil_binary_img);

			// 동공 윤곽선 찾기
			RealContour(pupil_binary_img, t_center);

			// 동공 중심을 기준으로 위, 아래 몇 line만 남길 것인지에 대한 범위값 결정
			SetRange(&upper_range, &lower_range, tmp_pupil_diameter);

			IplImage* contour_img = cvCreateImage(cvGetSize(rough_pupil_region_img), 8, 1);
			//cvCopy(rough_pupil_region_img, contour_img,0);
			cvCopy(pupil_binary_img, contour_img,0);
			//cvShowImage("pupil_binary_img", contour_img);

			// 1차 동공 좌우 경계선 추출 및 보정
			TempContourDetection(smooth_img, contour_img, t_center, left_contour, right_contour, upper_range, lower_range, &l_cnt, &r_cnt, rough_pupil);
			cvReleaseImage(&contour_img);

			/*IplImage* smooth_img2 = cvCreateImage(cvSize(CROP_WIDTH, CROP_HEIGHT), 8, 1);
			cvCopy(smooth_img, smooth_img2, 0);
			DrawContour(smooth_img2, left_contour, right_contour, l_cnt, r_cnt);
			cvShowImage("Smoothing 동영상(보정 전)",smooth_img2);*/

			// 글린트 영향 받는 경계선 점들 제거
			//FilteringContour(smooth_img2, left_contour, right_contour, l_cnt, r_cnt, t_center);
			//cvReleaseImage(&smooth_img2);

			// 최종 경계선 획득 및 RANSAC 이용한 최종 동공 중심 탐지
			if(l_cnt > 7 && r_cnt > 7)
			{
				center = GetFinalPupilCenter(left_contour, right_contour, final_left_contour, final_right_contour, l_cnt, r_cnt, &left_num, &right_num, &box, &size, &b_RANSAC, &b_find);

				// 눈 ROI 영역 좌표 더하기
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
			// 조건 제어
			// 동공의 중심은 영상의 범위 내에 있어야 한다.
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

			if(calib) // 캘리브레이션
			{
				// Calibration() 이용하여 trans_matrix 초기화
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
			else // 실험
			{
				// 눈 특징 추출				
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
		} // end of 눈깜빡임이 아닌 경우 


		//cvShowImage("Smoothing 동영상(보정 후)",smooth_img);
		//cvShowImage("binary_img 동영상",binary_img);
		cvShowImage("result 동영상",frame);
	}

	//cvDestroyWindow("pupil_binary_img");
	//cvDestroyWindow("Ellipse Image");
	//cvDestroyWindow("Smoothing 동영상(보정 전)");
	//cvDestroyWindow("Smoothing 동영상(보정 후)");
	//cvDestroyWindow("binary_img 동영상");
	cvDestroyWindow("result 동영상");
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