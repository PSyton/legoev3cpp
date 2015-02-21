//
//  SBJEV3Brick.h
//  LEGO Control
//
//  Created by David Giovannini on 11/25/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3DiscoveredDevice.h"
#include "SBJEV3DeleteMethods.h"
#include "SBJEV3DeviceIdentifier.h"

#include <memory>

namespace SBJ
{
namespace EV3
{

class ConnectionFactory;
class DeviceIdentifier;
	
/*
 * The brick is the high-level object that pairs a discovered device and a singular transport
 */
 
class Brick
{
public:
	DeleteDefaultMethods(Brick);
	
	struct Battery
	{
		uint8_t level;
		float voltage;
		float current;
		float tempuratureRise;
		
		float indicator() const { return (float)level / 100.0; }
		float power() const { return voltage * current; }
		float resistence() const { return voltage / current; }
	};
	
	Brick(ConnectionFactory& factory);
	
	~Brick();
	
	void fetchDevice(const DeviceIdentifier& identifier = DeviceIdentifier());
	
	std::string serialNumber() const;
		
	bool isConnected() const;
	
	ConnectionTransport activeTransport() const
	{
		return _activeTransport;
	}
	
	std::string name() const;
	
	void setName(const std::string& name);
	
	DeviceInfo version() const;
	
	Battery battery();
	
	template <typename...  Opcodes>
	typename DirectCommand<Opcodes...>::Results directCommand(float timeout, Opcodes... opcodes)
	{
		if (auto device = _device.lock())
		{
			if (device->isConnected(_activeTransport))
			{
				return device->messenger().directCommand(_activeTransport, timeout, opcodes...);
			}
		}
		return typename DirectCommand<Opcodes...>::Results();
	}

	template <typename  Opcode>
	typename SystemCommand<Opcode>::Results systemCommand(float timeout, Opcode opcode)
	{
		if (auto device = _device.lock())
		{
			if (device->isConnected(_activeTransport))
			{
				return device->messenger().systemCommand(_activeTransport, timeout, opcode);
			}
		}
		return typename SystemCommand<Opcode>::Results();
	}

private:
	ConnectionFactory& _factory;
	ConnectionTransport _activeTransport = ConnectionTransport::none;
	std::weak_ptr<DiscoveredDevice> _device;
	
	void fetchBrickInfo(DiscoveredDevice::Ptr device);
};
	
}
}
