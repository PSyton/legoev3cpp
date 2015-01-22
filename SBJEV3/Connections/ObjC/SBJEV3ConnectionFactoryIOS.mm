//
//  SBJEV3ConnectionFactory.cpp
//  LEGO Control
//
//  Created by David Giovannini on 12/1/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#include "SBJEV3ConnectionFactory.h"
#include "SBJEV3ConnectionObjC.h"

#import "SBJEV3WifiConnectionImplObjC.h"
#import "SBJEV3BluetoothConnectionImplIOS.h"

#import <ExternalAccessory/ExternalAccessory.h>

using namespace SBJ::EV3;

static EV3ConnectionImplObjC* findConnectionUsb(Log& log, DeviceIdentifier& identifier);
static EV3ConnectionImplObjC* findConnectionWifi(Log& log, DeviceIdentifier& identifier);
static EV3ConnectionImplObjC* findConnectionBluetooth(Log& log, DeviceIdentifier& identifier);

/*
 * We need to have an objective c object to receive OS notifications.
 * The notifications are immediately handed to the factory.
 */

@interface EV3NotificationReceiver : NSObject
@end

@implementation EV3NotificationReceiver
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
//	EAAccessory* accessory = notification.object;
//	if ([accessory.protocolStrings containsObject: LEGOAccessoryProtocol])
	{
		_factory->handleChangeInAccessoryConnection();
	}
}

- (void) accessoryDidDisconnect: (NSNotification*) notification
{
//	EAAccessory* accessory = notification.object;
//	if ([accessory.protocolStrings containsObject: LEGOAccessoryProtocol])
	{
		_factory->handleChangeInAccessoryConnection();
	}
}

@end

// We cheet with a global variable here instead of a c++ pimpl.
static EV3NotificationReceiver* _notificationReceiver = nil;

ConnectionFactory::ConnectionFactory(Log& log)
: _log(log)
{
#if (TARGET_IPHONE_SIMULATOR)
#else
	_notificationReceiver = [[EV3NotificationReceiver alloc] initWithFactory: this];
#endif
}

ConnectionFactory::~ConnectionFactory()
{
	_notificationReceiver = nil;
}

void ConnectionFactory::promptBluetooth(DeviceIdentifier identifier, PromptBluetoothErrored errored)
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
		// EABluetoothAccessoryPickerErrorDomain
		if (error)
		{
			switch (error.code)
			{
				case EABluetoothAccessoryPickerAlreadyConnected:
					handleChangeInAccessoryConnection();
					break;
				case EABluetoothAccessoryPickerResultCancelled:
					if (errored) errored(PromptBluetoothError::canceled);
					break;
				case EABluetoothAccessoryPickerResultNotFound:
					if (errored) errored(PromptBluetoothError::noBluetooth);
					break;
				case EABluetoothAccessoryPickerResultFailed:
					if (errored) errored(PromptBluetoothError::failureToConnect);
					break;
			}
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

std::unique_ptr<Connection> ConnectionFactory::findConnection(DeviceIdentifier& identifier)
{
	EV3ConnectionImplObjC* impl = nil;
	auto method = identifier.connect;
	
	if (method || DeviceIdentifier::ConnectMethod::only)
	{
		if (method || DeviceIdentifier::ConnectMethod::bluetoothFirst)
		{
			impl = findConnectionBluetooth(_log, identifier);
		}
		else if (method || DeviceIdentifier::ConnectMethod::wifiFirst)
		{
			impl = findConnectionWifi(_log, identifier);
		}
		else if (method || DeviceIdentifier::ConnectMethod::usbFirst)
		{
			impl = findConnectionUsb(_log, identifier);
		}
	}
	else
	{
		if (impl == nil and (method || DeviceIdentifier::ConnectMethod::bluetoothFirst))
		{
			impl = findConnectionBluetooth(_log, identifier);
		}
		if (impl == nil and (method || DeviceIdentifier::ConnectMethod::wifiFirst))
		{
			impl = findConnectionWifi(_log, identifier);
		}
		if (impl == nil and (method || DeviceIdentifier::ConnectMethod::usbFirst))
		{
			impl = findConnectionUsb(_log, identifier);
		}
	}
	if (impl)
	{
		return std::unique_ptr<Connection>(new ConnectionObjC(impl));
	}
	return nullptr;
}

#pragma mark - Find Connection Impls

static EV3ConnectionImplObjC* findConnectionUsb(Log& log, DeviceIdentifier& identifier)
{
	return nil;
}

static EV3ConnectionImplObjC* findConnectionWifi(Log& log, DeviceIdentifier& identifier)
{
	EV3WifiAccessory* accessory = [[EV3WifiAccessory alloc] init];
	accessory.serial = @"00123456789";
	accessory.host = @"10.0.1.2";
	accessory.port = 5555;
	return [[EV3WifiConnectionImplIOS alloc] init: log withAccessory: accessory];
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

static EV3ConnectionImplObjC* findConnectionBluetooth(Log& log, DeviceIdentifier& identifier)
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
		return [[EV3BluetoothConnectionImplIOS alloc] init: log withAccessory: accessory];
	}
	return nil;
}
