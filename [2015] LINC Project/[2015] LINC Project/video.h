#include "header.h"

// 동영상 크기
#define WIDTH 640
#define HEIGHT 480

// 스크린 크기
#define S_WIDTH 640
#define S_HEIGHT 480

// 파일 이름 길이
#define FILENAME_LEN 50

// 동공 임계값의 누적값 제한
// #define THRESH_LIMITATION 2000000000

// 이미지 자르기
#define CROP_W_STRT 120
#define CROP_H_STRT 150
#define CROP_WIDTH 270
#define CROP_HEIGHT 200


// 대략적인 동공 영역 추출 시 잘못된 값이 획득되면 이전 영역 값으로 대체하기 위한 변수
//CvRect prev_rough_region;



// video_save.cpp 함수
IplImage*  Video(CvCapture* capture);
void VideoSave();

// select_files.cpp 함수
char (*FindVideos())[FILENAME_LEN];

void GrayToRGB(IplImage* src, IplImage* dst);

// video_play.cpp 함수
void VideoPlay(CvMat* trans_matrix[4], CvPoint calib_point[][3], bool calib);

// img_RoI.cpp 함수
IplImage* ROI(IplImage* img, CvRect roi);

// glint_threshold.cpp 함수
int GetGlintThresh(IplImage* img);

// temp_pupil_detection.cpp 함수(x)
CvPoint GetTempPupilCenter(IplImage* binary_img, int* tmp_pupil_diameter);

// temp_pupil_detection.cpp 함수 (x)
void remove_residual_binary_region(IplImage* binary);

// temp_pupil_detection.cpp 함수
CvRect RoughPupilRegion(IplImage* img);

// temp_pupil_detection.cpp 함수
void RemoveGlint(IplImage* img);

void TransMatrix(CvMat* trans_matrix[4], CvPoint gaze_point[][3], int quad_num);
CvPoint GazeMapping(CvMat* trans_matrix[4],CvPoint gaze_point[][3], CvPoint pos);
void Calibration(CvMat* trans_matrix[4], CvPoint gaze_point[][3]);