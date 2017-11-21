#pragma once
#include "Base4618.h"
#include <chrono>
#include <mutex>


class CTracker : public CBase4618
{
private:
	std::mutex canvasMutex, objectvector;


	bool isrunning = true;
	bool newframe = false;
	static const int UPS = 20;
	static const int FPS = 20;

	//actual update per second
	long long UPS_ACTUAL = 0;

	//servo control
	std::string comport = "COM5";
	CControl *serial;

	//hsv values for finding objects
	cv::Scalar upperbounds = { 40,255,255 };
	cv::Scalar lowerbounds = { 0,150,150 };

	//cv::Scalar upperbounds = { 90,255,255 };
	//cv::Scalar lowerbounds = {50,150,150};

	//colour for drawing over image
	cv::Scalar RectColour = { 255, 255, 255 };

	//for finding contours
	std::vector<cv::Vec4i> hierarchy;
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Point> contour;
	cv::Rect biggest;
	bool rectfound = false;

	//thread functions
	static int update_thread(CTracker *pong, bool *running);
	static int draw_thread(CTracker *pong, bool *running);

	//controls servos
	void controlServos(float dx, float dy);

	//selected color
	int selected_colour;

	//text location
	cv::Point FPS_Text_Location = { 0, 15 };
	cv::Point Colour_Text_Location = { 0, 30 };
	cv::Point Found_Text_Location = { 0, 45 };
	cv::Point Area_Text_Location = { 0, 60 };

	//camera
	cv::VideoCapture camera;

	//servo position
	float servo_x=2400;
	float servo_y=2400;

public:

#ifdef _WIN32
	static const int SERVO_MAX_PERIOD = 240000;
	static const int SERVO_MIN_PERIOD = 1200;
	static const int SERVO_GAIN = 2400;
#else
	static const int SERVO_MAX_PERIOD = 200000;
	static const int SERVO_MIN_PERIOD = 1000;
	static const int SERVO_GAIN = 1000;

	static const int SERVO_PIN_1 = 16;
	static const int SERVO_PIN_2 = 20;
#endif

	static const int OBJECT_MINIMUM_AREA = 1000;
	static const int CAMERA_NUMBER = 0;

	typedef std::chrono::duration<int, std::ratio<1, FPS>> FPS_Duration;
	typedef std::chrono::duration<int, std::ratio<1, UPS>> UPS_Duration;

	static const cv::Scalar ORANGE_LOWER_BOUNDS;
	static const cv::Scalar ORANGE_UPPER_BOUNDS;

	static const cv::Scalar BLUE_LOWER_BOUNDS;
	static const cv::Scalar BLUE_UPPER_BOUNDS;

	static const cv::Scalar GREEN_LOWER_BOUNDS;
	static const cv::Scalar GREEN_UPPER_BOUNDS;

	typedef enum {
		ORANGE,
		BLUE,
		GREEN,
		CUSTOM
	}COLOUR_SELECTION;

	CTracker();
	~CTracker();
	void run();
	void update();
	void draw();
	void printMenu();
	void setCustomColour(cv::Scalar &colour);
	bool isInt(std::string str);
};

