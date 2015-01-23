//
//  SBJEV3ConnectionObjC.h
//  LEGO Control
//
//  Created by David Giovannini on 12/1/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#import <ExternalAccessory/ExternalAccessory.h>
#import "SBJEV3Connection.h"

@class EV3ConnectionImpl;

namespace SBJ
{
namespace EV3
{

/*
 * This is the Obj-C specicific connection class. It uses an Obj-C impl.
 */

class ConnectionObjC : public Connection
{
public:
	ConnectionObjC(EV3ConnectionImpl* impl);
	
	~ConnectionObjC() override;
	
	ConnectionTransport transport() const override;
	
	void start(Read read) override;
	
	bool write(const uint8_t* buffer, size_t len) override;
	
private:
	EV3ConnectionImpl* _impl;
};
	
}
}
