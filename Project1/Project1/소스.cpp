//#include <iostream>
//#include <opencv2/opencv.hpp>
//#include <vector>
//using namespace std;
//using namespace cv;
//

//int main() {
//	Mat kernel2 = Mat::ones(Size(3, 3), CV_8U);
//	vector<vector<Point>> cont;
//	Scalar lo(0, 130, 101);
//	Scalar hi(198,155,148);
//	VideoCapture cap(1);
//	Mat frame,yc, mask, mask3, th,thh,res;
//	while (1) {
//		cap.read(frame); // ret, frame = cap.read()
//		CV_Assert(frame.data);
//
//		cvtColor(frame, yc, COLOR_BGR2YCrCb);
//		inRange(yc, lo, hi, mask);
//		GaussianBlur(mask, mask, Size(0,0),2);
//		morphologyEx(mask, mask,MORPH_CLOSE, kernel2,Point(-1,-1),  2);
//		bitwise_and(frame, frame, mask3, mask);
//		
//		threshold(mask, th, 60, 255, THRESH_BINARY + THRESH_OTSU);
//		th.copyTo(thh);
//		findContours(th, cont, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//	/*	res = Mat(frame.size(), CV_8U, cv::Scalar(255));
//		drawContours(res, cont,
//			-1,
//			cv::Scalar(0),
//			2);*/
//		vector<int> numb;
//
//		int cov = cont.size();
//
//		if (cov > 0) {
//			for (int j = 0; j < cov; j++) {
//				vector<Point> covt = cont[j];
//				double ar = contourArea(covt);
//				if (ar > 230) {
//					Moments M =moments(covt);
//					int x1= M.m10 / M.m00;
//					int y1 = M.m01 / M.m00;
//					numb.push_back(x1);
//					Point centroid = Point(x1, y1);
//					circle(frame, centroid, 3, Scalar(0, 24, 206), 3);
//
//					vector<Point> hull;
//					vector<Vec4i> defects;
//					Rect rect =boundingRect(covt);
//					try {
//						convexHull(covt, hull, false, false);
//						convexityDefects(covt, hull, defects);
//						for (int i = 0; i < defects.size(); i++) {
//						}
//					}
//					catch (exception e) {
//						continue;
//					}
//				}
//			}
//			drawContours(frame, cont, -1, (34, 223, 109), 3);
//		}
//		putText(frame, to_string(numb.size()));
//
//		imshow("kovi", mask3);
//		imshow("Hand detec",frame);
//		if (waitKey(30) >= 0)
//			break;
//	}
//}
#include <iostream>
#include <opencv2\opencv.hpp>

cv::Point getHandCenter(cv::Mat &mask, double &radius);
float fingerEnd(float p1, float p2, float t1, float t2, float q1, float q2);
void putText(cv::Mat& frame, std::string txt) {

	putText(frame, "Fingers:" + txt, cv::Point(0, 70), cv::FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2, CV_AA);
}

void printHand(cv::Mat &image, int fingers) {
	switch (fingers) {
	case 0:
	case 1:
		putText(image, "Rock");
		break;
	case 2:
	case 3:
		putText(image, "Scissors");
		break;
	default:
		putText(image, "Paper");

	}
}
std::vector<cv::Point> saveLine;

int erosion_value = 2;
int const max_erosion = 2;
int erosion_size = 0;
int const ersion_max_size = 21;
int dilation_value = 0;
int dilation_size = 0;
int erosion_type = 0;
int drawX, drawY;

int main()
{
	cv::Mat element;
	cv::Mat frame, tmpImg;
	cv::Mat handImg, mask, mask1;
	cv::Point dst;

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;

	double radius = 5;
	cv::VideoCapture video(0);

	if (!video.isOpened()) return 0;
	cv::Mat realImg;
	cv::namedWindow("change_image", CV_WINDOW_AUTOSIZE);
	cv::namedWindow("original_image", CV_WINDOW_AUTOSIZE);
	//cv::createTrackbar("ele_erosion", "original_image", &erosion_value, max_erosion);
	//cv::createTrackbar("erosion_size", "original_image", &erosion_size, ersion_max_size);
	//cv::createTrackbar("ele_dilation", "original_image", &dilation_value, max_erosion);
	//cv::createTrackbar("dilation_size", "original_image", &dilation_size, ersion_max_size);
	cv::Rect roi = cv::Rect(cv::Point(300, 100), cv::Size(200, 200));
	while (true)
	{
		video >> realImg;
		cv::flip(realImg, realImg, 1);	//영상 반전
		cv::rectangle(realImg, roi, cv::Scalar(0, 255, 0), 2);
		tmpImg = realImg(roi);
		GaussianBlur(tmpImg, tmpImg, cv::Size(5, 5), 0); //35,35
		if (tmpImg.empty()) break;

		if (erosion_value == 0) erosion_type = cv::MORPH_RECT;
		else if (erosion_value == 1) erosion_type = cv::MORPH_CROSS;
		else if (erosion_value == 2) erosion_type = cv::MORPH_ELLIPSE;

		element = cv::getStructuringElement(erosion_type, cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1), cv::Point(erosion_size, erosion_size));

		//손 색깔을 검출하기 위해서 YCbCr 컬러 모델 사용. 잡음에 강하다고..
		//Y는 휘도 성분, Cb, Cr은 색차 성분
		//피부색이 가지는 Cb,Cr의 영역은 
		//Cb: 77 ~ 127
		//Cr: 133 ~ 173
		cv::Scalar lo(0, 130 / 2 + 133 / 2, 101 / 2 + 77 / 2);
		cv::Scalar hi(198 / 2 + 255 / 2, 155 / 2 + 173 / 2, 148 / 2 + 127 / 2);
		cv::cvtColor(tmpImg, handImg, CV_BGR2YCrCb);
		cv::inRange(handImg, cv::Scalar(0, 133, 77), cv::Scalar(255, 173, 127), handImg);
		//	cv::inRange(handImg, lo, hi, handImg);
		//	cv::erode(handImg, handImg, element);
		//	cv::dilate(handImg, handImg, element);

		mask = handImg.clone();


		cv::findContours(mask, contours, hierarchy, CV_RETR_LIST, CV_CLOCKWISE, cv::Point(0, 0));



		int largestContour = 0;

		for (int i = 0; i < contours.size(); i++) {
			if (cv::contourArea(contours[i]) > cv::contourArea(contours[largestContour])) {
				largestContour = i;
			}

		}

		//가장 큰 영역하나를 잡는다.

		//	cv::drawContours(tmpImg, contours, largestContour, cv::Scalar(0, 255, 255), 1, 8, std::vector < cv::Vec4i>(), 0, cv::Point());	//contour 그리기

		if (!contours.empty()) {
			std::vector<std::vector<cv::Point>>hull(1);

			cv::convexHull(cv::Mat(contours[largestContour]), hull[0], false); // 모든 점을 포함할 수 있는 점들을 hull[0]에 저장
			cv::drawContours(tmpImg, hull, 0, cv::Scalar(0, 255, 0), 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());		//점 잇기

			if (hull[0].size() > 2) {

				cv::Rect boundingBox = cv::boundingRect(hull[0]);
				cv::Point center = cv::Point(boundingBox.x + boundingBox.width / 2, boundingBox.y + boundingBox.height / 2);

				std::vector<cv::Point> validPoints;

				std::vector<int> hullIndex;
				cv::convexHull(cv::Mat(contours[largestContour]), hullIndex); //hullIndex들에 점들 저장
				std::vector<cv::Vec4i> convexityDefects;

				cv::convexityDefects(cv::Mat(contours[largestContour]), hullIndex, convexityDefects);

				for (int i = 0; i < convexityDefects.size(); i++) {
					if (convexityDefects[i][3] > 13 * 256) {
						cv::Point p1 = contours[largestContour][convexityDefects[i][0]];
						cv::Point p2 = contours[largestContour][convexityDefects[i][1]];
						cv::Point p3 = contours[largestContour][convexityDefects[i][2]];
						//cv::line(tmpImg, p1, p3, cv::Scalar(255, 0, 0), 2);
						//cv::line(tmpImg, p3, p2, cv::Scalar(255, 0, 0), 2);

						double angle = std::atan2(center.y - p1.y, center.x - p1.x) * 180 / CV_PI;
						double inA = fingerEnd(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
						double length = std::sqrt(std::pow(p1.x - p3.x, 2) + std::pow(p1.y - p3.y, 2));

						//if (angle > -30 && angle < 160 && std::abs(inA) > 20 && std::abs(inA) < 120 && length > 0.1 * boundingBox.height)
						{
							validPoints.push_back(p1);
						}
					}
				}

				int maxY = 0, maxX = 0;

				printHand(realImg, validPoints.size());
				if (validPoints.size() != 0) {
					maxY = validPoints[0].y, maxX = validPoints[0].x;

					for (int i = 0; i < validPoints.size(); i++)
					{
						if (true || validPoints[i].y < maxY) {
							maxY = validPoints[i].y;
							maxX = validPoints[i].x;
						}
						cv::circle(tmpImg, cv::Point(maxX, maxY), 9, cv::Scalar(255, 0, 0), 2);

						saveLine.push_back(cv::Point(maxX, maxY));

						/*for (std::vector<int>::size_type i = 0; i < saveLine.size(); i++) {
						cv::line(tmpImg, saveLine[i], saveLine[i], cv::Scalar(255, 5, 222), 5);
						}*/

						if (cv::waitKey(10) == 32) {
							saveLine.clear();
						}
						maxY = 0; maxX = 0;
					}
				}
			}
		}
		cv::line(tmpImg, cv::Point(drawX, drawY), cv::Point(drawX, drawY), cv::Scalar(0, 0, 0), 3, 8, 0);

		//cv::flip(handImg, handImg, 1);	//영상 반전

		cv::imshow("change_image", handImg);
		cv::imshow("original_image", realImg);

		if (cv::waitKey(30) >= 0) {
			break;
		}
	}


	video.release();
	tmpImg.release();
	frame.release();

	cv::destroyAllWindows();

	return 0;
}

cv::Point getHandCenter(cv::Mat &mask, double &radius)
{
	int maxDst[2];

	cv::Mat dst;	//행렬 저장
	cv::distanceTransform(mask, dst, CV_DIST_L2, 5);

	//최솟값 필요 없음
	cv::minMaxIdx(dst, NULL, &radius, NULL, maxDst, mask);

	return cv::Point(maxDst[1], maxDst[0]);
}

float fingerEnd(float p1, float p2, float t1, float t2, float q1, float q2)
{
	//두점 사이 거리 계산하기 위해. 제곱근 계산하는 sqrt
	float dist1 = std::sqrt((p1 - q1)*(p1 - q1) + (p2 - q2)*(p2 - q2));
	float dist2 = std::sqrt((t1 - q1)*(t1 - q1) + (t2 - q2)*(t2 - q2));

	float aX, aY;
	float bX, bY;
	float cX, cY;

	cX = q1;
	cY = q2;

	if (dist1 < dist2) {
		bX = p1;
		bY = p2;
		aX = t1;
		aY = t2;
	}
	else {
		bX = t1;
		bY = t2;
		aX = p1;
		aY = p2;
	}

	float r1 = cX - aX;
	float r2 = cY - aY;
	float z1 = bX - aX;
	float z2 = bY - aY;

	float result = std::acos((z1*r1 + z1 * r2) / (std::sqrt(z1*z1 + z2 * z2) * std::sqrt(r1*r1 + r2 * r2)));

	result = result * 180 / CV_PI;

	return result;
}

/*
Ahmet Yaylalioglu
10.02.2017
*/
//
//#include <stdio.h>
//#include <iostream>
//#include <opencv2\core\core.hpp>
//#include <opencv2\highgui\highgui.hpp>
//#include <opencv2\core\core.hpp>
//#include <opencv2\imgproc\imgproc.hpp>
//#include <opencv2\features2d\features2d.hpp>
//#include <opencv2\opencv.hpp> //blur efekti için
//#include <opencv2/video/background_segm.hpp>
//
//using namespace cv;
//using namespace std;
//
//
//void track(int, void*);
//Mat orjinalGoruntu;
//Mat fgMaskMOG2;
//Mat griGoruntu, kirpik, or2, kenarlar, aynali;
//int thresh = 140, maxVal = 255;
//int type = 1, deger = 8;
//
//
//int main() {
//
//
//
//	Ptr< BackgroundSubtractor> pMOG2;
//	pMOG2 = new BackgroundSubtractorMOG2();
//	cv::Rect myRoi(288, 12, 288, 288);
//	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3), Point(1, 1));
//	Mat frame;
//	Mat resizeF;
//	VideoCapture cap;
//	cap.open(0);
//	while (1)
//	{
//		Mat aynali2;
//		cap.read(orjinalGoruntu);
//		cv::flip(orjinalGoruntu, aynali, 1);
//		cv::rectangle(orjinalGoruntu, cv::Point(300, 300), cv::Point(12, 12), cv::Scalar(0, 0, 255));
//		cv::rectangle(aynali, myRoi, cv::Scalar(0, 0, 255));
//		kirpik = aynali(myRoi);
//		cvtColor(kirpik, griGoruntu, CV_RGB2GRAY);
//		equalizeHist(griGoruntu, griGoruntu);
//		GaussianBlur(griGoruntu, griGoruntu, Size(23, 23), 0); //35,35
//															   threshold(griGoruntu, or2, thresh, maxVal, THRESH_OTSU + CV_THRESH_BINARY_INV);
//		namedWindow("ayarla", CV_WINDOW_AUTOSIZE);
//		createTrackbar("Esik", "ayarla", &thresh, 250, track);
//		createTrackbar("Maksimum", "ayarla", &maxVal, 255, track);
//		createTrackbar("Esik Tipi", "ayarla", &type, 4, track);
//		createTrackbar("Kenarlar", "ayarla", &deger, 100, track);
//		pMOG2->operator()(kirpik, fgMaskMOG2);
//		cv::rectangle(fgMaskMOG2, myRoi, cv::Scalar(0, 0, 255));
//
//		cv::flip(fgMaskMOG2, aynali2, 1);
//
//		track(0, 0);
//		imshow("ORJINAL Goruntu", aynali);
//		imshow("ArkaPlan Kaldırıldı", fgMaskMOG2);
//		imshow("Gri", griGoruntu);
//
//
//
//
//		char key = waitKey(24);
//		if (key == 27) break;
//	}
//
//	return 0;
//}
//
//
//
//void track(int, void*) {
//	int count = 0;
//	char a[40];
//	vector<vector<Point> > contours;
//	vector<Vec4i> hierarchy;
//	threshold(fgMaskMOG2, or2, thresh, maxVal, type);
//	GaussianBlur(fgMaskMOG2, fgMaskMOG2, Size(11, 11), 3.5, 3.5);
//	threshold(fgMaskMOG2, or2, 10, 255, THRESH_OTSU);
//	GaussianBlur(fgMaskMOG2, fgMaskMOG2, Size(27, 27), 3.5, 3.5);
//	threshold(fgMaskMOG2, fgMaskMOG2, thresh, maxVal, type); //THRESH_BINARY + THRESH_OTSU
//															 Canny(or2, kenarlar, deger, deger * 2, 3);
//	Canny(fgMaskMOG2, kenarlar, deger, deger * 2, 3); //OR2
//	findContours(fgMaskMOG2, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0)); //OR2
//	Mat cizim = Mat::zeros(kenarlar.size(), CV_8UC3); //kenarlar.size() or2.size()
//	if (contours.size() > 0) {
//		size_t indexOfBiggestContour = -1;
//		size_t sizeOfBiggestContour = 0;
//		for (size_t i = 0; i < contours.size(); i++) {
//			if (contours[i].size() > sizeOfBiggestContour) {
//				sizeOfBiggestContour = contours[i].size();
//				indexOfBiggestContour = i;
//			}
//		}
//		vector<vector<int> >hull(contours.size());
//		vector<vector<Point> >hullPoint(contours.size()); //elin hareketine göre eli çevreleyen çokgen	
//		vector<vector<Vec4i> > defects(contours.size()); //parmak uclarindaki yesil noktalar..multi dimensional matrix
//		vector<vector<Point> >defectPoint(contours.size()); //point olarak parmak ucu noktalarýný x,y olarak tutuyor
//		vector<vector<Point> >contours_poly(contours.size()); //eli çevreleyen hareketli dikdörtgen		
//		Point2f rect_point[4];
//		vector<RotatedRect>minRect(contours.size());
//		vector<Rect> boundRect(contours.size());
//		for (size_t i = 0; i<contours.size(); i++) {
//			if (contourArea(contours[i])>5000) {
//				convexHull(contours[i], hull[i], true);
//				convexityDefects(contours[i], hull[i], defects[i]);
//				if (indexOfBiggestContour == i) {
//					minRect[i] = minAreaRect(contours[i]);
//					for (size_t k = 0; k<hull[i].size(); k++) {
//						int ind = hull[i][k];
//						hullPoint[i].push_back(contours[i][ind]);
//					}
//					count = 0;
//
//					for (size_t k = 0; k<defects[i].size(); k++) {
//						if (defects[i][k][3]>13 * 256) {
//							int p_start = defects[i][k][0];
//							int p_end = defects[i][k][1];
//							int p_far = defects[i][k][2];
//							defectPoint[i].push_back(contours[i][p_far]);
//							circle(griGoruntu, contours[i][p_end], 3, Scalar(0, 255, 0), 2); //i ydi
//							count++;
//						}
//
//					}
//
//					if (count == 1)
//						strcpy_s(a, "1");
//					else if (count == 2)
//						strcpy_s(a, "2");
//					else if (count == 3)
//						strcpy_s(a, "3");
//					else if (count == 4)
//						strcpy_s(a, "4");
//					else if (count == 5 || count == 6)
//						strcpy_s(a, "5");
//					else
//						strcpy_s(a, "EL GOSTER");
//
//					putText(aynali, a, Point(75, 450), CV_FONT_HERSHEY_SIMPLEX, 3, Scalar(0, 255, 0), 3, 8, false);
//
//					drawContours(cizim, contours, i, Scalar(255, 255, 0), 2, 8, vector<Vec4i>(), 0, Point());
//					drawContours(cizim, hullPoint, i, Scalar(255, 255, 0), 1, 8, vector<Vec4i>(), 0, Point());
//					drawContours(griGoruntu, hullPoint, i, Scalar(0, 0, 255), 2, 8, vector<Vec4i>(), 0, Point());
//					approxPolyDP(contours[i], contours_poly[i], 3, false);
//					boundRect[i] = boundingRect(contours_poly[i]);
//					rectangle(griGoruntu, boundRect[i].tl(), boundRect[i].br(), Scalar(255, 0, 0), 2, 8, 0);
//					minRect[i].points(rect_point);
//					for (size_t k = 0; k<4; k++) {
//						line(griGoruntu, rect_point[k], rect_point[(k + 1) % 4], Scalar(0, 255, 0), 2, 8);
//					}
//
//				}
//			}
//
//		}
//
//	}
//
//
//	imshow("Sonuc", cizim);
//
//}