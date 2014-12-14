//
//  SBJEV3ConnectionToken.cpp
//  LEGO Control
//
//  Created by David Giovannini on 12/11/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#include "SBJEV3ConnectionToken.h"
#include "SBJEV3ConnectionFactory.h"


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

void ConnectionToken::promptBluetooth(PromptBluetoothCompleted completion)
{
	if (_connected == false)
	{
		_factory.promptBluetooth(_identifier, completion);
	}
}

bool ConnectionToken::makeConnection(const DeviceIdentifier& updatedIdentifier, Connection* connection)
{
	if ((connection != nullptr) != (_connected == true))
	{
		// keep our identifier the same but pass along the update
		_connected = connection != nullptr;
		if (_action) _action(updatedIdentifier, connection);
		return true;
	}
	return false;
}

void ConnectionToken::disconnect()
{
	_connected = false;
	if (_action) _action(_identifier, nullptr);
}