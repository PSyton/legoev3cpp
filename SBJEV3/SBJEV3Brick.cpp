//
//  SBJEV3Brick.cpp
//  LEGO Control
//
//  Created by David Giovannini on 11/25/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#include "SBJEV3Brick.h"
#include "SBJEV3ConnectionFactory.h"
#include "SBJEV3Connection.h"

using namespace SBJ::EV3;

Brick::Brick(ConnectionFactory& factory, const DeviceIdentifier& identifier)
: _log(factory.log())
, _identifier(identifier)
, _messenger(_log, [](auto buffer) { return((const COMRPL*)buffer)->MsgCnt; })
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
	directCommand(0.0, set);
	_name = name;
}
	
Brick::Battery Brick::battery()
{
	auto result = directCommand(1.0, BatteryLevel(), BatteryVoltage(), BatteryCurrent(), BatteryTempuratureRise());
	return { std::get<0>(result), std::get<1>(result), std::get<2>(result), std::get<3>(result) };
}

bool Brick::isConnected() const
{
	return _token->isConnected();
}

void Brick::prompt(PromptAccessoryErrored errored)
{
	_token->prompt([this, errored](auto error)
	{
		if (errored) errored(*this, error);
	});
}

void Brick::prompt(ConnectionTransport transport, PromptAccessoryErrored errored)
{
	_token->prompt(transport, [this, errored](auto error)
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
	_connectionTransport = connection ? connection->transport() : ConnectionTransport::none;
	_messenger.connectionChange(connection);
	if (_connectionTransport != ConnectionTransport::none)
	{
		auto result = directCommand(5.0,
			GetBrickName<>(),
			HardwareVersion(),
			FirmwareVersion(),
			FirmwareBuild(),
			OSVersion(),
			OSBuild(),
			FullVersion()
			);
		_name = std::get<0>(result);
		_version = { std::get<1>(result), std::get<2>(result), std::get<3>(result), std::get<4>(result), std::get<5>(result), std::get<6>(result) };
	}
	else
	{
		_name.clear();
		_version.clear();
	}
	if (connectionEvent) connectionEvent(*this);
}
