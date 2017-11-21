#pragma once
#include "Control.h"
#ifdef _WIN32
#include "opencv.hpp"
#else
#include <opencv2/opencv.hpp>
#endif
#include <opencv2/opencv.hpp>
/**
*
* @brief Contains a the base instance of a game
*
* @author Kevin Heyer
*
*/
class CBase4618
{
protected:
	cv::Mat *canvas;///<the canvas to draw to
public:

	/** @brief instantiates control and canvas
	*
	*/
	CBase4618();
	~CBase4618();

	/** @brief while loop calling update and draw. Use this to run game
	*
	*/
	void run();

	/** @brief overload this for updating background data
	*
	*/
	virtual void update();

	/** @brief overload this to draw to the canvas
	*
	*/
	virtual void draw();

};

