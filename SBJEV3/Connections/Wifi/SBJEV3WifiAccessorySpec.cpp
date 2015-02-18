//
//  SBJEV3WifiAccessorySpec.cpp
//  Jove's Landing
//
//  Created by David Giovannini on 1/25/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#include "SBJEV3WifiAccessorySpec.h"

#include "SBJEV3WifiAccessory.h"
#include "SBJEV3DeviceIdentifier.h"

#include <stdlib.h>
#include <algorithm>

using namespace SBJ::EV3;

WifiAccessorySpec::WifiAccessorySpec(const std::string& host, const uint8_t* udpPacket, size_t length)
: _host(host)
{
	if (length < 60) { return; }
	if (udpPacket == nullptr) { return; }
	
	char packet[1024];
	::memcpy(packet, udpPacket, std::min((size_t)1024, length));
	packet[length] = 0;
	
	char* serial = packet;
	
	char* port = strchr(serial, 0x0d);
	if (port == nullptr) { return; }
	*port = 0;
	port+=2;
	
	char* name = strchr(port, 0x0d);
	if (name == nullptr) { return; }
	*name = 0;
	name+=2;
	
	char* protocol = strchr(name, 0x0d);
	if (protocol == nullptr) { return; }
	*protocol = 0;
	protocol+=2;
	
	char* end = strchr(protocol, 0x0d);
	if (end == nullptr) { return; }
	*end = 0;
	end+=2;
	
	serial = strchr(serial, ':');
	if (serial == nullptr) { return; }
	serial+=2;
	
	port = strchr(port, ':');
	if (port == nullptr) { return; }
	port+=2;

	name = strchr(name, ':');
	if (name == nullptr) { return; }
	name+=2;
	
	protocol = strchr(protocol, ':');
	if (protocol == nullptr) { return; }
	protocol+=2;
	
	_serial = serial;
	_port = ::atoi(port);
	_name = name;
	_protocol = protocol;
}

std::string WifiAccessorySpec::unlockRequest() const
{
	char packet[1024];
	::sprintf(packet, "GET /target?sn=%sVMTP1.0\x0d\x0aProtocol:%s\x0d\x0a", _serial.c_str(), _protocol.c_str());
	return packet;
}

bool WifiAccessorySpec::unlockResponse(const uint8_t* response, size_t length)
{
	char packet[1024];
	::memcpy(packet, response, std::min((size_t)1024, length));
	packet[length] = 0;
	
	if (memcmp(packet, "Accept:", 7) == 0)
	{
		*::strchr(packet+7, 0x0d) = 0;
		_acceptance = (packet + 7);
		return true;
	}
	return false;
}