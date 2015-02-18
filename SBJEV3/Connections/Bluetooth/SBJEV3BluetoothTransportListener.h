//
//  SBJEV3BluetoothTransportListener.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/25/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//
#pragma once

#include "SBJEV3TransportListener.h"
#include "SBJEV3ConnectionToken.h"

#include <map>

namespace SBJ
{
namespace EV3
{

class BluetoothTransportListener : public TransportListener
{
public:

	BluetoothTransportListener();
	
	virtual ~BluetoothTransportListener();
	
	virtual void startWithDiscovery(Discovery discovery) override;
	
	virtual std::unique_ptr<Connection> createConnection(Log& log, const std::string& serial)  override;
	
	virtual void requestDisconnect(const std::string& serial) override;
	
	static void prompt(PromptAccessoryErrored errored = PromptAccessoryErrored());
	
private:
	Discovery _discovery;
};

}
}