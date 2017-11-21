#include "stdafx.h"
#include "Tracker.h"
#include <thread>
#include "Text.h"


const cv::Scalar CTracker::ORANGE_LOWER_BOUNDS = { 0,100,100 };
const cv::Scalar CTracker::ORANGE_UPPER_BOUNDS = { 60,255,255 };

const cv::Scalar CTracker::BLUE_LOWER_BOUNDS = { 40,150,150 };
const cv::Scalar CTracker::BLUE_UPPER_BOUNDS = { 120,255,255 };

const cv::Scalar CTracker::GREEN_LOWER_BOUNDS = { 120,150,150 };
const cv::Scalar CTracker::GREEN_UPPER_BOUNDS = { 180,255,255 };

int CTracker::update_thread(CTracker * tracker, bool * running)
{
	while (*running)
	{
		tracker->update();
	}
	return 0;
}

int CTracker::draw_thread(CTracker * tracker, bool * running)
{
	while (*running)
	{
		if (tracker->newframe)
		{
			tracker->newframe = false;
			tracker->draw();
		}
		
	}
	return 0;
}

void CTracker::controlServos(float dx, float dy)
{
	//servo 1 is 2.5 channel 0 (servo header)
	//servo 2 is 2.6 channel 1 

	//invert x

	//dx = 1 - dx;

	dx *= 60;
	dy *= 60;
	servo_x += dx;
	servo_y += dy;
	

	if (servo_x < SERVO_MIN_PERIOD)
	{
		servo_x = SERVO_MIN_PERIOD;
	}
	else if (servo_x > (SERVO_MIN_PERIOD + SERVO_GAIN))
	{
		servo_x = SERVO_MIN_PERIOD + SERVO_GAIN;
	}

	if (servo_y < SERVO_MIN_PERIOD)
	{
		servo_y = SERVO_MIN_PERIOD;
	}
	else if (servo_y >(SERVO_MIN_PERIOD + SERVO_GAIN))
	{
		servo_y = SERVO_MIN_PERIOD + SERVO_GAIN;
	}

	//std::cout << "\ndx: " << servo_x << " dy: " << servo_y;

#ifdef _WIN32
	serial->setData(CControl::ChannelType::SERVO, CControl::SERVO_PORTS::SERVO_RAW_1, servo_x);
	serial->setData(CControl::ChannelType::SERVO, CControl::SERVO_PORTS::SERVO_RAW_2, servo_y);
#else
	serial->setData(CControl::ChannelType::SERVO, SERVO_PIN_1, servo_x);
	serial->setData(CControl::ChannelType::SERVO, SERVO_PIN_2, servo_y);
#endif
}

CTracker::CTracker()
{
	serial = new CControl(comport);
	camera.open(CAMERA_NUMBER);
	selected_colour = COLOUR_SELECTION::ORANGE;
#ifdef _WIN32
	serial->setData(CControl::ChannelType::ANALOG, CControl::ANALOG_PORTS::SERVO_RAW_MIN, SERVO_MIN_PERIOD);
	serial->setData(CControl::ChannelType::ANALOG, CControl::ANALOG_PORTS::SERVO_RAW_MAX, SERVO_MAX_PERIOD);

	serial->setData(CControl::ChannelType::ANALOG, CControl::ANALOG_PORTS::LED_RED_RAW_MIN, SERVO_MIN_PERIOD);
	serial->setData(CControl::ChannelType::ANALOG, CControl::ANALOG_PORTS::LED_RED_RAW_MAX, SERVO_MAX_PERIOD);

	serial->setData(CControl::ChannelType::SERVO, CControl::SERVO_PORTS::SERVO_RAW_1, SERVO_MIN_PERIOD + SERVO_GAIN / 2);
	serial->setData(CControl::ChannelType::SERVO, CControl::SERVO_PORTS::SERVO_RAW_2, SERVO_MIN_PERIOD + SERVO_GAIN / 2);
	
#else
	//enter needed servo inputs (initializing)

#endif
}


CTracker::~CTracker()
{
	delete serial;
}

void CTracker::run()
{
	std::thread t1(&CTracker::update_thread, this, &isrunning);
	t1.detach();
	std::thread t2(&CTracker::draw_thread, this, &isrunning);
	t2.detach();
	std::string str;

	while (isrunning)
	{
		//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		//menu for colour selection and 
		printMenu();
		std::getline(std::cin, str);
		if (str.length() == 0)
		{
			continue;
		}
		switch (str[0])
		{
		case '1':
			//orange
			upperbounds = ORANGE_UPPER_BOUNDS;
			lowerbounds = ORANGE_LOWER_BOUNDS;
			selected_colour = COLOUR_SELECTION::ORANGE;
			break;

		case '2':
			//blue
			upperbounds = BLUE_UPPER_BOUNDS;
			lowerbounds = BLUE_LOWER_BOUNDS;
			selected_colour = COLOUR_SELECTION::BLUE;
			break;

		case '3':
			//green
			upperbounds = GREEN_UPPER_BOUNDS;
			lowerbounds = GREEN_LOWER_BOUNDS;
			selected_colour = COLOUR_SELECTION::GREEN;
			break;

		case '4':
			//custom colour
			selected_colour = COLOUR_SELECTION::CUSTOM;
			std::cout << "\nEnter lowerbounds values: ";
			setCustomColour(lowerbounds);
			std::cout << "\nEnter upperbounds values: ";
			setCustomColour(upperbounds);
			for (unsigned int i = 0; i < upperbounds.channels; i++)
			{
				if (lowerbounds[i] > upperbounds[i])
				{
					lowerbounds[i] = upperbounds[i];
					std::cout << "\n\tERROR: lowerbounds hsv values are greater than the upperbounds.... Great Job!";
				}
			}
			break;

		case 'q':
		case 'Q':
			isrunning = false;
			break;

		default:
			//do nothing
			break;
		}
	}
	//std::this_thread::sleep_for(std::chrono::milliseconds(10));
	camera.release();
}

void CTracker::update()
{
	auto time = std::chrono::steady_clock::now();
	auto endtime = time + UPS_Duration(1);

	canvasMutex.lock();
	//get new image
	if(camera.read(*canvas))
	{
		newframe=true;
	}
	else
	{
		return;
	}
	//camera >> *canvas;
	//newframe = true;
	//convert to hsv
	cv::Mat hsv;
	cv::cvtColor(*canvas, hsv, CV_BGR2HSV);
	canvasMutex.unlock();

	//erode
	cv::erode(hsv, hsv, cv::Mat());

	//dilate
	cv::dilate(hsv, hsv, cv::Mat());

	//filter colour
	cv::inRange(hsv, lowerbounds, upperbounds, hsv);

	//finding contours --destroys mat!!!
	objectvector.lock();
	cv::findContours(hsv, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
	objectvector.unlock();

	//find biggest object
	biggest = cv::Rect();
	rectfound = false;
	for (unsigned int i = 0; i < contours.size(); i++)
	{
		cv::Rect rect = boundingRect(contours.at(i));
		if (rect.size().area() > biggest.size().area())
		{
			if (rect.size().area() > OBJECT_MINIMUM_AREA)
			{
				biggest = rect;
				rectfound = true;
			}

		}
	}

	//servo control
	if (biggest.area()>OBJECT_MINIMUM_AREA)
	{
		int centerx = hsv.size().width / 2;//get camera resolution
		int centery = hsv.size().height / 2;
		//find difference between center of camera and center of biggest object
		float dx = (float)centerx - ((float)biggest.x + (float)biggest.width / 2.0f);
		float dy = ((float)centery - ((float)biggest.y + (float)biggest.height / 2.0f));
		//std::cout << "\ndx: " << dx << " dy: " << dy;
		dx /= (float)centerx;
		dy /= (float)centery;
		//std::cout << "\ndx: " << dx << " dy: " << dy;

		controlServos(dx,dy);
	}

	//cv::waitKey(1);
	std::this_thread::sleep_until(endtime);
	//must be last
	UPS_ACTUAL = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - time).count();
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void CTracker::draw()
{
	auto time = std::chrono::steady_clock::now();
	auto endtime = time + FPS_Duration(1);

	canvasMutex.lock();
	objectvector.lock();
	//draw object colour
	for (unsigned int i = 0; i < contours.size(); i++)
	{
		cv::drawContours(*canvas, contours, i, RectColour, -1, 8, hierarchy);
	}

	//draw main rectangle
	cv::rectangle(*canvas, biggest, RectColour);
	objectvector.unlock();
	//fps text
	std::string updates = "FPS: " + std::to_string((1000000000.0f) / ((float)UPS_ACTUAL));
	CText::draw(*canvas, FPS_Text_Location, updates, 0.5);
	//colour text

	switch (selected_colour)
	{
	case COLOUR_SELECTION::ORANGE:
		CText::draw(*canvas, Colour_Text_Location, "Orange", 0.5);
		break;

	case COLOUR_SELECTION::BLUE:
		CText::draw(*canvas, Colour_Text_Location, "Blue", 0.5);
		break;

	case COLOUR_SELECTION::GREEN:
		CText::draw(*canvas, Colour_Text_Location, "Green", 0.5);
		break;

	case COLOUR_SELECTION::CUSTOM:
		CText::draw(*canvas, Colour_Text_Location, "Custom", 0.5);
		break;

	default:
		CText::draw(*canvas, Colour_Text_Location, "Invalid Colour", 0.5);
		break;
	}

	//if found and area text
	if (rectfound)
	{
		CText::draw(*canvas, Found_Text_Location, "Object found", 0.5);
		std::string area = "Area: " + std::to_string(biggest.area());
		CText::draw(*canvas, Area_Text_Location, area, 0.5);
	}
	else
	{
		CText::draw(*canvas, Found_Text_Location, "Object not found", 0.5);
	}
	cv::imshow("Output", *canvas);
	canvasMutex.unlock();



	char c = cv::waitKey(1);
	switch (c)
	{
	case 'q':
	case 'Q':
		isrunning = false;
		break;
	default:
		break;
	}
	std::this_thread::sleep_until(endtime);
}

void CTracker::printMenu()
{
	std::cout << "\n1: Select Orange";
	std::cout << "\n2: Select Blue";
	std::cout << "\n3: Select Green";
	std::cout << "\n4: Select custom colour";
	std::cout << "\nq: Quit";
	std::cout << "\n\tinput: ";
}

void CTracker::setCustomColour(cv::Scalar & colour)
{
	std::string input;
	int temp;
	while (1)
	{
		std::cout << "\nPlease enter H value (0-179): ";
		std::getline(std::cin, input);
		if (isInt(input))
		{
			temp = atoi(input.c_str());
			if (temp >= 0 && temp <= 179)
			{
				colour[0] = temp;
				break;
			}
			else
			{
				std::cout << "\n\tOut of range....";
			}
		}
		else
		{
			std::cout << "\n\tDo you even know what a number is?";
		}

	}
	input = "";

	while (1)
	{
		std::cout << "\nPlease enter S value (0-255): ";
		std::getline(std::cin, input);
		if (isInt(input))
		{
			temp = atoi(input.c_str());
			if (temp >= 0 && temp <= 255)
			{
				colour[1] = temp;
				break;
			}
			else
			{
				std::cout << "\n\tOut of range....";
			}
		}
		else
		{
			std::cout << "\n\tDo you even know what a number is?";
		}

	}
	input = "";

	while (1)
	{
		std::cout << "\nPlease enter V value (0-255): ";
		std::getline(std::cin, input);
		if (isInt(input))
		{
			temp = atoi(input.c_str());
			if (temp >= 0 && temp <= 255)
			{
				colour[2] = temp;
				break;
			}
			else
			{
				std::cout << "\n\tOut of range....";
			}
		}
		else
		{
			std::cout << "\n\tDo you even know what a number is?";
		}

	}

}

bool CTracker::isInt(std::string str)
{
	if (!str.length() > 0)
	{
		return false;
	}
	for (unsigned int i = 0; i < str.length(); i++)
	{
		if (!isdigit(str[i]))
		{
			return false;
		}
	}
	return true;
}
