#pragma once
#include <string>
#include <opencv2\core\mat.hpp>
#include <opencv2\opencv.hpp>

class myMat {
	cv::Mat mat;
public:
	myMat(int r, int c) : mat(r, c, CV_64FC3) {}
	int row() { return mat.rows; }
	int col() { return mat.cols; }
	cv::Vec3d& at(int ro, int co) { return mat.at<cv::Vec3d>(ro, co); }
	void show(const std::string& name) { cv::imshow(name, mat); waitKey(1); }
	void waitKey(int delay = 0) { cv::waitKey(delay); }
	void save(const std::string& name) {
		cv::Mat res;
		mat.convertTo(res, CV_8UC3, 255);
		cv::imwrite(name, res);
	}
};

inline cv::Vec3d assign(cv::Vec3d& lh, const vec3& rh) {
	lh[0] = rh.b, lh[1] = rh.g, lh[2] = rh.r;
	return lh;
}