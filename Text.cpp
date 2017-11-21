#include "stdafx.h"
#include "Text.h"


CText::CText()
{
}


CText::~CText()
{
}

void CText::draw(cv::Mat & canvas, cv::Point centerpoint, std::string text, double fontscale, int thickness,cv::Scalar colour)
{
	int baseline = 0;
	cv::Size size = cv::getTextSize(text, cv::FONT_HERSHEY_COMPLEX, fontscale, thickness, &baseline);
	//cv::Point textOrg((CPong::screensize - size.width) / 2, (size.height) * 2);

	putText(canvas, text, centerpoint, cv::FONT_HERSHEY_COMPLEX, fontscale, cv::Scalar::all(255), thickness, 8);

}
