//
//  SBJEV3ConnectionToken.h
//  LEGO Control
//
//  Created by David Giovannini on 12/1/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3DeviceIdentifier.h"

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

using ConnectionChanged = std::function<void(const DeviceIdentifier& updatedIdentifier, std::unique_ptr<Connection>& connection)>;
using PromptAccessoryErrored = std::function<void(PromptAccessoryError error)>;

/*
 * ConnectionToken is an RAII connection change registration token
 */
 
class ConnectionToken
{
public:
	ConnectionToken(ConnectionFactory& factory, const DeviceIdentifier& identifier, ConnectionChanged action);
	
	~ConnectionToken();
	
	const DeviceIdentifier& identifier() const
	{
		return _identifier;
	}
	
	bool isConnected() const
	{
		return _connected;
	}
	
	void makeConnection(const DeviceIdentifier& updatedIdentifier, std::unique_ptr<Connection>& connection);
	
	void prompt(PromptAccessoryErrored errored = PromptAccessoryErrored());
	
	void disconnect();
	
private:
	ConnectionFactory& _factory;
	DeviceIdentifier _identifier;
	ConnectionChanged _action;
	bool _connected = false;
};


}
}