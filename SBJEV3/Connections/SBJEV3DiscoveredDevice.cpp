//
//  SBJEV3DiscoveredDevice.cpp
//  Jove's Landing
//
//  Created by David Giovannini on 1/25/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#include "SBJEV3DiscoveredDevice.h"
#include "SBJEV3ConnectionToken.h"

using namespace SBJ::EV3;

DiscoveredDevice::DiscoveredDevice(Log& log)
: _log(log)
, _available(ConnectionTransport::none)
, _messenger(log, [](auto buffer) { return((const COMRPL*)buffer)->MsgCnt; })
{
}

DiscoveredDevice::~DiscoveredDevice()
{
}

DiscoveredDeviceChanged DiscoveredDevice::addTransport(ConnectionFactory& factory, ConnectionTransport transport, const std::string& serial, const std::string& name)
{
	DiscoveredDeviceChanged change = DiscoveredDeviceChanged::none;
	if (serial.length())
	{
		change |= DiscoveredDeviceChanged::serialChange;
		_serial = serial;
	}
	if (name.length())
	{
		change |= DiscoveredDeviceChanged::nameChange;
		_name = name;
	}
	if (hasTransport(transport) == false)
	{
		change |= DiscoveredDeviceChanged::transportAdded;
		_connectionTokens[transport].reset((
			new ConnectionToken(_serial, transport, factory,
			  [this, transport](auto device, auto& connection)
			  {
				  _messenger.connectionChange(transport, connection);
			  })));
		_available.insert(transport);
	}
	return change;
}

DiscoveredDeviceChanged DiscoveredDevice::removeTransport(ConnectionTransport transport)
{
	DiscoveredDeviceChanged change;
	if (hasTransport(transport) == true)
	{
		change |= DiscoveredDeviceChanged::transportRemoved;
		_connectionTokens.erase(transport);
		_available.erase(transport);
	}
	return change;
}

bool DiscoveredDevice::isConnected(ConnectionTransport transport) const
{
	return _messenger.isConnected(transport);
}

bool DiscoveredDevice::setIsConnected(ConnectionTransport transport, bool connected)
{
	auto token = _connectionTokens[transport].get();
	if (token)
	{
		return token->setIsConnected(connected);
	}
	return false;
}