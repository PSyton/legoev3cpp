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

@class EV3ConnectionImplObjC;

namespace SBJ
{
namespace EV3
{

/*
 * This is the iOS specicific connection class
 */

class ConnectionObjC : public Connection
{
public:
	ConnectionObjC(EV3ConnectionImplObjC* impl);
	
	~ConnectionObjC() override;
	
	Type type() const override;
	
	void start(Read read) override;
	
	bool write(const uint8_t* buffer, size_t len) override;
	
private:
	EV3ConnectionImplObjC* _impl;
};
	
}
}
