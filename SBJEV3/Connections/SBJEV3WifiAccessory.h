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
#include <chrono>
#include <map>
#include <set>

namespace SBJ
{
namespace EV3
{

class DeviceIdentifier;

class WifiAccessorySpec
{
public:
	WifiAccessorySpec(const std::string& host, const uint8_t* udpPacket, size_t length);
	
	bool isValid() const
	{
		return (_key.empty() == false);
	}
	
	bool isUnlocked() const
	{
		return (_acceptance.empty() == false);
	}
	
	void unlock()
	{
		_acceptance.clear();
	}
	
	const std::string& key() const
	{
		return _key;
	}
	
	const std::string& host()
	{
		return _host;
	}
	
	const int port()
	{
		return _port;
	}
	
	std::string unlockRequest() const;
	
	bool unlockResponse(const uint8_t* response, size_t length);
	
private:
	std::string _host;
	int _port;
	std::string _protocol;
	std::string _serial;
	std::string _name;
	std::string _acceptance;
	std::string _key;
};

class WifiAccessory
{
public:
	enum class State
	{
		discovered,
		locked,
		stale,
		errored
	};

	using Ptr = std::shared_ptr<WifiAccessory>;
	
	WifiAccessory(const WifiAccessorySpec& spec);
	
	const std::string& key() const
	{
		return _spec.key();
	}
	
	const std::string& host()
	{
		return _spec.host();
	}
	
	const int port()
	{
		return _spec.port();
	}
	
	std::string unlockRequest() const
	{
		return _spec.unlockRequest();
	}
	
	bool tryLock(const uint8_t* response, size_t length);
	
	bool waitForLock();
	
	void unlock();
	
	State state() const
	{
		return _state;
	}
	
	void ping()
	{
		_ping = std::chrono::system_clock::now();
	}
	
private:
	std::mutex _mutex;
	std::condition_variable _isReady;
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	WifiAccessorySpec _spec;
	State _state;
	std::chrono::system_clock::time_point _ping;
};

class WifiAccessoryCollection
{
public:
	using Change = std::function<void(const std::string& key, WifiAccessory::Ptr accessory)>;
	
	WifiAccessoryCollection();
	
	void start(Change change);
	
	void ping();
	
	void onUdpPacket(const std::string& host, const uint8_t* data, size_t length);
	
	WifiAccessory::Ptr findAccessory(DeviceIdentifier& identifier);
	
	const std::map<std::string, WifiAccessory::Ptr>& accessories() const
	{
		return _accessories;
	}

private:
	std::map<std::string, WifiAccessory::Ptr> _accessories;
	Change _change;
};

}
}
