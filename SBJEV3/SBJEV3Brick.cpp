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
: _log(factory.log())
, _identifier(identifier)
, _stack(_log, [](auto buffer) { return((const COMRPL*)buffer)->MsgCnt; })
{
	_token.reset((
		new ConnectionToken(factory, identifier,
		  [this](DeviceIdentifier updatedIdentifier, std::unique_ptr<Connection>& connection)
		  {
			  handleConnectionChange(updatedIdentifier, connection);
		  })));
}

Brick::~Brick()
{
}

void Brick::setName(const std::string& name)
{
	SetBrickName set;
	set.name = name;
	directCommand(5.0, set);
	_name = name;
}
	
BatteryInfo Brick::battery()
{
	auto result = directCommand(1.0, BatteryV(), BatteryI(), BatteryT(), BatteryL());
	return { std::get<0>(result), std::get<1>(result), std::get<2>(result), std::get<3>(result) };
}

bool Brick::isConnected() const
{
	return _token->isConnected();
}

void Brick::promptForBluetooth(PromptBluetoothErrored errored)
{
	_token->promptBluetooth([this, errored](auto error)
	{
		if (errored) errored(*this, error);
	});
}

void Brick::disconnect()
{
	_token->disconnect();
}

void Brick::handleConnectionChange(const DeviceIdentifier& updatedIdentifier, std::unique_ptr<Connection>& connection)
{
	_identifier = updatedIdentifier;
	_connectionType = connection ? connection->type() : Connection::Type::none;
	_stack.connectionChange(connection);
	_name = std::get<0>(directCommand(5.0, GetBrickName()));
	if (connectionEvent) connectionEvent(*this);
}
