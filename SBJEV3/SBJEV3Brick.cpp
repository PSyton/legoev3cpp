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
	
Brick::Battery Brick::battery()
{
	auto result = directCommand(1.0, BatteryVoltage(), BatteryCurrent(), BatteryTempuratureRise(), BatteryLevel());
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
	if (_connectionType != Connection::Type::none)
	{
		// TODO: why timeouts? const length in opcodes could be -1, max lengths maybe incorrect size
		auto result = directCommand(5.0,
			GetBrickName(),
			HardwareVersion(),
			//FirmwareVersion(), // timeout
			FirmwareBuild(),
			//OSVersion(), // timeout
			//OSBuild(), // timeout
			FullVersion()
			);
		_name = std::get<0>(result);
		// TODO: serial number
		_version = { std::get<1>(result), /*std::get<2>(result)*/"", std::get<2>(result), /*std::get<4>(result)*/"", /*std::get<5>(result)*/"", std::get<3>(result) };
	}
	else
	{
		_name.clear();
		_version.clear();
	}
	if (connectionEvent) connectionEvent(*this);
}
