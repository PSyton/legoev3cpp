//
//  SBJEV3DeviceIdentifier.h
//  LEGO Control
//
//  Created by David Giovannini on 12/1/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3FlagOperatorOverloads.h"

#include <string>
#include <functional>

namespace SBJ
{
namespace EV3
{

/*
 * DeviceIdentifier is a way for a program/user to identifiy an EV3 in a generic or specific way.
 * The identifier is used to find/restore connections.
 */

struct DeviceIdentifier
{
	enum class ConnectMethod : int
	{
		only = 0x10,
		usbOnly = 0x11,
		usbFirst = 0x01,
		bluetoothOnly = 0x12,
		bluetoothFirst = 0x02,
		wifiOnly = 0x14,
		wifiFirst = 0x04,
	};
	
	enum class SearchMethod
	{
		anyDevice,
		nameOnly,
		nameFirst,
		serialOnly,
		serialFirst
	};
	
	std::string name;
	std::string serial;
#if (TARGET_IPHONE_SIMULATOR)
	ConnectMethod connect = ConnectMethod::wifiFirst;
#else
	ConnectMethod connect = ConnectMethod::bluetoothOnly;
#endif
	SearchMethod search = SearchMethod::anyDevice;
};

FlagOperatorOverloads(DeviceIdentifier::ConnectMethod);

}
}