#include "stdafx.h"
#include "Base4618.h"


CBase4618::CBase4618()
{
	canvas = new cv::Mat();
}


CBase4618::~CBase4618()
{
}

void CBase4618::run()
{
	while (1)
	{
		switch (cv::waitKey(1))
		{
			case 'q':
			case 'Q':
				return;
			default:
				update();
				draw();
				break;
		}
	}
}

void CBase4618::update()
{
}

void CBase4618::draw()
{
}