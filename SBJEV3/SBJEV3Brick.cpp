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
		if (device->setIsConnected(_activeTransport, true) == false)
		{
			_activeTransport = ConnectionTransport::none;
		}
		fetchBrickInfo();
	}
	else
	{
		_activeTransport = ConnectionTransport::none;
		fetchBrickInfo();
	}
}

void Brick::setName(const std::string& name)
{
	SetBrickName set;
	set.name = name;
	directCommand(0.0, set);
	_name = name;
}

std::string Brick::serialNumber() const
{
	if (auto device = _device.lock())
	{
		return device->serial();
	}
	return "";
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

void Brick::fetchBrickInfo()
{
	// TODO: bluetooth vs. wifi - the full call in bluetooth kills the stack on the brick (reboot required)
	if (_activeTransport == ConnectionTransport::bluetooth)
	{
		auto result = directCommand(1.0,
			GetBrickName<>(),
			HardwareVersion(),
			//FirmwareVersion(), // timeout
			FirmwareBuild(),
			//OSVersion(), // timeout
			//OSBuild(), // timeout
			FullVersion()
			);
		_name = std::get<0>(result);
		_version = { std::get<1>(result), /*std::get<2>(result)*/"", std::get<2>(result), /*std::get<4>(result)*/"", /*std::get<5>(result)*/"", std::get<3>(result) };
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
		_name = std::get<0>(result);
		_version = { std::get<1>(result), std::get<2>(result), std::get<3>(result), std::get<4>(result), std::get<5>(result), std::get<6>(result) };
	}
}
