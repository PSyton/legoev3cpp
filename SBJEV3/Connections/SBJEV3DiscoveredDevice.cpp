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

bool DeviceInfo::update(const DeviceInfo& info, bool isTruth)
{
	if (isTruth)
	{
		if (info.serial.length()) serial = info.serial;
		if (info.name.length()) name = info.name;
		if (info.hardwareVersion.length()) hardwareVersion = info.hardwareVersion;
		if (info.firmwareVersion.length()) firmwareVersion = info.firmwareVersion;
		if (info.firmwareBuild.length()) firmwareBuild = info.firmwareBuild;
		if (info.oSVersion.length()) oSVersion = info.oSVersion;
		if (info.oSBuild.length()) oSBuild = info.oSBuild;
		if (info.fullVersion.length()) fullVersion = info.fullVersion;
	}
	else
	{
		if (serial.length() == 0) serial = info.serial;
		if (name.length() == 0) name = info.name;
		if (hardwareVersion.length() == 0) hardwareVersion = info.hardwareVersion;
		if (firmwareVersion.length() == 0) firmwareVersion = info.firmwareVersion;
		if (firmwareBuild.length() == 0) firmwareBuild = info.firmwareBuild;
		if (oSVersion.length() == 0) oSVersion = info.oSVersion;
		if (oSBuild.length() == 0) oSBuild = info.oSBuild;
		if (fullVersion.length() == 0) fullVersion = info.fullVersion;
	}
	return true;
}

DiscoveredDevice::DiscoveredDevice(Log& log)
: _log(log)
, _available(ConnectionTransport::none)
, _messenger(log, [](auto buffer) { return((const COMRPL*)buffer)->MsgCnt; })
{
}

DiscoveredDevice::~DiscoveredDevice()
{
}

bool DiscoveredDevice::updateInfo(const DeviceInfo& info, bool isTruth)
{
	return _info.update(info, isTruth);
}

DiscoveredDeviceChanged DiscoveredDevice::addTransport(ConnectionFactory& factory, ConnectionTransport transport, const std::string& serial, const DeviceInfo& info)
{
	DiscoveredDeviceChanged change = DiscoveredDeviceChanged::none;
	if (_info.update(info, false))
	{
		change |= DiscoveredDeviceChanged::infoChange;
	}
	if (hasTransport(transport) == false)
	{
		change |= DiscoveredDeviceChanged::transportAdded;
		_connectionTokens[transport].reset((
			new ConnectionToken(serial, transport, factory,
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