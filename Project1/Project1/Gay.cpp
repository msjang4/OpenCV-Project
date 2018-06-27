#include "Gay.hpp"



uchar bilinear_value(Mat img, double x, double y) {
	if (x >= img.cols - 1)x--;
	if (y >= img.rows - 1)y--;

	Point pt((int)x, (int)y);
	int A = img.at<uchar>(pt);            // 좌측상단점
	int B = img.at<uchar>(pt + Point(0, 1)); //좌측하단점
	int C = img.at<uchar>(pt + Point(1, 0));  //우측상단점
	int D = img.at<uchar>(pt + Point(1, 1));  //우측하단점

	double alpha = y - pt.y;
	double beta = x - pt.x;
	int M1 = A + (int)cvRound(alpha*(B - A));
	int M2 = C + (int)cvRound(alpha*(D - C));
	int P = M1 + (int)cvRound(beta*(M2 - M1));
	return saturate_cast<uchar>(P);
}
void scaling_bilinear(Mat img, Mat& dst, Size size) {
	dst = Mat(size, CV_8U, Scalar(0));
	double ratioY = (double)size.height / img.rows;
	double ratioX = (double)size.width / img.cols;
	for (int i = 0; i < dst.rows; i++) {
		for (int j = 0; j < dst.cols; j++) {
			double y = (i / ratioY);
			double x = (j / ratioX);
			dst.at<uchar>(i, j) = bilinear_value(img, x, y);
		}
	}
}