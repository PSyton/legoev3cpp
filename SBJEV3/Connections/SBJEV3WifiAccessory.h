//
//  SBJEV3WifiAccessory.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/23/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#pragma once

#include <string>
#include <thread>
#include <memory>

namespace SBJ
{
namespace EV3
{

class WifiAccessory
{
public:
	using Ptr = std::unique_ptr<WifiAccessory>;

	WifiAccessory(const std::string& host, const uint8_t* udpPacket, size_t length);
	
	const std::string& host()
	{
		return _host;
	}
	
	const int port()
	{
		return _port;
	}
	
	std::string unlockRequest() const;
	
	bool tryUnlock(const uint8_t* response, size_t length);
	
	bool waitUnlocked();
	
private:
	std::mutex _mutex;
	std::condition_variable _isReady;
	
	std::string _host;
	int _port;
	std::string _protocol;
	std::string _serial;
	std::string _name;
	std::string _acceptance;
};

}
}
