//
//  SBJEV3WifiTransportListener.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/25/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3TransportListener.h"
#include "SBJEV3WifiAccessory.h"

#include <map>

namespace SBJ
{
namespace EV3
{

class WifiTransportListener : public TransportListener
{
public:
	WifiTransportListener();
	
	virtual ~WifiTransportListener();
	
	virtual void startWithDiscovery(Discovery discovery) override;
	
	virtual std::unique_ptr<Connection> createConnection(Log& log, const std::string& serial)  override;
			
	void onUdpPacket(const std::string& host, const uint8_t* data, size_t length);
	
	void evaluateStaleness();
	
private:
	std::map<std::string, WifiAccessory::Ptr> _accessories;
	Discovery _discovery;
};

}
}