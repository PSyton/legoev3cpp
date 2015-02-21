//
//  SBJEV3BluetoothTransportListener.mm
//  Jove's Landing
//
//  Created by David Giovannini on 1/25/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#include "SBJEV3BluetoothTransportListener.h"
#include "SBJEV3ConnectionObjC.h"
#import "SBJEV3ConnectionToken.h"
#import "SBJEV3BluetoothConnectionImpliOS.h"

#import <Foundation/Foundation.h>
#import <ExternalAccessory/ExternalAccessory.h>

using namespace SBJ::EV3;

std::string serialFromAccessory(EAAccessory* accessory)
{
	// TODO: accessory.serialNumber is empty!
	return std::to_string(accessory.connectionID);
}

DeviceInfo infoFromAccessory(EAAccessory* accessory)
{
	static unsigned int name = 0;
	name++;
	
	DeviceInfo info;
	info.serial = serialFromAccessory(accessory);
	// TODO: accessory.name is always "MFI Accessory"!
	info.name = "BT EV3 " + std::to_string(name);
	info.firmwareVersion = accessory.firmwareRevision.UTF8String;
	info.hardwareVersion = accessory.hardwareRevision.UTF8String;
	info.firmwareBuild = accessory.modelNumber.UTF8String;
	return info;
}

@interface EV3BluetoothTransportListenerDelegate : NSObject
@end

@implementation EV3BluetoothTransportListenerDelegate
{
	TransportListener::Discovery _callback;
}

- (void) startWithcallback: (TransportListener::Discovery) callback
{
	_callback = callback;
	[self initialLoad];
	EAAccessoryManager* mgr = [EAAccessoryManager sharedAccessoryManager];
	[[NSNotificationCenter defaultCenter] addObserver: self selector:@selector(accessoryDidConnect:) name: EAAccessoryDidConnectNotification object: nil];
	[[NSNotificationCenter defaultCenter] addObserver: self selector:@selector(accessoryDidDisconnect:) name: EAAccessoryDidDisconnectNotification object: nil];
	[mgr registerForLocalNotifications];
}

- (void) initialLoad
{
	EAAccessoryManager* mgr = [EAAccessoryManager sharedAccessoryManager];
	NSArray* accesories = mgr.connectedAccessories;
	for (EAAccessory* accessory in accesories)
	{
		if ([accessory.protocolStrings containsObject: LEGOAccessoryProtocol])
		{
			DeviceInfo info = infoFromAccessory(accessory);
			_callback(ConnectionTransport::bluetooth, info.serial, &info);
		}
	}
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
	EAAccessory* accessory = notification.userInfo[EAAccessoryKey];
	DeviceInfo info = infoFromAccessory(accessory);
	_callback(ConnectionTransport::bluetooth, info.serial, &info);
}

- (void) accessoryDidDisconnect: (NSNotification*) notification
{
	EAAccessory* accessory = notification.userInfo[EAAccessoryKey];
	_callback(ConnectionTransport::none, serialFromAccessory(accessory), nullptr);
}

- (void) prompt: (PromptAccessoryErrored) errored
{
	EAAccessoryManager* mgr = [EAAccessoryManager sharedAccessoryManager];
	[mgr showBluetoothAccessoryPickerWithNameFilter: nil completion:^(NSError *error)
	{
		if (error)
		{
			switch (error.code)
			{
				case EABluetoothAccessoryPickerAlreadyConnected:
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

@end

static EV3BluetoothTransportListenerDelegate* _delegate;

BluetoothTransportListener::BluetoothTransportListener()
{
	_delegate = [[EV3BluetoothTransportListenerDelegate alloc] init];
}

BluetoothTransportListener::~BluetoothTransportListener()
{
	_delegate = nil;
}

void BluetoothTransportListener::startWithDiscovery(Discovery discovery)
{
	_discovery = discovery;
	[_delegate startWithcallback: _discovery];
}

void BluetoothTransportListener::prompt(PromptAccessoryErrored errored)
{
	[_delegate prompt: errored];
}

std::unique_ptr<Connection> BluetoothTransportListener::createConnection(Log& log, const std::string& serial)
{
	EAAccessoryManager* mgr = [EAAccessoryManager sharedAccessoryManager];
	NSArray* accessories = mgr.connectedAccessories;
	
	auto i = [accessories indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop)
	{
		return serialFromAccessory(obj) == serial;
	}];
	
	EV3BluetoothConnectionImpl* impl = nil;
	if (i != NSNotFound)
	{
		EAAccessory* accessory = accessories[i];
		if (accessory)
		{
			impl = [[EV3BluetoothConnectionImpl alloc] init: log withAccessory: accessory];
		}
	}
	return std::unique_ptr<Connection>(new ConnectionObjC(impl));
}
