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
	_key = _serial;
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

WifiAccessory::WifiAccessory(const WifiAccessorySpec& spec)
: _spec(spec)
, _ping(std::chrono::system_clock::now())
{
}

bool WifiAccessory::tryLock(const uint8_t* response, size_t length)
{
	bool locked = false;
	{
		std::unique_lock<std::mutex> lock(_mutex);
		locked = _spec.unlockResponse(response, length);
	}
	if (locked)
	{
		_isReady.notify_all();
	}
	return locked;
}
	
bool WifiAccessory::waitForLock()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_isReady.wait_for(lock, std::chrono::milliseconds(3000), ^{return _spec.isUnlocked() > 0;});
	return _spec.isUnlocked();
}
	
void WifiAccessory::unlock()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_spec.unlock();
}

WifiAccessoryCollection::WifiAccessoryCollection()
{
}

void WifiAccessoryCollection::start(Change change)
{
	_change = change;
}

void WifiAccessoryCollection::ping()
{
	std::set<WifiAccessory::Ptr> stale;
	for (auto a : _accessories)
	{
		auto state = a.second->state();
		switch (state)
		{
			case WifiAccessory::State::discovered:
				break;
			case WifiAccessory::State::locked:
				break;
			case WifiAccessory::State::stale:
				stale.insert(a.second);
				break;
			case WifiAccessory::State::errored:
				stale.insert(a.second);
				break;
		}
	}
	for (auto a : stale)
	{
		_accessories.erase(a->key());
	}
	
	for (auto a : stale)
	{
		_change(a->key(), WifiAccessory::Ptr());
	}
}

void WifiAccessoryCollection::onUdpPacket(const std::string& host, const uint8_t* data, size_t length)
{
	WifiAccessorySpec spec(host, data, length);
	WifiAccessory::Ptr accessory;
	if (spec.isValid())
	{
		auto f = _accessories.find(spec.key());
		if (f == _accessories.end())
		{
			accessory.reset(new WifiAccessory(spec));
			_accessories[spec.key()] = accessory;
			_change(accessory->key(), accessory);
		}
		else
		{
			f->second->ping();
		}
	}
}

WifiAccessory::Ptr WifiAccessoryCollection::findAccessory(DeviceIdentifier& identifier)
{
	return _accessories.size() ? _accessories.begin()->second : WifiAccessory::Ptr();
}

