#include "Gay.hpp"


using namespace cv;
int main() {
	srand(time(0));
	
	Mat BG(Size(1600, 900), CV_8UC3, Scalar(0, 0, 0));
	Size sz_log(1200, 400);
	Size sz_entity(400, 400);
	Size sz_center(100, 100);
	Size sz_cam(250, 250);
	Rect log(Point(20, 20), Point(1220, 220));
	Rect Computer(Point(200, 400), Point(600, 800));
	Rect Player(Point(1000, 400), Point(1400, 800));
	Rect Center(Point(750, 550), Point(850, 650));
	Rect handDisplay(Point(1300, 20), Point(1550, 270));

	Size sz_btn(200, 100);
	Rect btn1(Point(700, 350), Point(900, 450));
	Rect btn2(Point(700, 200), Point(900, 300));

	/*Rect log(Point(20, 20), Point(220, 1220));
	Rect Computer(Point(200, 200), Point(700, 600));
	Rect Player(Point(700, 1000), Point(200, 1400));
	Rect Center(Point(350, 750), Point(450, 850));
	Rect handDisplay(Point(20, 1300), Point(270, 1550));*/
	
	GUI *app = new GUI(BG, sz_log, sz_entity, sz_center, sz_cam, log, Computer, Player, Center, handDisplay);
	app->addLog("> ooo!");
	app->addLog("> shit!");
	app->addLog("> ooo!");
	app->addLog("> shit!");
	app->addLog("> oo!");
	app->addLog("> tttt");
	
	app->matinit();

	Retarder timer = Retarder(1000);
	while (true) {
		app->showLog();
		app->display();

		if (waitKey(1) == 27) break;
	}
	
}