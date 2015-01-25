//
//  SBJEV3WifiAccessory.cpp
//  Jove's Landing
//
//  Created by David Giovannini on 1/23/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

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

#pragma mark -

WifiAccessory::WifiAccessory(const WifiAccessorySpec& spec)
: WifiAccessorySpec(spec)
, _state(isValid() ? State::discovered : State::errored)
{
	udpPing();
}

bool WifiAccessory::tryLock(const uint8_t* response, size_t length)
{
	bool locked = false;
	{
		std::unique_lock<std::mutex> lock(_mutex);
		locked = unlockResponse(response, length);
		if (locked)
		{
			_state = State::locked;
		}
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
	_isReady.wait_for(lock, std::chrono::milliseconds(3000), ^{return isUnlocked() > 0;});
	return isUnlocked();
}
	
void WifiAccessory::unlock(bool withError)
{
	std::unique_lock<std::mutex> lock(_mutex);
	forgetLock();
	_state = withError ? State::errored : State::discovered;
}

WifiAccessory::State WifiAccessory::evaluateStaleness()
{
	std::unique_lock<std::mutex> lock(_mutex);
	if (_state == State::discovered)
	{
		const auto stop = std::chrono::system_clock::now();
		const auto d_actual = std::chrono::duration_cast<std::chrono::minutes>(stop - _ping).count();
		if (d_actual >= 30)
		{
			_state = State::stale;
		}
	}
	return _state;
}

void WifiAccessory::udpPing()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_ping = std::chrono::system_clock::now();
	if (_state == State::stale)
	{
		_state = State::discovered;
	}
}

#pragma mark -

WifiAccessoryCollection::WifiAccessoryCollection()
{
}

void WifiAccessoryCollection::start(Change change)
{
	_change = change;
}

void WifiAccessoryCollection::evaluateStaleness()
{
	std::set<WifiAccessory::Ptr> stale;
	for (auto a : _accessories)
	{
		auto state = a.second->evaluateStaleness();
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
			f->second->udpPing();
		}
	}
}

WifiAccessory::Ptr WifiAccessoryCollection::findAccessory(DeviceIdentifier& identifier)
{
	WifiAccessory::Ptr accessory;
	
	if (_accessories.empty()) return accessory;

	auto byName = [identifier](const auto& i)
	{
		return i.second->name() == identifier.name;
	};
	
	auto bySerial = [identifier](const auto& i)
	{
		return i.second->serial() == identifier.serial;
	};
	
	auto f = _accessories.end();
	switch (identifier.search)
	{
		case DeviceIdentifier::SearchMethod::anyDevice:
			f = _accessories.begin();
			break;
		case DeviceIdentifier::SearchMethod::nameOnly:
			f = std::find_if(_accessories.begin(), _accessories.end(), byName);
			break;
		case DeviceIdentifier::SearchMethod::serialOnly:
			f = std::find_if(_accessories.begin(), _accessories.end(), bySerial);
			break;
		case DeviceIdentifier::SearchMethod::nameFirst:
			f = std::find_if(_accessories.begin(), _accessories.end(), byName);
			if (f == _accessories.end()) f = std::find_if(_accessories.begin(), _accessories.end(), bySerial);
			break;
		case DeviceIdentifier::SearchMethod::serialFirst:
			auto f = std::find_if(_accessories.begin(), _accessories.end(), bySerial);
			if (f == _accessories.end()) f = std::find_if(_accessories.begin(), _accessories.end(), byName);
			break;
	}
	
	if (f != _accessories.end())
	{
		accessory = f->second;
		identifier.name = accessory->name();
		identifier.serial = accessory->serial();
		identifier.connection.makePriority(ConnectionTransport::wifi);
	}
	
	return accessory;
}

