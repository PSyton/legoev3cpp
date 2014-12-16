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

- (void) accessoryDidConnect: (NSNotification*) notification
{
	EAAccessory* accessory = notification.object;
	if ([accessory.protocolStrings containsObject: LEGOAccessoryProtocol])
	{
		_factory->handleChangeInAccessoryConnection();
	}
}

- (void) accessoryDidDisconnect: (NSNotification*) notification
{
	EAAccessory* accessory = notification.object;
	if ([accessory.protocolStrings containsObject: LEGOAccessoryProtocol])
	{
		_factory->handleChangeInAccessoryConnection();
	}
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
		// Device already connected but reselected
		if (error && error.code == 0)
		{
			// Rebroadcast connection event
			handleChangeInAccessoryConnection();
		}
		// User pressed cancel
		else if (error.code == 2)
		{
			if (completion) completion(true);
		}
		else
		{
			// TODO: expose error
			if (completion) completion(true);
		}
	}];
#endif
}

void ConnectionFactory::registerNotification(ConnectionToken* token)
{
	_tokens.insert(token);
	DeviceIdentifier identifier = token->identifier();
	std::unique_ptr<Connection> testConnection(findConnection(identifier));
	token->makeConnection(identifier, testConnection);
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
		__block std::unique_ptr<Connection> testConnection(findConnection(identifier));
		dispatch_async(dispatch_get_main_queue(), ^
		{
			(*item)->makeConnection(identifier, testConnection);
		});
	}
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

std::unique_ptr<Connection> ConnectionFactory::findConnection(DeviceIdentifier& identifier)
{
#if (TARGET_IPHONE_SIMULATOR)
	identifier.name = "Simulated";
	return std::unique_ptr<Connection>(new ConnectionIOS(nullptr));
#else
	if (identifier.connect == DeviceIdentifier::ConnectMethod::usbOnly)
	{
		return nullptr;
	}
	
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
		return std::unique_ptr<Connection>(new ConnectionIOS(accessory));
	}
	return nullptr;
#endif
}
