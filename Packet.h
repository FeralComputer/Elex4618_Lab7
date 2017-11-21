#pragma once
#include <string>

/**
*
* @brief creates a packet for data communication
*
*
* @author Kevin Heyer
*
*/
class CPacket
{
private:
	char command;///< <G,S,A,W> what the packet does
	char type;///< <D,A,S> access digital/analog/servo
	int channel;///< which address to write to use ANALOG_PORTS/DIGITAL_PORTS in CControl
	int value;///< the value from or for the channel

public:
	static const int TOKENIZEDPACKETSIZE = 4;///<size of tokenized packet

	/** @brief use this receiving packets of data must call parseString to enter data
	*
	*/
	CPacket();

	/** @brief create packet for getting data
	*
	* @param command <G,S,A,W> action for packet to do
	*
	* @param type <D,A,S> the channel type
	*
	* @param channel channel number use ANALOG_PORTS or DIGITAL_PORTS
	*/
	CPacket(char command, char type, int channel);

	/** @brief create packet for setting data
	*
	* @param command <G,S,A,W> action for packet to do
	*
	* @param type <D,A,S> the channel type
	*
	* @param channel channel number use ANALOG_PORTS or DIGITAL_PORTS
	*
	* @param value parameter to send to port
	*/
	CPacket(char command, char type, int channel, int value);

	~CPacket();
	
	/** @brief creates a string from command, type, channel, and value(if 'S') to send through com port
	*
	* @return string in form to send through com port
	*/
	std::string createString();
	
	/** @brief parses string and seperates to command, type, channel, and value
	*
	* @param data the string from com port
	*
	* @return true if parsed succesfully and completely, false otherwise
	*/
	bool parseString(std::string data);

	/** @brief checks if string is safe to convert to int
	*
	* @param str the string checked to see if safe to convert
	*
	* @return true if safe to convert to number, false otherwise
	*/
	bool checkForValidInt(std::string str);

	/** @brief use for comparing sent and received data to check for data integrity
	*
	* @param rhs what to compare with
	*/
	bool operator ==(const CPacket &rhs);

	/** @brief returns command
	*
	* @return command assigned to packet
	*/
	char getCommand();

	/** @brief returns type
	*
	* @return type assigned to packet
	*/
	char getType();

	/** @brief returns channel
	*
	* @return channel assigned to packet
	*/
	int getChannel();

	/** @brief returns value
	*
	* @return value assigned to packet
	*/
	int getValue();
};

