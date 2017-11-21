#pragma once
#include "Serial.h"
#include "Packet.h"

/**
*
* @brief Wrapper for sending packets to and from a comport
*
* @author Kevin Heyer
*
*/
class CControl
{
private:
#ifdef _WIN32
	Serial *serial;//<serial port being written to
	
#else
	int handle;
	int BAUDRATE = 200000;
	
#endif
	std::string comport;//<comport location
	/** @brief sends a packet of data to the comport
	*
	* @param data the packet of data to be sent
	*/
	bool sendData(CPacket data);

	/** @brief receives a packet of data from the comport
	*
	* @param data the packet of data for data to be received into
	*/
	bool receiveData(CPacket &data);
	
	int getValue(int handle,char channel);
public:
	static const int PORTTIMEOUT = 1000;///<timeout of port in ms
#ifdef _WIN32
	static const int ADCRESOLUTION = 16384;///<resolution of uc 
#else
	static const int ADCRESOLUTION = 1024;///<resolution of adc connected to pi
	
#endif
	//digital	

	/** @brief Digital port codes for communication with msp432
	*
	*/
	typedef enum
	{
		UNDEFINED,
		PUSHBUTTON_A,
		PUSHBUTTON_B,
		PUSHBUTTON_JOYSTICK,
		LED1,					//red
		LED_RGB_RED,
		LED_RGB_GREEN,
		LED_RGB_BLUE
	}DIGITAL_PORTS;

	//ANALOG	

	/** @brief Analog port codes for communication with msp432
	*
	*/
	typedef enum
	{
		UNKNOWN,
		JOYSTICK_X_RAW,
		JOYSTICK_Y_RAW,
		GYRO_X_RAW,
		GYRO_Y_RAW,
		GYRO_Z_RAW,
		JOYSTICK_X_SCALED,
		JOYSTICK_Y_SCALED,
		SERVO_RAW_MIN,
		SERVO_RAW_MAX,
		LED_RED_RAW_MIN,
		LED_RED_RAW_MAX,
		LED_GREEN_RAW_MIN,
		LED_GREEN_RAW_MAX,
		LED_BLUE_RAW_MIN,
		LED_BLUE_RAW_MAX
	}ANALOG_PORTS;
	/*
	1 - Raw A/D from Joystick X.
2 - Raw A/D from Joystick Y.
3 - Raw A/D from Gyro X.
4 - Raw A/D from Gyro Y.
5 - Raw A/D from Gyro Z.
6 - Scaled value for Joystick X (0-100).
7 - Scaled value for Joystick Y (0-100).
8 - Servo raw minimum.
9 - Servo raw maximum.
10 - Red LED raw minimum.
11 - Red LED raw maximum.
12 - Green LED raw minimum.
13 - Green LED raw maximum.
14 - Blue LED raw minimum.
15 - Blue LED raw maximum.
	*/

	/** @brief Servo port codes for communication with msp432
	*
	*/
	/*
101 - P2.5 - Raw - Servo connector.
102 - P2.6 - Raw - Red LED (Booster).
103 - P2.4 - Raw - Green LED (Booster).
104 - P2.6 - Raw - Blue LED (Booster).
	*/
	typedef enum
	{
		SERVO_1 = 1,
		SERVO_2 = 2,
		SERVO_RAW_1 = 101,
		SERVO_RAW_2 = 102
		
	}SERVO_PORTS;

	/** @brief NOT IMPLEMENTED state machine for packets, shouldnt be in here if used
	*
	*/
	typedef enum
	{
		PACKET_STATE_WAIT_FOR_START,	//$ start byte
		PACKET_STATE_COMMAND,			//G,S,A get set
		PACKET_STATE_SPACE1,			//space
		PACKET_STATE_TYPE,				//D,A,S type of input	
		PACKET_STATE_SPACE2,			//space 
		PACKET_STATE_CHANNEL,			//channel
		PACKET_STATE_SPACE3,			//space
		PACKET_STATE_VALUE,				//value
		PACKET_STATE_RETURNCHAR			//return byte
	}PACKET_STATE;

	/** @brief control types for communication with msp432.
	*	
	*
	*/
	typedef enum  {
		DIGITAL='D',
		ANALOG='A',
		SERVO='S'
	}ChannelType;

	/** @brief instantiates the serial port
	*
	* @param comport "COM#" string containing the com port location
	*/
	CControl(std::string comport);

	/** @brief closes com port when done
	*
	*/
	~CControl();

	/** @brief gets data from com port
	*
	* @param type <D,A,S> the channel type
	*
	* @param channel channel number use ANALOG_PORTS or DIGITAL_PORTS
	*
	* @param value location of where to return value received from port
	*
	* @return true if succesfully received data and send and receive packets are equal
	*/
	bool getData(int type, int channel, int &value);

	/** @brief sets data from com port
	*
	* @param type <D,A,S> the channel type
	*
	* @param channel channel number use ANALOG_PORTS or DIGITAL_PORTS
	*
	* @param value parameter to send to port
	*
	* @return true if succesfully received data and send and receive packets are equal
	*/
	bool setData(int type, int channel, int value);

	/** @brief returns com port
	*
	* @return string in form of "COM#"
	*/
	std::string getComPort();
};

