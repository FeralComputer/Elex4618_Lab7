#pragma once

#ifdef _WIN32
#include "opencv.hpp"
#else
#include <opencv2/opencv.hpp>
#endif
class CText
{
private:

public:
	CText();
	~CText();
	static void draw(cv::Mat & canvas, cv::Point centerpoint, std::string text, double fontscale=1,int thickness=1, cv::Scalar colour=cv::Scalar::all(255));

};

