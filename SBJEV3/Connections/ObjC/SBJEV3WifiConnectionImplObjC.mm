//
//  SBJEV3WifiConnectionImplObjC.m
//  Jove's Landing
//
//  Created by David Giovannini on 1/22/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#import "SBJEV3WifiConnectionImplObjC.h"

using namespace SBJ::EV3;

static const std::string LogDomian = "Connect";

@implementation EV3WifiConnectionImpl
{
	WifiAccessory::Ptr _accessory;
	NSInputStream* _inputStream;
	NSOutputStream* _outputStream;
}

- (id) init: (SBJ::EV3::Log&) log withAccessory: (SBJ::EV3::WifiAccessory::Ptr&) accessory
{
	self = [super init: log];
	_accessory = std::move(accessory);
	return self;
}

- (bool) lockConnection
{
	std::mutex _mutex;
	[self start: ^(const uint8_t* data, size_t size)
// Response
	{
		_accessory->tryUnlock(data, size);
	}];
	
// Request
	std::string request = _accessory->unlockRequest();
	[self write: (const uint8_t*)request.c_str() len: request.size()];
	
// Resolve
	{
		std::unique_lock<std::mutex> lock(_mutex);
		if (_accessory->waitUnlocked() == false)
		{
			[self close];
			return false;
		}
		return true;
	}
}

- (ConnectionTransport) transport
{
	return ConnectionTransport::wifi;
}

- (void) createStreams
{
	if (_accessory)
	{
		CFReadStreamRef readStream;
		CFWriteStreamRef writeStream;
		CFStreamCreatePairWithSocketToHost(NULL, (__bridge CFStringRef)[NSString stringWithUTF8String: _accessory->host().c_str()], _accessory->port(), &readStream, &writeStream);
		_inputStream = (__bridge NSInputStream *)readStream;
		_outputStream = (__bridge NSOutputStream *)writeStream;
	}
}

- (NSInputStream*) inputStream
{
	return _inputStream;
}

- (NSOutputStream*) outputStream
{
	return _outputStream;
}

@end
