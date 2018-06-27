#pragma once
#include <iostream>
#include <opencv2\opencv.hpp>
#include <queue>

using namespace std;
using namespace cv;

int getInfo(VideoCapture &video);
extern int finger;

uchar bilinear_value(Mat img, double x, double y);

void scaling_bilinear(Mat img, Mat& dst, Size size);

class Retarder
{
	long time;

	long nextTimeout;

public:
	Retarder(long time) : time(time)
	{
		this->restart();
	}

	void setTime(long time)
	{
		this->time = time;
		this->restart();
	}

	bool expired()
	{
		if (clock() > this->nextTimeout)
		{
			this->restart();

			return true;
		}

		return false;
	}

	void restart(int addDelay = 0)
	{
		this->nextTimeout = clock() + this->time + addDelay;
	}
};

class GUI {
private:
	Mat BG;
	Size sz_log;
	Size sz_entity;
	Size sz_center;
	Size sz_cam;
	Rect log;
	Rect Computer;
	Rect Player;
	Rect Center;
	Rect handDisplay;
	string PATH = "./resources/";
	Mat mat_Computer, mat_Player, mat_Center, mat_handDisplay, mat_log;
	Mat t1, t2, t3, t4, t5;
	Mat s1, s2, s3, s4, s5;
	queue<string> buf;
	int finger;
	Mat img_R, img_P, img_S;
	bool isStart = true;
	int compu;
	Retarder *timer;
	int count = 0;
	int res_pl = 0;


	VideoCapture video;

public:
	GUI(Mat BG, Size sz1, Size sz2, Size sz3, Size sz4, Rect r1, Rect r2, Rect r3, Rect r4, Rect r5) {
		this->BG = BG;
		this->sz_log = sz1;
		this->sz_entity = sz2;
		this->sz_center = sz3;
		this->sz_cam = sz4;
		this->log = r1;
		this->Computer = r2;
		this->Player = r3;
		this->Center = r4;
		this->handDisplay = r5;
		video = VideoCapture(0);
		timer = new Retarder(1000);
	
	}

	void matinit() {
		t1 = imread(PATH + "IMG_PL.jpg", IMREAD_COLOR);
		t2 = imread(PATH + "IMG_COM.jpg", IMREAD_COLOR);
		t3 = imread(PATH + "rock.png", IMREAD_COLOR);
		t4 = imread(PATH + "paper.png", IMREAD_COLOR);
		t5 = imread(PATH + "scissor.png", IMREAD_COLOR);

		
		resize(t1, mat_Computer, sz_entity);
		resize(t2, mat_Player, sz_entity);
		resize(t3, img_R, sz_entity);
		resize(t4, img_P, sz_entity);
		resize(t5, img_S, sz_entity);
	
		mat_Center = Mat(100, 100, CV_8UC3, Scalar(0, 0, 0));
		putText(mat_Center, "VS", Point(0, 90), FONT_HERSHEY_PLAIN, 4, Scalar(50, 250, 50), 3);

		mat_handDisplay = Mat(250, 250, CV_8UC3, Scalar(0, 0, 0));
		mat_log = Mat(200, 1200, CV_8UC3, Scalar(0, 0, 0));



	}
	void display() {


		if (count == 3 && isStart) {
			if(res_pl == compu){
				addLog("> draw!");
			}
			else if ((res_pl + 1) % 3 == compu) {
				addLog("> Lose!");
			}
			else {
				addLog("> Win!");

			}
			isStart = false;
			count = 0;
		}

		if (isStart) {
			if (timer->expired()) {
				count++;
			}
			compu = rand() % 3;
			switch (compu) {
			case 0: mat_Computer = img_S; break;
			case 1: mat_Computer = img_R; break;
			case 2: mat_Computer = img_P; break;
				
			}
			finger = getInfo(video);
			switch (finger) {
			case -1: addLog("> Error : Can't detect Camera");
				addLog("> Please Check your Camera.");
				break;
			case 0:
			case 1: mat_Player = img_R; res_pl = 1; break;
			case 2:
			case 3: mat_Player = img_S; res_pl = 0; break;
			default: mat_Player = img_P; res_pl = 2;
			}
		}
		else {
			if (timer->expired()) {
				isStart = true;
			}
		}

		Render(mat_Computer, BG, Computer, s1);
		Render(mat_Player, BG, Player, s2);
		Render(mat_Center, BG, Center, s3);
		Render(mat_handDisplay, BG, handDisplay, s4);

		Render(mat_log, BG, log, s5);
		

		imshow("wow", BG);
		

	}
	void Render(Mat mat, Mat &dst, Rect size, Mat &sub) {
		sub = dst(size);
		mat.copyTo(sub);
	}

	void addLog(string str) {
		if (buf.size() < 4) {
			buf.push(str);
		}
		else {
			buf.pop();
			buf.push(str);
		}
	}

	void showLog() {  
		s5 = Scalar(0, 0, 0);
		mat_log = Scalar(0, 0, 0);
		int length = buf.size();
		for (int i = 0; i < length; i++) {
			string str = buf.front();
			
			putText(mat_log, str, Point(5, 30 + i * 50), FONT_HERSHEY_PLAIN, 2, Scalar(255,255,255), 2);

			buf.pop();
			buf.push(str);
		}
	}

};