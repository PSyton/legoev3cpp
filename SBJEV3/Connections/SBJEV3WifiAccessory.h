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
	
	std::string unlockRequest() const;
	
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
	
	const std::string& name()
	{
		return _name;
	}
	
	const std::string& serial()
	{
		return _serial;
	}
	
private:
	std::string _host;
	int _port;
	std::string _protocol;
	std::string _serial;
	std::string _name;
	std::string _acceptance;
	std::string _key;
	
protected:
	
	bool unlockResponse(const uint8_t* response, size_t length);
	
	bool isUnlocked() const
	{
		return (_acceptance.empty() == false);
	}
	
	void forgetLock()
	{
		_acceptance.clear();
	}
	
};

class WifiAccessory : public WifiAccessorySpec
{
public:
	enum class State
	{
		discovered, // created
		locked, //locked
		errored, // i/o discovered disconnect
		stale // unused
	};

	using Ptr = std::shared_ptr<WifiAccessory>;
	
	WifiAccessory(const WifiAccessorySpec& spec);
	
	bool tryLock(const uint8_t* response, size_t length);
	
	bool waitForLock();
	
	void unlock(bool withError);
	
	State evaluateStaleness();
	
	void udpPing();
	
private:
	std::mutex _mutex;
	std::condition_variable _isReady;
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	State _state;
	std::chrono::system_clock::time_point _ping;
};

class WifiAccessoryCollection
{
public:
	using Change = std::function<void(const std::string& key, WifiAccessory::Ptr accessory)>;
	
	WifiAccessoryCollection();
	
	void start(Change change);
	
	void evaluateStaleness();
	
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
