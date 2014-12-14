//
//  SBJEV3Brick.cpp
//  LEGO Control
//
//  Created by David Giovannini on 11/25/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#include "SBJEV3Brick.h"
#include "SBJEV3ConnectionFactory.h"

using namespace SBJ::EV3;

Brick::Brick(ConnectionFactory& factory, const DeviceIdentifier& identifier)
: _identifier(identifier)
{
	_token.reset((
		new ConnectionToken(factory, identifier,
		  [this](DeviceIdentifier updatedIdentifier, Connection* connection)
		  {
			  _identifier = updatedIdentifier;
			  _stack.connectionChange(connection);
			  if (connectionEvent) connectionEvent(*this);
		  })));
}

Brick::~Brick()
{
}

bool Brick::isConnected() const
{
	return _token->isConnected();
}

void Brick::promptForBluetooth(PromptBluetoothCompleted completion)
{
	_token->promptBluetooth([this, completion](bool canceled)
	{
		if (completion) completion(*this, canceled);
	});
}

void Brick::disconnect()
{
	_token->disconnect();
}
