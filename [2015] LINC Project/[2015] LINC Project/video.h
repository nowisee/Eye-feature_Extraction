#include "header.h"

// ������ ũ��
#define WIDTH 640
#define HEIGHT 480

// ��ũ�� ũ��
#define S_WIDTH 640
#define S_HEIGHT 480

// ���� �̸� ����
#define FILENAME_LEN 50

// ���� �Ӱ谪�� ������ ����
// #define THRESH_LIMITATION 2000000000

// �̹��� �ڸ���
#define CROP_W_STRT 120
#define CROP_H_STRT 150
#define CROP_WIDTH 270
#define CROP_HEIGHT 200


// �뷫���� ���� ���� ���� �� �߸��� ���� ȹ��Ǹ� ���� ���� ������ ��ü�ϱ� ���� ����
//CvRect prev_rough_region;



// video_save.cpp �Լ�
IplImage*  Video(CvCapture* capture);
void VideoSave();

// select_files.cpp �Լ�
char (*FindVideos())[FILENAME_LEN];

void GrayToRGB(IplImage* src, IplImage* dst);

// video_play.cpp �Լ�
void VideoPlay(CvMat* trans_matrix[4], CvPoint calib_point[][3], bool calib);

// img_RoI.cpp �Լ�
IplImage* ROI(IplImage* img, CvRect roi);

// glint_threshold.cpp �Լ�
int GetGlintThresh(IplImage* img);

// temp_pupil_detection.cpp �Լ�(x)
CvPoint GetTempPupilCenter(IplImage* binary_img, int* tmp_pupil_diameter);

// temp_pupil_detection.cpp �Լ� (x)
void remove_residual_binary_region(IplImage* binary);

// temp_pupil_detection.cpp �Լ�
CvRect RoughPupilRegion(IplImage* img);

// temp_pupil_detection.cpp �Լ�
void RemoveGlint(IplImage* img);

void TransMatrix(CvMat* trans_matrix[4], CvPoint gaze_point[][3], int quad_num);
CvPoint GazeMapping(CvMat* trans_matrix[4],CvPoint gaze_point[][3], CvPoint pos);
void Calibration(CvMat* trans_matrix[4], CvPoint gaze_point[][3]);