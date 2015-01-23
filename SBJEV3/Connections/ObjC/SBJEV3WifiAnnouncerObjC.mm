//
//  SBJEV3WifiAnnouncerObjC.mm
//  Jove's Landing
//
//  Created by David Giovannini on 1/22/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#import "SBJEV3WifiAnnouncerObjC.h"
#import "SBJEV3WifiConnectionImplObjC.h"

@implementation EV3WifiAnnouncer
{
	SBJEV3WifiAnnouncerChange _change;
}

- (id) initWithChange: (SBJEV3WifiAnnouncerChange) change
{
	self = [super init];
	_change = [change copy];
	return self;
}

- (EV3ConnectionImpl*) findConnection: (SBJ::EV3::Log&)log identifier: (SBJ::EV3::DeviceIdentifier&) identifier
{
	EV3WifiAccessory* accessory = [[EV3WifiAccessory alloc] init];
	accessory.serial = @"0016533d3414";
	accessory.host = @"10.0.1.2";
	accessory.port = 5555;
	accessory.protocol = @"EV3";
	
	EV3WifiConnectionImpl* impl = [[EV3WifiConnectionImpl alloc] init: log withAccessory: accessory];
	if ([accessory connect: impl] == false)
	{
		impl = nil;
	}
	return impl;
}

- (void) prompt: (SBJ::EV3::PromptAccessoryErrored) errored
{
	EV3WifiAccessory* accessory = [[EV3WifiAccessory alloc] init];
	accessory.serial = @"0016533d3414";
	accessory.host = @"10.0.1.2";
	accessory.port = 5555;
	accessory.protocol = @"EV3";
	
	_change(accessory.serial, accessory);
}

@end
