#include "header.h"
#include "video.h"

void SetRange(int* upper_range, int* lower_range, int tmp_pupil_diameter);
void TempContourDetection(IplImage* smooth_img, IplImage* contour_img, CvPoint t_center, CvPoint* left_contour, CvPoint* right_contour, int upper_range, int lower_range, int* l_cnt, int* r_cnt, CvRect rough_pupil);
void FilteringContour(IplImage* smooth_img, CvPoint* left_contour, CvPoint* right_contour, int l_cnt, int r_cnt, CvPoint t_center);
CvPoint GetFinalPupilCenter(CvPoint* left_contour, CvPoint* right_contour, CvPoint* final_left_contour, CvPoint* final_right_contour, int l_cnt, int r_cnt, int* left_num, int* right_num, CvBox2D* box, CvSize* size, bool* b_RANSAC, bool* b_find);

void DrawContour(IplImage* smooth_img, CvPoint* final_left_contour, CvPoint* final_right_contour, int left_num, int right_num);
void DrawPupilInfo(IplImage* result_img, CvPoint final_pupil_center, CvBox2D box, CvSize size);
void DrawPupilCenterLine(IplImage* gray_img, CvPoint center);

// Curve Fitting
void knot(int n, int c, int x[]);
void basis(int c,float t, int npts,int x[],float n[]);
void bspline(int npts,int k,int p1,float b[],float p[]);
void GetCurveFittedPoints(CvPoint* contour, CvPoint* fit_contour, int input_num, int output_num, int fit_size);

//»Ò¿Á
void RealContour(IplImage *img, CvPoint t_center);