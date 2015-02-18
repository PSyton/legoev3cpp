//
//  SBJEV3ConnectionToken.h
//  LEGO Control
//
//  Created by David Giovannini on 12/1/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3DiscoveredDevice.h"

#include <string>
#include <functional>
#include <memory>

namespace SBJ
{
namespace EV3
{

enum class PromptAccessoryError
{
	canceled,
	noConnectivity,
	failureToConnect
};

class ConnectionFactory;
class Connection;

using ConnectionChanged = std::function<void(DiscoveredDevice::Ptr device, std::unique_ptr<Connection>& connection)>;
using PromptAccessoryErrored = std::function<void(PromptAccessoryError error)>;

/*
 * ConnectionToken is an RAII connection change registration token
 */
 
class ConnectionToken
{
public:
	ConnectionToken(const std::string& serial, ConnectionTransport transport, ConnectionFactory& factory, ConnectionChanged connectionChange);

	~ConnectionToken();
	
	const std::string& serial() const
	{
		return _serial;
	}
	
	bool isConnected() const
	{
		return _isConnected;
	}
	
	ConnectionTransport transport() const
	{
		return _transport;
	}
	
	void connectionHasBennMade(DiscoveredDevice::Ptr device, std::unique_ptr<Connection>& connection);
	
	bool setIsConnected(bool connected);
	
private:
	std::string _serial;
	ConnectionFactory& _factory;
	ConnectionChanged _connectionChange;
	ConnectionTransport _transport;
	bool _isConnected;
};


}
}