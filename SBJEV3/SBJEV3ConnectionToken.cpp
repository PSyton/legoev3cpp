//
//  SBJEV3ConnectionToken.cpp
//  LEGO Control
//
//  Created by David Giovannini on 12/11/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#include "SBJEV3ConnectionToken.h"
#include "SBJEV3ConnectionFactory.h"
#include "SBJEV3Connection.h"


using namespace SBJ::EV3;

ConnectionToken::ConnectionToken(ConnectionFactory& factory, const DeviceIdentifier& identifier, ConnectionChanged action)
: _factory(factory)
, _identifier(identifier)
, _action(action)
{
	_factory.registerNotification(this);
}

ConnectionToken::~ConnectionToken()
{
	_factory.unregisterNotification(this);
}

void ConnectionToken::promptBluetooth(PromptBluetoothErrored errored)
{
	if (_connected == false)
	{
		_factory.promptBluetooth(_identifier, errored);
	}
}

void ConnectionToken::makeConnection(const DeviceIdentifier& updatedIdentifier, std::unique_ptr<Connection>& connection)
{
	if ((connection != nullptr) != (_connected == true))
	{
		// keep our identifier the same but pass along the update
		_connected = connection != nullptr;
		if (_action) _action(updatedIdentifier, connection);
	}
}

void ConnectionToken::disconnect()
{
	_connected = false;
	std::unique_ptr<Connection> connection;
	if (_action) _action(_identifier, connection);
}