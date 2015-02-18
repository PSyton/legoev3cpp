//
//  SBJEV3WifiTransportListener.cpp
//  Jove's Landing
//
//  Created by David Giovannini on 1/25/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#include "SBJEV3WifiTransportListener.h"
#include "SBJEV3DeviceIdentifier.h"
#include "SBJEV3ConnectionObjC.h"
#include "SBJEV3Connection.h"
#import "SBJEV3WifiConnectionImplObjC.h"
#import "GCDAsyncUdpSocket.h"

using namespace SBJ::EV3;

@interface EV3WifiTransportListenerDelegate : NSObject
{
	WifiTransportListener* _callback;
	GCDAsyncUdpSocket* _udpSocket;
	NSTimer* _timer;
}
@end

@implementation EV3WifiTransportListenerDelegate

- (id) init
{
	self = [super init];
	_udpSocket = [[GCDAsyncUdpSocket alloc] initWithDelegate:self delegateQueue:dispatch_get_main_queue()];
	return self;
}

- (void) startWithcallback: (WifiTransportListener*) callback
{
	_callback = callback;
	
	NSError* error = nil;
	[_udpSocket bindToPort: 3015 error:&error];
	[_udpSocket beginReceiving:&error];
	
	_timer = [NSTimer scheduledTimerWithTimeInterval: 10.0 target:self selector:@selector(onTimer:) userInfo:nil repeats:NO];
}

- (void) onTimer: (NSObject*) userInfo
{
	_callback->evaluateStaleness();
}

- (void)udpSocket:(GCDAsyncUdpSocket *)sock didReceiveData:(NSData *)data
                                             fromAddress:(NSData *)address
                                       withFilterContext:(id)filterContext;
{
	NSString *host = nil;
	uint16_t port = 0;
	[GCDAsyncUdpSocket getHost:&host port:&port fromAddress:address];
	_callback->onUdpPacket(host.UTF8String, (const uint8_t*)data.bytes, data.length);
}

@end

static EV3WifiTransportListenerDelegate* _delegate;

WifiTransportListener::WifiTransportListener()
{
	_delegate = [[EV3WifiTransportListenerDelegate alloc] init];
}

void WifiTransportListener::startWithDiscovery(Discovery discovery)
{
	_discovery = discovery;
	[_delegate startWithcallback: this];
}

WifiTransportListener::~WifiTransportListener()
{
	_delegate = nil;
}

std::unique_ptr<Connection> WifiTransportListener::createConnection(Log& log, const std::string& serial)
{
	WifiAccessory::Ptr accessory(_accessories[serial]);

	EV3WifiConnectionImpl* impl = nil;
	if (accessory)
	{
		impl = [[EV3WifiConnectionImpl alloc] init: log withAccessory: accessory];
		if ([impl lockConnection] == false)
		{
			impl = nil;
		}
	}
	return std::unique_ptr<Connection>(new ConnectionObjC(impl));
}
