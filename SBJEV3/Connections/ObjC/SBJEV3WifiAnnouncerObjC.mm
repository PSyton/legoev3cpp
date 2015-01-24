//
//  SBJEV3WifiAnnouncerObjC.mm
//  Jove's Landing
//
//  Created by David Giovannini on 1/22/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#import "SBJEV3WifiAnnouncerObjC.h"
#import "SBJEV3WifiConnectionImplObjC.h"
#import "AsyncUdpSocket.h"

using namespace SBJ::EV3;

@interface EV3WifiAnnouncer()<AsyncUdpSocketDelegate>
{
	AsyncUdpSocket* _socket;
	WifiAccessoryCollection _collection;
}
@end

@implementation EV3WifiAnnouncer

- (id) init
{
	self = [super init];
	_socket = [[AsyncUdpSocket alloc] initWithDelegate:self];
	return self;
}

- (void) start: (SBJEV3WifiAnnouncerChange) change
{
	_collection.start([change](auto key, auto accessory)
	{
		change(key, accessory);
	});
	
	std::string udpPacket = "Serial-Number: 0016533d3414\x0d\x0aPort: 5555\x0d\x0aName: EV3\x0d\x0aProtocol: EV3\x0d\x0a";
	NSData* data = [NSData dataWithBytes: udpPacket.c_str() length: udpPacket.size()];
	[self onUdpSocket: _socket didReceiveData: data withTag: 0 fromHost: @"10.0.1.2" port: 12345];
}

- (void) onTimer
{
	_collection.ping();
}

- (BOOL)onUdpSocket:(AsyncUdpSocket *)sock
     didReceiveData:(NSData *)data
            withTag:(long)tag
           fromHost:(NSString *)host
               port:(UInt16)port
{
	WifiAccessory::State discovered = _collection.onUdpPacket(host.UTF8String, (const uint8_t*)data.bytes, data.length);
	return discovered != WifiAccessory::State::errored;
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
