//
//  SBJEV3ConnectionFactory.cpp
//  LEGO Control
//
//  Created by David Giovannini on 12/1/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#include "SBJEV3ConnectionFactory.h"
#include "SBJEV3ConnectionIOS.h"
#import <ExternalAccessory/ExternalAccessory.h>

using namespace SBJ::EV3;

/*
 * We need to have an objective c object to receive OS notifications.
 * The notifications are immediately handed to the factory.
 */

@interface NotificationReceiver : NSObject
@end

@implementation NotificationReceiver
{
	ConnectionFactory* _factory;
}

- (id) initWithFactory: (ConnectionFactory*) factory
{
	self = [super init];
	_factory = factory;
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

- (void) accessoryDidConnect: (id) v
{
	_factory->handleChangeInAccessoryConnection();
}

- (void) accessoryDidDisconnect: (id) v
{
	_factory->handleChangeInAccessoryConnection();
}

@end

// We cheet with a global variable here instead of a c++ pimpl.
static NotificationReceiver* _notificationReceiver = nil;

ConnectionFactory::ConnectionFactory()
{
#if (TARGET_IPHONE_SIMULATOR)
#else
	_notificationReceiver = [[NotificationReceiver alloc] initWithFactory: this];
#endif
}

ConnectionFactory::~ConnectionFactory()
{
	_notificationReceiver = nil;
}

void ConnectionFactory::promptBluetooth(DeviceIdentifier identifier, PromptBluetoothCompleted completion)
{
#if (TARGET_IPHONE_SIMULATOR)
	// always connect in simulator
	dispatch_async(dispatch_get_main_queue(), ^
	{
		handleChangeInAccessoryConnection();
	});
#else
	EAAccessoryManager* mgr = [EAAccessoryManager sharedAccessoryManager];
	[mgr showBluetoothAccessoryPickerWithNameFilter: nil completion:^(NSError *error)
	{
		// TODO: differentiate between user cancel and pairing error
		if (completion) completion(error != nil);
	}];
#endif
}

void ConnectionFactory::registerNotification(ConnectionToken* token)
{
	_tokens.insert(token);
	DeviceIdentifier identifier = token->identifier();
	Connection* testConnection(findConnection(identifier));
	if (token->makeConnection(identifier, testConnection) == false)
	{
		delete testConnection;
	}
}

void ConnectionFactory::unregisterNotification(ConnectionToken* token)
{
	_tokens.erase(token);
}

void ConnectionFactory::handleChangeInAccessoryConnection()
{
	for (auto item = _tokens.begin(); item != _tokens.end(); item++)
	{
		DeviceIdentifier identifier = (*item)->identifier();
		Connection* testConnection(findConnection(identifier));
		dispatch_async(dispatch_get_main_queue(), ^
		{
			if ((*item)->makeConnection(identifier, testConnection) == false)
			{
				delete testConnection;
			}
		});
	}
}

Connection* ConnectionFactory::findConnection(DeviceIdentifier& identifier)
{
#if (TARGET_IPHONE_SIMULATOR)
	// Always connect in simulator
	identifier.name = "Simulated";
	return new ConnectionIOS(nullptr);
#else
	if (identifier.connect == DeviceIdentifier::ConnectMethod::usbOnly)
	{
		return nullptr;
	}
	
	NSString* requestedName = [NSString stringWithUTF8String: identifier.name.c_str()];
	NSString* requestedSerial = [NSString stringWithUTF8String: identifier.serial.c_str()];
	
	BOOL(^byProtocol)(id, NSUInteger, BOOL*) = ^(id obj, NSUInteger idx, BOOL *stop)
	{
		EAAccessory* accessory = obj;
		if ([accessory.protocolStrings containsObject: LEGOAccessoryProtocol])
		{
			return YES;
		}
		return NO;
	};
	BOOL(^byName)(id, NSUInteger, BOOL*) = ^(id obj, NSUInteger idx, BOOL *stop)
	{
		EAAccessory* accessory = obj;
		if ([accessory.protocolStrings containsObject: LEGOAccessoryProtocol])
		{
			if ([accessory.name isEqualToString: requestedName])
			{
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
			if ([accessory.serialNumber isEqualToString: requestedSerial])
			{
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
		// TODO: populate identifier missing fields
		EAAccessory* accessory = accessories[foundIt];
		return new ConnectionIOS(accessory);
	}
	return nullptr;
#endif
}
