#include "header.h"
#include "video.h"
#include <time.h>
//#include "find_files.h"

void main(void)
{
	int select = 0; // 조건 선택
	CvMat* trans_matrix[4];
	CvPoint calib_point[3][3];
	FILE *ifp, *ifp2, *ofp, *ofp2;
	
	printf("-----------------------------------------------\n");
	printf("|  (1) 캘리브레이션 (2) 동영상 선택 (3) 종료  |\n");
	printf("-----------------------------------------------\n");

	srand((unsigned int)time(NULL));

	do {
		fflush(stdin);
		printf("조건 선택 : ");
		scanf("%d", &select);

		switch(select)
		{
		case 1:
			ofp = fopen("trans_matrix.txt", "wt");
			VideoPlay(trans_matrix, calib_point, true);
			for(int i=0; i<4; i++)
			{
				for(int j=0; j<4; j++) for(int k=0; k<4; k++) fprintf(ofp, "%f ", cvmGet(trans_matrix[i], j, k));
				fprintf(ofp, "\n");
			}
			fclose(ofp);

			ofp2 = fopen("calib_point.txt", "wt");
			for(int i=0; i<3; i++)
				for(int j=0; j<3; j++)
					fprintf(ofp2, "%d %d\n", calib_point[i][j].x, calib_point[i][j].y);
			fclose(ofp2);

			break;

		case 2:
			ifp = fopen("trans_matrix.txt", "r");
			double tmp;
			for(int i=0; i<4; i++)
			{
				trans_matrix[i] = cvCreateMat(4, 4, CV_32FC1);
				for(int j=0; j<4; j++)
				{
					for(int k=0; k<4; k++)
					{
						fscanf(ifp, "%lf", &tmp);
						cvmSet(trans_matrix[i], j, k, tmp);
					}
				}
			}
			fclose(ifp);

			ifp2 = fopen("calib_point.txt", "r");
			for(int i=0; i<3; i++)
				for(int j=0; j<3; j++)
					fscanf(ifp2, "%d %d", &calib_point[i][j].x, &calib_point[i][j].y);
			fclose(ifp2);

			VideoPlay(trans_matrix, calib_point, false);			
			break;

		default:
			break;
		}

	} while(select != 3);

	cvDestroyAllWindows();
}