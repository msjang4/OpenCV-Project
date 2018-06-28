#include "Gay.hpp"




using namespace cv;


MYSQL * cons = mysql_init(NULL);
MYSQL_RES * sql_result;
MYSQL_ROW sql_row;
const char * host = "localhost";
const char *id = "root";
const char *pass = "1028tjdgns";
const char * dbname = "Open";
const char * tablename = "rank";

void insert_score(const char * name, int score) {
	char str[100] = "";
	sprintf(str, "select * from %s where name = %s;", tablename, name);
	mysql_query(cons, str);
	sql_result = mysql_store_result(cons);
	sql_row = mysql_fetch_row(sql_result);
	if (sql_row == 0) {
		sprintf(str, "insert into %s(name,score) values('%s',%d);", tablename, name, score);
		mysql_query(cons, str);
	}
	else {
		sprintf(str, "update %s set score = %d where name = '%s';", tablename, score, name);
		mysql_query(cons, str);
	}
}
void road_rank(int i, std::vector<std::string>& namev, std::vector<int>& scorev) {
	char str[100] = "";
	sprintf(str, "select * from %s.%s order by score desc;", dbname, tablename);
	mysql_query(cons, str);
	sql_result = mysql_store_result(cons);
	if (i <0) {
		while ((sql_row = mysql_fetch_row(sql_result))) {
			namev.push_back(sql_row[0]);//팝은 알아서 하라지~
			scorev.push_back(atoi(sql_row[1]));//팝은 알아서 하라지~
			std::cout << sql_row[0] << sql_row[1] << std::endl;
		}
	}
	else {
		while ((sql_row = mysql_fetch_row(sql_result))) {
			if (i == 0) {
				break;
			}
			namev.push_back(sql_row[0]);//팝은 알아서 하라지~
			scorev.push_back(atoi(sql_row[1]));//팝은 알아서 하라지~
			std::cout << sql_row[0] << sql_row[1] << std::endl;
			i--;
		}
	}
}

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

	std::vector<std::string> namevec;
	std::vector<int> scorevec;
	cons = mysql_real_connect(cons, host, id, pass, NULL, 3306, NULL, NULL);
	if (cons == 0) {
		std::cout << "연결 실패" << mysql_error(cons) << std::endl;
	}
	else
	{
		std::cout << "연결 성공" << std::endl;
		mysql_set_character_set(cons, "euckr");
		char str[100] = "";
		sprintf(str, "select * from %s.%s order by score desc;", dbname, tablename);
		std::cout << str;
		mysql_query(cons, str);
		sql_result = mysql_store_result(cons);
		while ((sql_row = mysql_fetch_row(sql_result))) {
			std::cout << sql_row[0] << sql_row[1] << std::endl;
		}
	}

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