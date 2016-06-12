#include <opencv2/opencv.hpp>
#include <opencv\highgui.h>
#include <iostream>
#include <cstdlib>
#include "common.h"
#include "raytracer.h"
using namespace cv;
using namespace std;

const int HEIGHT = 600;
const int WIDTH = 800;

//Mat screen(600, 800, CV_8UC3);

int main() {
	myMat target(HEIGHT, WIDTH);
	Engine tracer(target);
	cout << "starring rendering wolegecao\n";
	tracer.render();
	cout << "rendered\n";

	target.show("real time render");
	waitKey(0);
	//cout << PI << endl;
	//system("pause");
	return 0;
}