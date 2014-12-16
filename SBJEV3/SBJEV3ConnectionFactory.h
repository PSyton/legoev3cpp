//
//  SBJEV3ConnectionFactory.h
//  LEGO Control
//
//  Created by David Giovannini on 12/1/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include <set>
#include "SBJEV3ConnectionToken.h"

namespace SBJ
{
namespace EV3
{

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

	ConnectionFactory();
	
	~ConnectionFactory();
	
	void promptBluetooth(DeviceIdentifier identifier = DeviceIdentifier(), PromptBluetoothCompleted completion = PromptBluetoothCompleted());
		
	void registerNotification(ConnectionToken* token);
	
	void unregisterNotification(ConnectionToken* token);
	
	void handleChangeInAccessoryConnection();
	
private:
	std::set<ConnectionToken*> _tokens;
	
	std::unique_ptr<Connection> findConnection(DeviceIdentifier& identifier);
	
};

}
}