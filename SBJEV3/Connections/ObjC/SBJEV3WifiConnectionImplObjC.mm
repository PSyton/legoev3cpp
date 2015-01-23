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

@implementation EV3WifiAccessory
{
	std::mutex _mutex;
	std::condition_variable _isReady;
}

- (bool) connect: (EV3WifiConnectionImpl*) impl
{
	[impl start: ^(const uint8_t* data, size_t size)
// Response
	{
		char response[17];
		response[16] = 0;
		::memcpy(response, data, std::min(size, sizeof(response)-1));
		bool accepted = false;
		if (memcmp(response, "Accept:", 7) == 0)
		{
			std::unique_lock<std::mutex> lock(_mutex);
			_acceptance = [[NSString stringWithUTF8String: response + 7] stringByTrimmingCharactersInSet:
                              [NSCharacterSet whitespaceAndNewlineCharacterSet]];
			accepted = true;
		}
		if (accepted)
		{
			_isReady.notify_all();
		}
	}];
	
// Request
	NSString* str = [NSString stringWithFormat: @"GET /target?sn=%@VMTP1.0\x0d\x0aProtocol:%@\x0d\x0a", _serial, _protocol];
	const char* data = str.UTF8String;
	[impl write: (const uint8_t*)data len: str.length];
	
// Resolve
	{
		std::unique_lock<std::mutex> lock(_mutex);
		_isReady.wait_for(lock, std::chrono::milliseconds(2000), ^{return _acceptance != nil;});
		if (_acceptance == nil)
		{
			[impl close];
			return false;
		}
		return true;
	}
}

@end

@implementation EV3WifiConnectionImpl
{
	EV3WifiAccessory* _accessory;
	NSInputStream* _inputStream;
	NSOutputStream* _outputStream;
	bool _accepted;
}

- (id) init: (SBJ::EV3::Log&) log withAccessory: (EV3WifiAccessory*) accessory
{
	self = [super init: log];
	_accessory = accessory;
	return self;
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
		CFStreamCreatePairWithSocketToHost(NULL, (__bridge CFStringRef)_accessory.host, _accessory.port, &readStream, &writeStream);
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
