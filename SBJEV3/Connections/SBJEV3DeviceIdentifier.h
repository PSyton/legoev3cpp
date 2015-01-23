//
//  SBJEV3DeviceIdentifier.h
//  LEGO Control
//
//  Created by David Giovannini on 12/1/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3ConnectionPreference.h"

#include <string>

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
	ConnectionPreference connection;
	SearchMethod search = SearchMethod::anyDevice;
};

}
}