﻿
#include "Gay.hpp"

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


int getInfo(cv::VideoCapture &video)
{
	cv::Mat element;
	cv::Mat frame, tmpImg;
	cv::Mat handImg, mask, mask1;
	cv::Point dst;
	int finger = 0;

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;

	double radius = 5;

	if (!video.isOpened()) return -1;
	cv::Mat realImg;
	cv::namedWindow("change_image", CV_WINDOW_AUTOSIZE);
	cv::namedWindow("original_image", CV_WINDOW_AUTOSIZE);

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
						double length = std::sqrt(std::pow(p1.x - p3.x, 2) + std::pow(p1.y - p3.y, 2));

						//if (angle > -30 && angle < 160 && std::abs(inA) > 20 && std::abs(inA) < 120 && length > 0.1 * boundingBox.height)
						{
							validPoints.push_back(p1);
						}
					}
				}

				int maxY = 0, maxX = 0;

				printHand(realImg, validPoints.size());
				finger = validPoints.size();
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

		tmpImg.release();
		frame.release();
		return finger;
		
	}


	

	cv::destroyAllWindows();

	
}



