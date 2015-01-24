//
//  SBJEV3ConnectionFactory.h
//  LEGO Control
//
//  Created by David Giovannini on 12/1/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3ConnectionToken.h"
#include "SBJEV3ConnectionPreference.h"

#include <set>

namespace SBJ
{
namespace EV3
{

class Log;

/*
 * ConnectionFactory creates connection events per device identifier registration when a device is connected (BT paired as well)
 * and ready for use. If the disconnect can be detected on the native platform, those events are broadcasted as well.
 *
 * The implementation file will be platform specific. The implementation can assume there is one and only one ConnectionFactory
 * instance.
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
	
	void start();
	
	void prompt(ConnectionPreference method, PromptAccessoryErrored errored = PromptAccessoryErrored());
		
	void registerNotification(ConnectionToken* token);
	
	void unregisterNotification(ConnectionToken* token);
	
	void handleChangeInAccessoryConnection(ConnectionTransport transport);
	
private:
	std::set<ConnectionToken*> _tokens;
	Log& _log;
	
	std::unique_ptr<Connection> findConnection(ConnectionTransport filter, DeviceIdentifier& identifier);
};

}
}