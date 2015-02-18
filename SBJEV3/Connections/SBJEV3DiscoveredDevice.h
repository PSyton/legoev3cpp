//
//  SBJEV3DiscoveredDevice.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/25/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3TransportSelection.h"
#include "SBJEV3Messenger.h"
#include "SBJEV3FlagOperatorOverloads.h"

#include <string>
#include <map>

namespace SBJ
{
namespace EV3
{

class ConnectionToken;
class ConnectionFactory;
class Messenger;
class Log;

enum class DiscoveredDeviceChanged : int
{
	none				= 0x0000,
	added				= 0x1000,
	transportAdded		= 0x0100,
	connected			= 0x0010,
	nameChange			= 0x0001,
	serialChange		= 0x0002,
	disconnected		= 0x0020,
	transportRemoved	= 0x0200,
	removed				= 0x2000
};
FlagOperatorOverloads(DiscoveredDeviceChanged)

/*
 * DiscoveredDevice is the single record of a device discovered by one or more transports.
 */

class DiscoveredDevice
{
public:
	using Ptr = std::shared_ptr<DiscoveredDevice>;

	DiscoveredDevice(Log& log);
	
	~DiscoveredDevice();
	
	Log& log() const
	{
		return _log;
	}
	
	DiscoveredDeviceChanged addTransport(ConnectionFactory& factory, ConnectionTransport transport, const std::string& serial, const std::string& name);
	
	DiscoveredDeviceChanged removeTransport(ConnectionTransport transport);
	
	bool hasTransport(ConnectionTransport transport) const
	{
		return _available.find(transport);
	}
	
	bool hasAnyTransport() const
	{
		return _available.findAny();
	}
	
	bool isConnected(ConnectionTransport transport) const;
	
	bool setIsConnected(ConnectionTransport transport, bool connected);
	
	const std::string& name()
	{
		return _name.size() ? _name : _serial;
	}
	
	const std::string& serial()
	{
		return _serial;
	}
	
	Messenger& messenger()
	{
		return _messenger;
	}

private:
	Log& _log;
	std::string _serial;
	std::string _name;
	TransportSelection _available;
	std::map<ConnectionTransport, std::unique_ptr<ConnectionToken>> _connectionTokens;
	Messenger _messenger;
};

}
}