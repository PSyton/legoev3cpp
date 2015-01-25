//
//  SBJEV3WifiAnnouncerObjC.mm
//  Jove's Landing
//
//  Created by David Giovannini on 1/22/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#import "SBJEV3WifiAnnouncerObjC.h"
#import "SBJEV3WifiConnectionImplObjC.h"
#import "GCDAsyncUdpSocket.h"

using namespace SBJ::EV3;

@interface EV3WifiAnnouncer()<GCDAsyncUdpSocketDelegate>
{
	GCDAsyncUdpSocket* _udpSocket;
	WifiAccessoryCollection _collection;
	NSTimer* _timer;
}
@end

@implementation EV3WifiAnnouncer

- (id) init
{
	self = [super init];
	_udpSocket = [[GCDAsyncUdpSocket alloc] initWithDelegate:self delegateQueue:dispatch_get_main_queue()];
	return self;
}

- (void) start: (SBJEV3WifiAnnouncerChange) change
{
	_collection.start([change](auto key, auto accessory)
	{
		change(key, accessory);
	});
	
	NSError* error = nil;
	[_udpSocket bindToPort: 3015 error:&error];
	[_udpSocket beginReceiving:&error];
	
	_timer = [NSTimer scheduledTimerWithTimeInterval: 10.0 target:self selector:@selector(onTimer:) userInfo:nil repeats:NO];
}

- (void) onTimer: (NSObject*) userInfo
{
	_collection.evaluateStaleness();
}

- (void)udpSocket:(GCDAsyncUdpSocket *)sock didReceiveData:(NSData *)data
                                             fromAddress:(NSData *)address
                                       withFilterContext:(id)filterContext;
{
	NSString *host = nil;
	uint16_t port = 0;
	[GCDAsyncUdpSocket getHost:&host port:&port fromAddress:address];
	_collection.onUdpPacket(host.UTF8String, (const uint8_t*)data.bytes, data.length);
}

- (EV3ConnectionImpl*) findConnection: (SBJ::EV3::Log&)log identifier: (SBJ::EV3::DeviceIdentifier&) identifier
{
	WifiAccessory::Ptr accessory = _collection.findAccessory(identifier);
	EV3WifiConnectionImpl* impl = nil;
	if (accessory)
	{
		impl = [[EV3WifiConnectionImpl alloc] init: log withAccessory: accessory];
		if ([impl lockConnection] == false)
		{
			impl = nil;
		}
	}
	return impl;
}

- (void) prompt: (SBJ::EV3::PromptAccessoryErrored) errored
{
	auto m = _collection.accessories();
}

@end
