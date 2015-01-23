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

- (bool) connect: (SBJ::EV3::Log&) log
{
	log.write(LogDomian, "Wifi Open Lock");
	
	EV3WifiConnectionImpl* open = [[EV3WifiConnectionImpl alloc] init: log withAccessory: self];
	[open start: ^(const uint8_t* data, size_t size)
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
	[open write: (const uint8_t*)str.UTF8String len: str.length];
	
// Resolve
	{
		std::unique_lock<std::mutex> lock(_mutex);
		_isReady.wait_for(lock, std::chrono::milliseconds(2000), ^{return _acceptance != nil;});
		[open close];
		if (_acceptance != nil)
		{
			log.write(LogDomian, "Wifi Unlocked ", _acceptance);
			return true;
		}
		log.write(LogDomian, "Wifi failed to unlock ");
		return false;
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
