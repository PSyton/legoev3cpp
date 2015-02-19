//
//  SBJEV3ConnectionFactory.cpp
//  Jove's Landing
//
//  Created by David Giovannini on 1/25/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#include "SBJEV3ConnectionFactory.h"
#include "SBJEV3DeviceIdentifier.h"
#include "SBJEV3ConnectionToken.h"
#include "SBJEV3Connection.h"

#include "SBJEV3WifiTransportListener.h"
#include "SBJEV3BluetoothTransportListener.h"
#include "SBJEV3Messenger.h"

using namespace SBJ::EV3;

ConnectionFactory::ConnectionFactory(Log& log)
: _log(log)
{
	_transports[ConnectionTransport::none].reset(new TransportListener());
	_transports[ConnectionTransport::usb].reset(new TransportListener());
	_transports[ConnectionTransport::wifi].reset(new WifiTransportListener());
	_transports[ConnectionTransport::bluetooth].reset(new BluetoothTransportListener());
}

ConnectionFactory::~ConnectionFactory()
{
}

void ConnectionFactory::start(DiscoveredDeviceChangeEvennt deviceChangedEvent)
{
	_deviceChangedEvent = deviceChangedEvent;
	for (auto& i : _transports)
	{
		i.second->startWithDiscovery([this](auto transport, auto serial, auto name)
		{
			if (name.size()) discovered(transport, serial, name); else undiscovered(transport, serial);
		});
	}
}

#pragma mark - Accessors

void ConnectionFactory::promptBluetooth(PromptAccessoryErrored errored)
{
	((BluetoothTransportListener*)_transports[ConnectionTransport::bluetooth].get())->prompt(errored);
}

std::vector<DiscoveredDevice::Ptr> ConnectionFactory::getDiscovered() const
{
	std::vector<DiscoveredDevice::Ptr> v;
	for (auto discovered : _discovered)
	{
		v.push_back(discovered.second);
	}
	return v;
}

DiscoveredDevice::Ptr ConnectionFactory::findDiscovered(DeviceIdentifier& identifier)
{
	DiscoveredDevice::Ptr discovered;
	
	if (_discovered.empty()) return discovered;
		
	bool checkName = (identifier.search || DeviceIdentifier::SearchMethod::nameCheck);
	bool checkSerial = (identifier.search || DeviceIdentifier::SearchMethod::serialCheck);
	bool serialFirst = checkSerial && (identifier.search || DeviceIdentifier::SearchMethod::serialFirst);
	bool useFirst = (checkName == false && checkSerial == false);
	
	for (auto transport : identifier.transports)
	{
		if (transport == ConnectionTransport::none)
		{
			continue;
		}
		auto byName = [identifier, transport](const auto& i)
		{
			return i.second->name() == identifier.name && i.second->hasTransport(transport);
		};
		
		auto bySerial = [identifier, transport](const auto& i)
		{
			return i.second->serial() == identifier.serial && i.second->hasTransport(transport);
		};
		
		auto byConnection = [transport](const auto& i)
		{
			return i.second->hasTransport(transport);
		};
		
		auto f = _discovered.end();
		if (useFirst)
		{
			f = std::find_if(_discovered.begin(), _discovered.end(), byConnection);
		}
		else
		{
			if (serialFirst)
			{
				f = std::find_if(_discovered.begin(), _discovered.end(), bySerial);
			}
			if (checkName && f == _discovered.end())
			{
				f = std::find_if(_discovered.begin(), _discovered.end(), byName);
			}
			if (checkSerial && f == _discovered.end())
			{
				f = std::find_if(_discovered.begin(), _discovered.end(), bySerial);
			}
		}
		
		if (f != _discovered.end())
		{
			discovered = f->second;
			identifier.name = discovered->name();
			identifier.serial = discovered->serial();
			identifier.transports.makePriority(transport);
			break;
		}
	}
	return discovered;
}

#pragma mark - Transport Events
	
void ConnectionFactory::discovered(ConnectionTransport transport, const std::string& serial, const std::string& name)
{
	DiscoveredDeviceChanged change = DiscoveredDeviceChanged::none;
	DiscoveredDevice::Ptr discovered;
	auto findDiscovered = _discovered.find(serial);
	if (findDiscovered == _discovered.end())
	{
		discovered.reset(new DiscoveredDevice(_log));
		_discovered[serial] = discovered;
		change |= DiscoveredDeviceChanged::added;
	}
	else
	{
		discovered = findDiscovered->second;
	}
	change |= discovered->addTransport(*this, transport, serial, name);
	_deviceChangedEvent(*discovered, change);
}

void ConnectionFactory::undiscovered(ConnectionTransport transport, const std::string& serial)
{
	DiscoveredDeviceChanged change = DiscoveredDeviceChanged::none;
	auto findDiscovered = _discovered.find(serial);
	if (findDiscovered != _discovered.end())
	{
		DiscoveredDevice::Ptr discovered = findDiscovered->second;
		change |= discovered->removeTransport(transport);
		if (discovered->hasAnyTransport())
		{
			change |= DiscoveredDeviceChanged::removed;
			_discovered.erase(serial);
		}
		_deviceChangedEvent(*discovered, change);
	}
}

#pragma mark - ConnectionToken Lifecycle

void ConnectionFactory::registerConnectionNotification(ConnectionToken* token)
{
	_tokens.insert(token);
}

void ConnectionFactory::unregisterConnectionNotification(ConnectionToken* token)
{
	_tokens.erase(token);
}

#pragma mark - ConnectionToken Connection Requests
	
bool ConnectionFactory::requestConnect(ConnectionToken& token)
{
	std::string serial = token.serial();
	DiscoveredDeviceChanged change = DiscoveredDeviceChanged::none;
	auto findDiscovered = _discovered.find(serial);
	if (findDiscovered != _discovered.end())
	{
		DiscoveredDevice::Ptr discovered = findDiscovered->second;
		auto connection = _transports[token.transport()]->createConnection(_log, serial);
		if (connection != nullptr)
		{
			token.connectionHasBennMade(discovered, connection);
			change |= DiscoveredDeviceChanged::connected;
		}
		_deviceChangedEvent(*discovered, change);
		return (change || DiscoveredDeviceChanged::connected);
	}
	return false;
}

bool ConnectionFactory::requestDisconnect(ConnectionToken& token)
{
	std::string serial = token.serial();
	DiscoveredDeviceChanged change = DiscoveredDeviceChanged::none;
	auto findDiscovered = _discovered.find(serial);
	if (findDiscovered != _discovered.end())
	{
		DiscoveredDevice::Ptr discovered = findDiscovered->second;
		_transports[token.transport()]->requestDisconnect(serial);
		std::unique_ptr<Connection> connection;
		token.connectionHasBennMade(DiscoveredDevice::Ptr(), connection);
		change |= DiscoveredDeviceChanged::disconnected;
		_deviceChangedEvent(*discovered, change);
		return true;
	}
	return false;
}