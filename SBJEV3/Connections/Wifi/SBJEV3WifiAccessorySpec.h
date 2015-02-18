//
//  SBJEV3WifiAccessorySpec.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/23/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#pragma once

#include <string>

namespace SBJ
{
namespace EV3
{
	
/*
 *	WifiAccessorySpec is a light-weight class that contains all information for a WIFI connection.
 */

class WifiAccessorySpec
{
public:
	WifiAccessorySpec(const std::string& host, const uint8_t* udpPacket, size_t length);
	
	bool isValid() const
	{
		return (_serial.empty() == false);
	}
	
	const std::string& host()
	{
		return _host;
	}
	
	const int port()
	{
		return _port;
	}
	
	const std::string& name()
	{
		return _name;
	}
	
	const std::string& serial()
	{
		return _serial;
	}
	
	std::string unlockRequest() const;
	
	bool unlockResponse(const uint8_t* response, size_t length);
	
	bool isUnlocked() const
	{
		return (_acceptance.empty() == false);
	}
	
	void forgetLock()
	{
		_acceptance.clear();
	}
	
private:
	std::string _host;
	int _port;
	std::string _protocol;
	std::string _serial;
	std::string _name;
	std::string _acceptance;
};

}
}