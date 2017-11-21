#include "stdafx.h"
#include "Packet.h"
#include <vector>
#include <sstream>
#include <iostream>


CPacket::CPacket()
{
}

CPacket::CPacket(char command, char type,int channel)
{
	this->command = command;
	this->type = type;
	this->channel = channel;
}

CPacket::CPacket(char command, char type,int channel, int value)
{
	this->command = command;
	this->type = type;
	this->channel = channel;
	this->value = value;
}


CPacket::~CPacket()
{
}

std::string CPacket::createString()
{
	std::string str="";
	str += "$";							//start byte
	str += toupper(command);			//command
	str += " ";							//whitespace
	str += toupper(type);				//type of channel <D,A,S>
	str += " ";							//whitespace
	str += std::to_string(channel);		//channel
	if (toupper(command) == 'S')		//only set requires a value to be sent
	{
		str += " ";						//add whitespace
		str += std::to_string(value);	//add value
	}
	str += " ";							//add whitespace
	str += "\r\n";						//end byte(s)
	return str; //"$<command> <channel> <value(optional)> \r\n"
}

bool CPacket::parseString(std::string data)
{
	std::vector<std::string> temp;								//contains parsed strings
	std::string tokenized_result;								//temp value moved to temp
	std::stringstream student_info_inline(data);				//string stream doing the parsing
	//tokenize line into seperate strings
	while (getline(student_info_inline, tokenized_result, ' '))
	{
		temp.push_back(tokenized_result);
	}

	if (temp.size() < TOKENIZEDPACKETSIZE)		//packet must have parsed this many strings or packet is corrupt
	{
		std::cout << "\t\tERROR: parseString not enough tokenized strings";
		return false;
	}

	command = temp[0].back();					//set command
	type = temp[1].back();						//set type
	if (checkForValidInt(temp[2]))				//check if channel is valid and convert to int
	{
		channel = std::atoi(temp[2].c_str());
	}
	//remove \r\n temp fix
	temp[3].resize(temp[3].size() - 2);
	if (checkForValidInt(temp[3]))				//check if value returned is valid and convert to int
	{
		value = std::atoi(temp[3].c_str());		
	}
	return true;
}

bool CPacket::checkForValidInt(std::string str)
{
	for (int i = 0; i < str.length(); i++)
	{
		if (!isdigit(str[i]))
		{
			return false;
		}
	}
	return true;
}

bool CPacket::operator==(const CPacket & rhs)
{
	bool state = true;
	//commands will be different but if set 'S' is called the values must match
	if (((rhs.command == 'S') | (command == 'S'))&(rhs.value != value))
	{
		state = false;
		std::cout << "\t\tERROR:== operator values do not match on set command\n";
	}
	else if(rhs.type!=type)
	{
		state = false;
		std::cout << "\t\tERROR:== operator types do not match\n";
	}
	else if (rhs.channel != channel)
	{
		state = false;
		std::cout << "\t\tERROR:== operator channels do not match\n";
	}
	return false;
}

char CPacket::getCommand()
{
	return command;
}

char CPacket::getType()
{
	return type;
}

int CPacket::getChannel()
{
	return channel;
}

int CPacket::getValue()
{
	return value;
}
