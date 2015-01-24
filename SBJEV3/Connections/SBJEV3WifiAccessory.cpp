//
//  SBJEV3WifiAccessory.cpp
//  Jove's Landing
//
//  Created by David Giovannini on 1/23/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#include "SBJEV3WifiAccessory.h"

#include <stdlib.h>

using namespace SBJ::EV3;

WifiAccessory::WifiAccessory(const std::string& host, const uint8_t* udpPacket, size_t length)
: _host(host)
{
	char packet[1024];
	::memcpy(packet, udpPacket, std::min((size_t)1024, length));
	packet[length] = 0;
	
	char* serial = packet;
	char* port = strchr(serial, 0x0d) + 2;
	*(port - 2) = 0;
	char* name = strchr(port, 0x0d) + 2;
	*(name - 2) = 0;
	char* protocol = strchr(name, 0x0d) + 2;
	*(protocol - 2) = 0;
	char* end = strchr(protocol, 0x0d) + 2;
	*(end - 2) = 0;
	
	serial = strchr(serial, ':') + 2;
	port = strchr(port, ':') + 2;
	name = strchr(name, ':') + 2;
	protocol = strchr(protocol, ':') + 2;
	
	_serial = serial;
	_port = ::atoi(port);
	_name = name;
	_protocol = protocol;
}

std::string WifiAccessory::unlockRequest() const
{
	char packet[1024];
	::sprintf(packet, "GET /target?sn=%sVMTP1.0\x0d\x0aProtocol:%s\x0d\x0a", _serial.c_str(), _protocol.c_str());
	return packet;
}

bool WifiAccessory::tryUnlock(const uint8_t* response, size_t length)
{
	char packet[1024];
	::memcpy(packet, response, std::min((size_t)1024, length));
	packet[length] = 0;
	
	if (memcmp(packet, "Accept:", 7) == 0)
	{
		{
			std::unique_lock<std::mutex> lock(_mutex);
			_acceptance = (packet + 7);
		}
		_isReady.notify_all();
		return true;
	}
	return false;
}

	
bool WifiAccessory::waitUnlocked()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_isReady.wait_for(lock, std::chrono::milliseconds(2000), ^{return _acceptance.size() > 0;});
	return _acceptance.size() > 0;
}
