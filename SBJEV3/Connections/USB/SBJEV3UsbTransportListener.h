//
//  SBJEV3UsbTransportListener.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/25/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3TransportListener.h"

namespace SBJ
{
namespace EV3
{

class UsbTransportListener : public TransportListener
{
public:
	UsbTransportListener() {}
	
	virtual ~UsbTransportListener() {}
	
	virtual void startWithDiscovery(Discovery discovery) override {}
	
	virtual std::unique_ptr<Connection> createConnection(Log& log, const std::string& serial) override { return std::unique_ptr<Connection>(); }

};

}
}