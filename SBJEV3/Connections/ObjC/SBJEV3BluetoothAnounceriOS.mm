//
//  SBJEV3BluetoothAnounceriOS.m
//  Jove's Landing
//
//  Created by David Giovannini on 1/22/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#import "SBJEV3BluetoothAnounceriOS.h"
#import "SBJEV3BluetoothConnectionImplIOS.h"
#import <ExternalAccessory/ExternalAccessory.h>

using namespace SBJ::EV3;

@implementation EV3BluetoothAnouncer
{
	EV3BluetoothAnouncerChange _change;
}

- (id) initWithChange: (EV3BluetoothAnouncerChange) change;
{
	self = [super init];
	_change = [change copy];
	EAAccessoryManager* mgr = [EAAccessoryManager sharedAccessoryManager];
	[[NSNotificationCenter defaultCenter] addObserver: self selector:@selector(accessoryDidConnect:) name: EAAccessoryDidConnectNotification object: nil];
	[[NSNotificationCenter defaultCenter] addObserver: self selector:@selector(accessoryDidDisconnect:) name: EAAccessoryDidDisconnectNotification object: nil];
	[mgr registerForLocalNotifications];
	return self;
}

- (void) dealloc
{
	EAAccessoryManager* mgr = [EAAccessoryManager sharedAccessoryManager];
	[[NSNotificationCenter defaultCenter] removeObserver: self name: EAAccessoryDidConnectNotification object: nil];
	[[NSNotificationCenter defaultCenter] removeObserver: self name: EAAccessoryDidDisconnectNotification object: nil];
	[mgr unregisterForLocalNotifications];
}

- (void) accessoryDidConnect: (NSNotification*) notification
{
//	EAAccessory* accessory = notification.object;
//	if ([accessory.protocolStrings containsObject: LEGOAccessoryProtocol])
	{
		_change();
	}
}

- (void) accessoryDidDisconnect: (NSNotification*) notification
{
//	EAAccessory* accessory = notification.object;
//	if ([accessory.protocolStrings containsObject: LEGOAccessoryProtocol])
	{
		_change();
	}
}

- (void) prompt: (PromptAccessoryErrored) errored
{
/*
#if (TARGET_IPHONE_SIMULATOR)
	// always connect in simulator
	dispatch_async(dispatch_get_main_queue(), ^
	{
		handleChangeInAccessoryConnection();
	});
#else
*/
	EAAccessoryManager* mgr = [EAAccessoryManager sharedAccessoryManager];
	[mgr showBluetoothAccessoryPickerWithNameFilter: nil completion:^(NSError *error)
	{
		// EABluetoothAccessoryPickerErrorDomain
		if (error)
		{
			switch (error.code)
			{
				case EABluetoothAccessoryPickerAlreadyConnected:
					_change();
					break;
				case EABluetoothAccessoryPickerResultCancelled:
					if (errored) errored(PromptAccessoryError::canceled);
					break;
				case EABluetoothAccessoryPickerResultNotFound:
					if (errored) errored(PromptAccessoryError::noConnectivity);
					break;
				case EABluetoothAccessoryPickerResultFailed:
					if (errored) errored(PromptAccessoryError::failureToConnect);
					break;
			}
		}
	}];
}

std::string fetchNameFromAccessory(EAAccessory* accessory)
{
// TODO: EAAccessory is lying
	return accessory.name.UTF8String;
}

std::string fetchSerialFromAccessory(EAAccessory* accessory)
{
// TODO: EAAccessory is lying
	return accessory.serialNumber.UTF8String;
}

- (EV3ConnectionImpl*) findConnection: (SBJ::EV3::Log&)log identifier: (SBJ::EV3::DeviceIdentifier&) identifier
{
	__block std::string foundName;
	__block std::string foundSerial;
	
	BOOL(^byProtocol)(id, NSUInteger, BOOL*) = ^(id obj, NSUInteger idx, BOOL *stop)
	{
		EAAccessory* accessory = obj;
		if ([accessory.protocolStrings containsObject: LEGOAccessoryProtocol])
		{
			foundName = fetchNameFromAccessory(accessory);
			foundSerial = fetchSerialFromAccessory(accessory);
			return YES;
		}
		return NO;
	};
	BOOL(^byName)(id, NSUInteger, BOOL*) = ^(id obj, NSUInteger idx, BOOL *stop)
	{
		EAAccessory* accessory = obj;
		if ([accessory.protocolStrings containsObject: LEGOAccessoryProtocol])
		{
			if (identifier.name == fetchNameFromAccessory(accessory))
			{
				foundName = identifier.name;
				foundSerial = fetchSerialFromAccessory(accessory);
				return YES;
			}
		}
		return NO;
	};
	BOOL(^bySerial)(id, NSUInteger, BOOL*) = ^(id obj, NSUInteger idx, BOOL *stop)
	{
		EAAccessory* accessory = obj;
		if ([accessory.protocolStrings containsObject: LEGOAccessoryProtocol])
		{
			if (identifier.serial == fetchSerialFromAccessory(accessory))
			{
				foundSerial = identifier.serial;
				foundName = fetchNameFromAccessory(accessory);
				return YES;
			}
		}
		return NO;
	};
	
	EAAccessoryManager* mgr = [EAAccessoryManager sharedAccessoryManager];
	NSArray* accessories = mgr.connectedAccessories;
	NSUInteger foundIt = NSNotFound;
	
	switch (identifier.search)
	{
		case DeviceIdentifier::SearchMethod::anyDevice:
			foundIt = [accessories indexOfObjectPassingTest:byProtocol];
			break;
		case DeviceIdentifier::SearchMethod::nameOnly:
			foundIt = [accessories indexOfObjectPassingTest:byName];
			break;
		case DeviceIdentifier::SearchMethod::serialOnly:
			foundIt = [accessories indexOfObjectPassingTest:bySerial];
			break;
		case DeviceIdentifier::SearchMethod::nameFirst:
			foundIt = [accessories indexOfObjectPassingTest:byName];
			if (foundIt == NSNotFound)
			{
				foundIt = [accessories indexOfObjectPassingTest:bySerial];
			}
			break;
		case DeviceIdentifier::SearchMethod::serialFirst:
			foundIt = [accessories indexOfObjectPassingTest:bySerial];
			if (foundIt == NSNotFound)
			{
				foundIt = [accessories indexOfObjectPassingTest:byName];
			}
			break;
	}
	
	if (foundIt != NSNotFound)
	{
		EAAccessory* accessory = accessories[foundIt];
		identifier.name = foundName;
		identifier.serial = foundSerial;
		identifier.connection.makePriority(ConnectionTransport::bluetooth);
		return [[EV3BluetoothConnectionImpl alloc] init: log withAccessory: accessory];
	}
	return nil;
}

@end
