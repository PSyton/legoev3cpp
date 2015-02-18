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

ConnectionToken::ConnectionToken(const std::string& serial, ConnectionTransport transport, ConnectionFactory& factory, ConnectionChanged connectionChange)
: _serial(serial)
, _transport(transport)
, _isConnected(false)
, _factory(factory)
, _connectionChange(connectionChange)
{
	_factory.registerConnectionNotification(this);
}

ConnectionToken::~ConnectionToken()
{
	_factory.unregisterConnectionNotification(this);
}

void ConnectionToken::connectionHasBennMade(DiscoveredDevice::Ptr device, std::unique_ptr<Connection>& connection)
{
	if ((connection == nullptr) != (_isConnected == false))
	{
		_isConnected = connection != nullptr;
		if (_connectionChange) _connectionChange(device, connection);
	}
}

bool ConnectionToken::setIsConnected(bool connected)
{
	if (connected)
	{
		return _factory.requestConnect(*this);
	}
	return _factory.requestDisconnect(*this);
}
