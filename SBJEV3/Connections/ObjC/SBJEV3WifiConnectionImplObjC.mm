//
//  SBJEV3WifiConnectionImplObjC.m
//  Jove's Landing
//
//  Created by David Giovannini on 1/22/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#import "SBJEV3WifiConnectionImplObjC.h"

using namespace SBJ::EV3;

@implementation EV3WifiAccessory
@end

@implementation EV3WifiConnectionImplIOS
{
	EV3WifiAccessory* _accessory;
	NSInputStream* _inputStream;
	NSOutputStream* _outputStream;
}

- (id) init: (SBJ::EV3::Log&) log withAccessory: (EV3WifiAccessory*) accessory
{
	self = [super init: log];
	_accessory = accessory;
	return self;
}

- (Connection::Type) type
{
	return Connection::Type::wifi;
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
