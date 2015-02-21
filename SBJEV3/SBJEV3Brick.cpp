//
//  SBJEV3Brick.cpp
//  LEGO Control
//
//  Created by David Giovannini on 11/25/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#include "SBJEV3Brick.h"
#include "SBJEV3ConnectionFactory.h"
#include "SBJEV3Connection.h"

using namespace SBJ::EV3;

Brick::Brick(ConnectionFactory& factory)
: _factory(factory)
{
}

Brick::~Brick()
{
}

void Brick::fetchDevice(const DeviceIdentifier& identifier)
{
	DeviceIdentifier resolved = identifier;
	_device = _factory.findDiscovered(resolved);
	if (auto device = _device.lock())
	{
		_activeTransport = *resolved.transports.begin();
		if (device->setIsConnected(_activeTransport, true))
		{
			fetchBrickInfo(device);
			return;
		}
		_activeTransport = ConnectionTransport::none;
	}
}

std::string Brick::name() const
{
	if (auto device = _device.lock())
	{
		return device->info().name;
	}
	return "";
}

void Brick::setName(const std::string& name)
{
	if (auto device = _device.lock())
	{
		SetBrickName set;
		set.name = name;
		directCommand(0.0, set);
		fetchBrickInfo(device);
	}
}

std::string Brick::serialNumber() const
{
	if (auto device = _device.lock())
	{
		return device->info().serial;
	}
	return "";
}

DeviceInfo Brick::version() const
{
	if (auto device = _device.lock())
	{
		return device->info();
	}
	return DeviceInfo();
}

bool Brick::isConnected() const
{
	if (auto device = _device.lock())
	{
		return device->isConnected(_activeTransport);
	}
	return false;
}

Brick::Battery Brick::battery()
{
	auto result = directCommand(1.0, BatteryLevel(), BatteryVoltage(), BatteryCurrent(), BatteryTempuratureRise());
	return { std::get<0>(result), std::get<1>(result), std::get<2>(result), std::get<3>(result) };
}

void Brick::fetchBrickInfo(DiscoveredDevice::Ptr device)
{
	// TODO: bluetooth vs. wifi - the full call in bluetooth kills the BT stack on the brick (EV3 reboot required)
	if (_activeTransport == ConnectionTransport::bluetooth)
	{
		auto result = directCommand(1.0,
			GetBrickName<>(),
			FullVersion()
			);
		
		DeviceInfo info;
		info.serial = device->info().serial;
		info.name = std::get<0>(result);
		info.fullVersion = std::get<1>(result);
		
		_factory.updateDeviceInfo(info);
	}
	else
	{
		auto result = directCommand(1.0,
			GetBrickName<>(),
			HardwareVersion(),
			FirmwareVersion(),
			FirmwareBuild(),
			OSVersion(),
			OSBuild(),
			FullVersion()
			);
			
		DeviceInfo info;
		info.serial = device->info().serial;
		info.name = std::get<0>(result);
		info.hardwareVersion = std::get<1>(result);
		info.firmwareVersion = std::get<2>(result);
		info.firmwareBuild = std::get<3>(result);
		info.oSVersion = std::get<4>(result);
		info.oSBuild = std::get<5>(result);
		info.fullVersion = std::get<6>(result);
		
		_factory.updateDeviceInfo(info);
	}
}
