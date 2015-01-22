//
//  EV3BluetoothConnectionImplIOS.m
//  Jove's Landing
//
//  Created by David Giovannini on 1/22/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#import "SBJEV3BluetoothConnectionImplIOS.h"

#import <ExternalAccessory/ExternalAccessory.h>

NSString* const LEGOAccessoryProtocol = @"COM.LEGO.MINDSTORMS.EV3";

using namespace SBJ::EV3;

@implementation EV3BluetoothConnectionImplIOS
{
	EAAccessory* _accessory;
	EASession* _session;
}

- (id) init: (SBJ::EV3::Log&) log withAccessory: (EAAccessory*) accessory
{
	self = [super init: log];
	_accessory = accessory;
	return self;
}

- (Connection::Type) type
{
	return Connection::Type::bluetooth;
}

- (void) close
{
	[super close];
	_session = nil;
	_accessory = nil;
}

- (void) createStreams
{
	if (_accessory)
	{
		_session = [[EASession alloc] initWithAccessory: _accessory forProtocol: LEGOAccessoryProtocol];
	}
}

- (NSInputStream*) inputStream
{
	return _session.inputStream;
}

- (NSOutputStream*) outputStream
{
	return _session.outputStream;
}

@end
