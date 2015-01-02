//
//  SBJEV3BluetoothConnectionIOS.h
//  LEGO Control
//
//  Created by David Giovannini on 12/1/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#import <ExternalAccessory/ExternalAccessory.h>
#import "SBJEV3Connection.h"

@class EV3Accessory;
extern "C" NSString* const LEGOAccessoryProtocol;

namespace SBJ
{
namespace EV3
{

class Log;

/*
 * This is the iOS specicific connection class
 */

class BluetoothConnectionIOS : public Connection
{
public:
	BluetoothConnectionIOS(Log& log, EAAccessory* accessory);
	
	virtual ~BluetoothConnectionIOS();
	
	virtual Type type() const;
	
	virtual void start(Read read);
	
	virtual bool write(const uint8_t* buffer, size_t len);
	
private:
	EV3Accessory* _delegate;
};
	
}
}
