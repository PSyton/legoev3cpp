//
//  SBJEV3ConnectionFactory.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/25/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3DiscoveredDevice.h"
#include "SBJEV3TransportListener.h"
#include "SBJEV3ConnectionToken.h"

#include <string>
#include <map>
#include <vector>
#include <set>

namespace SBJ
{
namespace EV3
{

class Log;
class DeviceIdentifier;
using DiscoveredDeviceChangeEvennt = std::function<void(DiscoveredDevice& device, DiscoveredDeviceChanged change)>;

/*
 * ConnectionFactory hosts the native implementations for connections and device discovery
 */
	
class ConnectionFactory
{
public:
	ConnectionFactory(Log& log);
	
	~ConnectionFactory();
	
	Log& log()
	{
		return _log;
	}
	
	void start(DiscoveredDeviceChangeEvennt deviceChangedEvent);

	// Brick and UI accrssors
	DiscoveredDevice::Ptr findDiscovered(DeviceIdentifier& identifier);
	std::vector<DiscoveredDevice::Ptr> getDiscovered() const;
	void promptBluetooth(PromptAccessoryErrored errored = PromptAccessoryErrored());
	
	// ConnectionToken life cycle
	void registerConnectionNotification(ConnectionToken* token);
	void unregisterConnectionNotification(ConnectionToken* token);
	
	// ConnectionToken connection requests
	bool requestDisconnect(ConnectionToken& token);
	bool requestConnect(ConnectionToken& token);
	
private:
	DiscoveredDeviceChangeEvennt _deviceChangedEvent;
	std::map<std::string, DiscoveredDevice::Ptr> _discovered;
	std::map<ConnectionTransport, std::unique_ptr<TransportListener>> _transports;
	std::set<ConnectionToken*> _tokens;
	Log& _log;
	
	// Transport events
	void discovered(ConnectionTransport transport, const std::string& serial, const std::string& name);
	void undiscovered(ConnectionTransport transport, const std::string& serial);
};

}
}