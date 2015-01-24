//
//  SBJEV3ConnectionFactory.cpp
//  LEGO Control
//
//  Created by David Giovannini on 12/1/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#include "SBJEV3ConnectionFactory.h"
#include "SBJEV3ConnectionObjC.h"
#import "SBJEV3BluetoothAnouncerIOS.h"
#import "SBJEV3WifiAnnouncerObjC.h"

using namespace SBJ::EV3;

// We cheat with global variables here instead of a c++ pimpl.
static EV3BluetoothAnouncer* _bluetooth = nil;
static EV3WifiAnnouncer* _wifi = nil;

ConnectionFactory::ConnectionFactory(Log& log)
: _log(log)
{
	_bluetooth = [[EV3BluetoothAnouncer alloc] initWithChange: ^
	{
		handleChangeInAccessoryConnection();
	}];
	_wifi = [[EV3WifiAnnouncer alloc] initWithChange: ^(WifiAccessory::Ptr&)
	{
		//handleChangeInAccessoryConnection();
	}];
}

ConnectionFactory::~ConnectionFactory()
{
	_bluetooth = nil;
	_wifi = nil;
}

void ConnectionFactory::prompt(ConnectionPreference method, PromptAccessoryErrored errored)
{
	for (auto i : method)
	{
		switch (i)
		{
			case ConnectionTransport::bluetooth:
				[_bluetooth prompt: errored];
				return;
			case ConnectionTransport::wifi:
				[_wifi prompt: errored];
				return;
			case ConnectionTransport::usb:
				continue;
			case ConnectionTransport::none:
				continue;
		}
	}
}

void ConnectionFactory::registerNotification(ConnectionToken* token)
{
	_tokens.insert(token);
	DeviceIdentifier identifier = token->identifier();
	std::unique_ptr<Connection> testConnection(findConnection(identifier));
	token->makeConnection(identifier, testConnection);
}

void ConnectionFactory::unregisterNotification(ConnectionToken* token)
{
	_tokens.erase(token);
}

void ConnectionFactory::handleChangeInAccessoryConnection()
{
	for (auto item = _tokens.begin(); item != _tokens.end(); item++)
	{
		DeviceIdentifier identifier = (*item)->identifier();
		__block std::unique_ptr<Connection> testConnection(findConnection(identifier));
		dispatch_async(dispatch_get_main_queue(), ^
		{
			(*item)->makeConnection(identifier, testConnection);
		});
	}
}

std::unique_ptr<Connection> ConnectionFactory::findConnection(DeviceIdentifier& identifier)
{
	EV3ConnectionImpl* impl = nil;
	ConnectionPreference method = identifier.connection;
	
	for (auto i : method)
	{
		switch (i)
		{
			case ConnectionTransport::bluetooth:
				impl = [_bluetooth findConnection: _log identifier: identifier];
				break;
			case ConnectionTransport::wifi:
				impl = [_wifi findConnection: _log identifier: identifier];
				break;
			case ConnectionTransport::usb:
				break;
			case ConnectionTransport::none:
				break;
		}
		if (impl) break;
	}
	
	if (impl)
	{
		return std::unique_ptr<Connection>(new ConnectionObjC(impl));
	}
	return nullptr;
}
