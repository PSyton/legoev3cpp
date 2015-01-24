//
//  SBJEV3WifiAnnouncerObjC.mm
//  Jove's Landing
//
//  Created by David Giovannini on 1/22/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#import "SBJEV3WifiAnnouncerObjC.h"
#import "SBJEV3WifiConnectionImplObjC.h"

using namespace SBJ::EV3;

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
	std::string udpPacket = "Serial-Number: 0016533d3414\x0d\x0aPort: 5555\x0d\x0aName: EV3\x0d\x0aProtocol: EV3\x0d\x0a";
	WifiAccessory::Ptr accessory(new WifiAccessory(
		"10.0.1.2",
		(const uint8_t*)udpPacket.c_str(), udpPacket.size()));
	
	EV3WifiConnectionImpl* impl = [[EV3WifiConnectionImpl alloc] init: log withAccessory: accessory];
	if ([impl lockConnection] == false)
	{
		impl = nil;
	}
	return impl;
}

- (void) prompt: (SBJ::EV3::PromptAccessoryErrored) errored
{
	std::string udpPacket = "Serial-Number: 0016533d3414\x0d\x0aPort: 5555\x0d\x0aName: EV3\x0d\x0aProtocol: EV3\x0d\x0a";
	WifiAccessory::Ptr accessory(new WifiAccessory(
		"10.0.1.2",
		(const uint8_t*)udpPacket.c_str(), udpPacket.size()));
	
	_change(accessory);
}

@end
