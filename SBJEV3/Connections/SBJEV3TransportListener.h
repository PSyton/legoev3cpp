//
//  TransportListener.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/25/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3TransportSelection.h"
#include "SBJEV3Connection.h"

#include <string>
#include <functional>

namespace SBJ
{
namespace EV3
{

class Connection;
class Log;

/*
 * TransportListener is an interface to abstact native implementation
 */

class TransportListener
{
public:
	using Discovery = std::function<void(ConnectionTransport transport, const std::string& serial, const std::string& name)>;

	virtual ~TransportListener() {}
	
	virtual void startWithDiscovery(Discovery discovery) {}
	
	virtual std::unique_ptr<Connection> createConnection(Log& log, const std::string& serial)  { return std::unique_ptr<Connection>(); }
};

}
}
