#include "video.h"

void TransMatrix(CvMat* trans_matrix[4], CvPoint gaze_point[][3], int quad_num)
{
	double nrow = 4.0;
	double ncol = 4.0;

	// 응시점
	double x1, x2, x3, x4;
	double y1, y2, y3, y4;

	// 모니터
	double s_x1, s_x2, s_x3, s_x4;
	double s_y1, s_y2, s_y3, s_y4;

	if(quad_num == 0)
	{
		x1 = (double)gaze_point[0][0].x;
		y1 = (double)gaze_point[0][0].y;

		x2 = (double)gaze_point[0][1].x;
		y2 = (double)gaze_point[0][1].y;

		x3 = (double)gaze_point[1][0].x;
		y3 = (double)gaze_point[1][0].y;

		x4 = (double)gaze_point[1][1].x;
		y4 = (double)gaze_point[1][1].y;

		s_x1 = 0;
		s_y1 = 0;

		s_x2 = S_WIDTH/2;
		s_y2 = 0;

		s_x3 = 0;
		s_y3 = S_HEIGHT/2;

		s_x4 = S_WIDTH/2;
		s_y4 = S_HEIGHT/2;
	}
	else if(quad_num == 1)
	{
		x1 = (double)gaze_point[0][1].x;
		y1 = (double)gaze_point[0][1].y;

		x2 = (double)gaze_point[0][2].x;
		y2 = (double)gaze_point[0][2].y;

		x3 = (double)gaze_point[1][1].x;
		y3 = (double)gaze_point[1][1].y;

		x4 = (double)gaze_point[1][2].x;
		y4 = (double)gaze_point[1][2].y;

		s_x1 = S_WIDTH/2;
		s_y1 = 0;

		s_x2 = S_WIDTH;
		s_y2 = 0;

		s_x3 = S_WIDTH/2;
		s_y3 = S_HEIGHT/2;

		s_x4 = S_WIDTH;
		s_y4 = S_HEIGHT/2;
	}
	else if(quad_num == 2)
	{
		x1 = (double)gaze_point[1][0].x;
		y1 = (double)gaze_point[1][0].y;

		x2 = (double)gaze_point[1][1].x;
		y2 = (double)gaze_point[1][1].y;

		x3 = (double)gaze_point[2][0].x;
		y3 = (double)gaze_point[2][0].y;

		x4 = (double)gaze_point[2][1].x;
		y4 = (double)gaze_point[2][1].y;

		s_x1 = 0;
		s_y1 = S_HEIGHT/2;

		s_x2 = S_WIDTH/2;
		s_y2 = S_HEIGHT/2;

		s_x3 = 0;
		s_y3 = S_HEIGHT;

		s_x4 = S_WIDTH/2;
		s_y4 = S_HEIGHT;
	}
	else if(quad_num == 3)
	{
		x1 = (double)gaze_point[1][1].x;
		y1 = (double)gaze_point[1][1].y;

		x2 = (double)gaze_point[1][2].x;
		y2 = (double)gaze_point[1][2].y;

		x3 = (double)gaze_point[2][1].x;
		y3 = (double)gaze_point[2][1].y;

		x4 = (double)gaze_point[2][2].x;
		y4 = (double)gaze_point[2][2].y;

		s_x1 = S_WIDTH/2;
		s_y1 = S_HEIGHT/2;

		s_x2 = S_WIDTH;
		s_y2 = S_HEIGHT/2;

		s_x3 = S_WIDTH/2;
		s_y3 = S_HEIGHT;

		s_x4 = S_WIDTH;
		s_y4 = S_HEIGHT;
	}

	CvMat* M;
	CvMat* C;
	CvMat* inverse_C;

	M = cvCreateMat(nrow, ncol, CV_32FC1); // 모니터 꼭지점 행렬
	trans_matrix[quad_num] = cvCreateMat(nrow, ncol, CV_32FC1); // 변환 행렬
	C = cvCreateMat(nrow, ncol, CV_32FC1); // 동공 중심 좌표 행렬
	inverse_C = cvCreateMat(nrow, ncol, CV_32FC1); // C의 역행렬
	CvMat* inverse_T = cvCreateMat(nrow, ncol, CV_32FC1); // 변환 행렬

	// M
	cvmSet(M, 0, 0, s_x1); cvmSet(M, 0, 1, s_x2); cvmSet(M, 0, 2, s_x3); cvmSet(M, 0, 3, s_x4);
	cvmSet(M, 1, 0, s_y1); cvmSet(M, 1, 1, s_y2); cvmSet(M, 1, 2, s_y3); cvmSet(M, 1, 3, s_y4); 
	cvmSet(M, 2, 0, 0); cvmSet(M, 2, 1, 0); cvmSet(M, 2, 2, 0); cvmSet(M, 2, 3, 0);
	cvmSet(M, 3, 0, 0); cvmSet(M, 3, 1, 0); cvmSet(M, 3, 2, 0); cvmSet(M, 3, 3, 0);

	// C
	cvmSet(C, 0, 0, x1); cvmSet(C, 0, 1, x2); cvmSet(C, 0, 2, x3); cvmSet(C, 0, 3, x4);
	cvmSet(C, 1, 0, y1); cvmSet(C, 1, 1, y2); cvmSet(C, 1, 2, y3); cvmSet(C, 1, 3, y4);
	cvmSet(C, 2, 0, x1*y1); cvmSet(C, 2, 1,x2*y2); cvmSet(C, 2, 2, x3*y3); cvmSet(C, 2, 3, x4*y4);
	cvmSet(C, 3, 0, 1); cvmSet(C, 3, 1, 1); cvmSet(C, 3, 2, 1); cvmSet(C, 3, 3, 1);

	//cvInvert(C, inverse_C, cv::DECOMP_SVD);
	cvInvert(C, inverse_C, CV_LU); //Gaussian Elimination
	cvMatMul(M, inverse_C, trans_matrix[quad_num]);

	/*printf("\n< Transform Matrix(%d) >\n", quad_num);

	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			printf("%f  ", cvmGet(trans_matrix[quad_num], i, j));
		}
		printf("\n");
	}
	printf("\n\n");*/
}

CvPoint GazeMapping(CvMat* trans_matrix[4],CvPoint gaze_point[][3], CvPoint pos)
{
	double nrow = 4.0;
	double ncol = 4.0;

	CvMat* W;
	CvMat* G;

	W = cvCreateMat(nrow, 1, CV_32FC1); // 동영상 중심 좌표 행렬
	G = cvCreateMat(nrow, 1, CV_32FC1); // 응시점 행렬

	// W
	cvmSet(W, 0, 0, (double)pos.x);
	cvmSet(W, 1, 0, (double)pos.y);
	cvmSet(W, 2, 0, (double)pos.x * (double)pos.y);
	cvmSet(W, 3, 0, 1);

	if(pos.x < gaze_point[1][1].x && pos.y < gaze_point[1][1].y)
		cvMatMul(trans_matrix[0], W, G);
	else if(gaze_point[1][1].x <= pos.x && pos.y < gaze_point[1][1].y)
		cvMatMul(trans_matrix[1], W, G);
	else if(pos.x < gaze_point[1][1].x && gaze_point[1][1].y <= pos.y)
		cvMatMul(trans_matrix[2], W, G);
	else if(gaze_point[1][1].x <= pos.x && gaze_point[1][1].y <= pos.y)
		cvMatMul(trans_matrix[3], W, G);

	CvPoint result;
	result.x = cvRound(cvmGet(G, 0, 0));
	result.y = cvRound(cvmGet(G, 1, 0));

	return result;
}

void Calibration(CvMat* trans_matrix[4], CvPoint gaze_point[][3])
{
	IplImage *gaze_point_img, *screen_point_img;

	gaze_point_img = cvCreateImage(cvSize(WIDTH, HEIGHT), 8, 1);
	screen_point_img = cvCreateImage(cvSize(S_WIDTH, S_HEIGHT), 8, 1);

	cvZero(gaze_point_img);
	cvZero(screen_point_img);

	// Pupil Center Points(3 by 3)
	/*{
		gaze_point[0][0].x = 0;
		gaze_point[0][0].y = 0;

		gaze_point[0][1].x = WIDTH/2;
		gaze_point[0][1].y = 0;

		gaze_point[0][2].x = WIDTH;
		gaze_point[0][2].y = 0;

		gaze_point[1][0].x = 0;
		gaze_point[1][0].y = HEIGHT/2;

		gaze_point[1][1].x = WIDTH/2;
		gaze_point[1][1].y = HEIGHT/2;

		gaze_point[1][2].x = WIDTH;
		gaze_point[1][2].y = HEIGHT/2;

		gaze_point[2][0].x = 0;
		gaze_point[2][0].y = HEIGHT;

		gaze_point[2][1].x = WIDTH/2;
		gaze_point[2][1].y = HEIGHT;

		gaze_point[2][2].x = WIDTH;
		gaze_point[2][2].y = HEIGHT;
	}*/

	for(int i=0; i<4; i++)
		TransMatrix(trans_matrix, gaze_point, i);

	/*CvPoint in[5] = {0}, out[5] = {0};

	for(int i=0; i<5; i++)
	{
		in[i].x = rand()%WIDTH;
		in[i].y = rand()%HEIGHT;
		out[i] = GazeMapping(trans_matrix, gaze_point, in[i]);

		gaze_point_img->imageData[in[i].y * gaze_point_img->widthStep + in[i].x] = 255;
		screen_point_img->imageData[out[i].y * screen_point_img->widthStep + out[i].x] = 255;
	}

	cvShowImage("Gaze Point", gaze_point_img);
	cvShowImage("Screen Point", screen_point_img);*/
}