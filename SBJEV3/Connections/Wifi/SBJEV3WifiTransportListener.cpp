//
//  SBJEV3WifiTransportListener.cpp
//  Jove's Landing
//
//  Created by David Giovannini on 1/25/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#include "SBJEV3WifiTransportListener.h"
#include "SBJEV3DeviceIdentifier.h"
#include "SBJEV3Connection.h"

#include <set>

using namespace SBJ::EV3;

void WifiTransportListener::onUdpPacket(const std::string& host, const uint8_t* data, size_t length)
{
	WifiAccessorySpec spec(host, data, length);
	WifiAccessory::Ptr accessory;
	if (spec.isValid())
	{
		auto f = _accessories.find(spec.serial());
		if (f == _accessories.end())
		{
			accessory.reset(new WifiAccessory(spec));
			_accessories[spec.serial()] = accessory;
			_discovery(ConnectionTransport::wifi, spec.serial(), spec.name());
		}
		else
		{
			f->second->udpPing();
		}
	}
}

void WifiTransportListener::evaluateStaleness()
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
		_accessories.erase(a->serial());
	}
	
	for (auto a : stale)
	{
		_discovery(ConnectionTransport::wifi, a->serial(), "");
	}
}
