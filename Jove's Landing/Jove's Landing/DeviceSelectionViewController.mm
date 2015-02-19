//
//  DeviceSelectionViewController.m
//  Jove's Landing
//
//  Created by David Giovannini on 12/14/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#import "DeviceSelectionViewController.h"
#include "SBJEV3Brick.h"
#include "SBJEV3Log.h"

using namespace SBJ::EV3;

@interface DeviceSelectionViewController ()<UITextFieldDelegate>
{
	Brick* _brick;
	IBOutlet UITableViewCell* _name;
	IBOutlet UITableViewCell* _serial;
	IBOutlet UIProgressView* _battery;
	IBOutlet UIImageView* _connectType;
	
	IBOutlet UITableViewCell* _selectName;
	IBOutlet UITableViewCell* _selectSerial;
	IBOutlet UITableViewCell* _selectUsb;
	IBOutlet UITableViewCell* _selectWifi;
	IBOutlet UITableViewCell* _selectBluetooth;
}

@end

@implementation DeviceSelectionViewController

- (void)viewDidLoad 
{
	[super viewDidLoad];
	self.tableView.contentInset = UIEdgeInsetsMake(20, 0, 0, 0);
	
	UITapGestureRecognizer* tap = [[UITapGestureRecognizer alloc] initWithTarget: self action: @selector(renameBrick:)];
	tap.numberOfTapsRequired = 2;
	[_name addGestureRecognizer: tap];
	[self.tableView setEditing: YES];
	
	self.refreshControl = [[UIRefreshControl alloc] init];
    self.refreshControl.backgroundColor = [UIColor purpleColor];
    self.refreshControl.tintColor = [UIColor whiteColor];
    [self.refreshControl addTarget:self action:@selector(selectDevice) forControlEvents:UIControlEventValueChanged];

	 [self load];
}

- (void) load
{
	DeviceIdentifier identifier;
	TransportSelection selection(ConnectionTransport::none);
	NSUserDefaults* userDefaults = [NSUserDefaults standardUserDefaults];
	NSDictionary* identifierValues = [userDefaults objectForKey: @"identifierValues"];
	if (identifierValues.count)
	{
		identifier.name = [identifierValues[@"name"] UTF8String];
		identifier.serial = [identifierValues[@"serial"] UTF8String];
		identifier.search = (DeviceIdentifier::SearchMethod)[identifierValues[@"search"] integerValue];
		NSArray* transports = identifierValues[@"transports"];
		for (NSNumber* transport in transports)
		{
			selection.insert((ConnectionTransport)transport.integerValue);
		}
		identifier.transports = selection;
	}
	[self setUIFromIdentifier: identifier];
}

- (DeviceIdentifier) save
{
	DeviceIdentifier identifier = [self buildIdentifier];
	NSDictionary* identifierValues = @
	{
		@"name" : [NSString stringWithUTF8String: identifier.name.c_str()],
		@"serial" : [NSString stringWithUTF8String: identifier.serial.c_str()],
		@"search" : @((int)identifier.search),
		@"transports" : @
		[
			@((int)identifier.transports[0]),
			@((int)identifier.transports[1]),
			@((int)identifier.transports[2]),
		]
	};
	NSUserDefaults* userDefaults = [NSUserDefaults standardUserDefaults];
	[userDefaults setObject: identifierValues forKey: @"identifierValues"];
	[userDefaults synchronize];
	return identifier;
}

- (void) setBrick: (SBJ::EV3::Brick*) brick
{
	_brick = brick;
	if (self.isViewLoaded)
	{
		[self updateUI];
	}
}

- (void) viewWillAppear:(BOOL)animated
{
	[super viewWillAppear:animated];
	[self updateUI];
}

#pragma mark - Device UI

- (void) updateUI
{
	if (_brick)
	{
		// Seriously Apple? Setting the text to empty string makes future setting invalid!
		_name.detailTextLabel.text = [NSString stringWithUTF8String: (_brick->name() + " ").c_str()];
		_serial.detailTextLabel.text = [NSString stringWithUTF8String: (_brick->serialNumber() + " ").c_str()];
		switch (_brick->activeTransport())
		{
			case ConnectionTransport::none:
				_connectType.image = [UIImage imageNamed: @"None"];
				break;
			case ConnectionTransport::usb:
				_connectType.image = [UIImage imageNamed: @"USB"];
				break;
			case ConnectionTransport::wifi:
				_connectType.image = [UIImage imageNamed: @"WIFI"];
				break;
			case ConnectionTransport::bluetooth:
				_connectType.image = [UIImage imageNamed: @"Bluetooth"];
				break;
		}
		if (_brick->isConnected())
		{
			Brick::Battery battery = _brick->battery();
			_battery.progress = battery.indicator();
		}
		else
		{
			_battery.progress = 0.0;
		}
	}
	else
	{
		_name.detailTextLabel.text = @"N/A";
		_serial.detailTextLabel.text = @"N/A";
		_connectType.image = [UIImage imageNamed: @"None"];
		_battery.progress = 0.0;
	}
}

- (void) renameBrick: (UITapGestureRecognizer*) recognizer
{
	if (_brick and _brick->isConnected())
	{
		UIAlertController* alert = [UIAlertController alertControllerWithTitle: @"Name" message: @"Up to 31 Characters" preferredStyle: UIAlertControllerStyleAlert];
		[alert addAction: [UIAlertAction actionWithTitle: @"OK" style: UIAlertActionStyleDefault handler: ^(UIAlertAction *action)
		{
			if ([[alert.textFields[0] text] length])
			{
				_brick->setName([[alert.textFields[0] text] UTF8String]);
				[self updateUI];
			}
		}]];
		[alert addAction: [UIAlertAction actionWithTitle: @"Cancel" style: UIAlertActionStyleCancel handler: nil]];
		[alert addTextFieldWithConfigurationHandler:^(UITextField *textField)
		{
			textField.placeholder = [NSString stringWithUTF8String: _brick->name().c_str()];
		}];
		[self presentViewController: alert animated: YES completion: nil];
	}
}

#pragma mark - Device Selection

- (DeviceIdentifier) buildIdentifier
{
	DeviceIdentifier identifier;
	identifier.name = [[(UILabel*)[_selectName viewWithTag: 2] text] UTF8String];
	identifier.serial = [[(UILabel*)[_selectSerial viewWithTag: 2] text] UTF8String];
	
	identifier.search = DeviceIdentifier::SearchMethod::anyDevice;
	if ([(UISwitch*)[_selectName viewWithTag: 1] isOn])
	{
		identifier.search |= DeviceIdentifier::SearchMethod::nameCheck;
	}
	if ([(UISwitch*)[_selectSerial viewWithTag: 1] isOn])
	{
		identifier.search |= DeviceIdentifier::SearchMethod::serialCheck;
	}
	if ([self.tableView indexPathForCell: _selectSerial].row == 0)
	{
		identifier.search |= DeviceIdentifier::SearchMethod::serialFirst;
	}
	
	auto transports = TransportSelection(ConnectionTransport::none);
	for (int i = 0; i < 3; i++)
	{
		UITableViewCell* test = [self.tableView cellForRowAtIndexPath: [NSIndexPath indexPathForRow: i inSection: 1]];
		if ([(UISwitch*)[test viewWithTag: 1] isOn])
		{
			if (test == _selectUsb)
			{
				transports.insert(ConnectionTransport::usb);
			}
			else if (test == _selectWifi)
			{
				transports.insert(ConnectionTransport::wifi);
			}
			else if (test == _selectBluetooth)
			{
				transports.insert(ConnectionTransport::bluetooth);
			}
		}
	}
	identifier.transports = transports;
	return identifier;
}

- (void) setUIFromIdentifier: (DeviceIdentifier) identifier
{
	bool checkName = (identifier.search || DeviceIdentifier::SearchMethod::nameCheck);
	bool checkSerial = (identifier.search || DeviceIdentifier::SearchMethod::serialCheck);
	bool serialFirst = (identifier.search || DeviceIdentifier::SearchMethod::serialFirst);
	
	[(UILabel*)[_selectName viewWithTag: 2] setText: [[NSString alloc] initWithUTF8String: identifier.name.c_str()]];
	[(UILabel*)[_selectSerial viewWithTag: 2] setText: [[NSString alloc] initWithUTF8String: identifier.serial.c_str()]];
	
	[(UISwitch*)[_selectName viewWithTag: 1] setOn: checkName];
	[(UISwitch*)[_selectSerial viewWithTag: 1] setOn: checkSerial];
	
	[(UISwitch*)[_selectUsb viewWithTag: 1] setOn: identifier.transports.find(ConnectionTransport::usb)];
	[(UISwitch*)[_selectWifi viewWithTag: 1] setOn: identifier.transports.find(ConnectionTransport::wifi)];
	[(UISwitch*)[_selectBluetooth viewWithTag: 1] setOn: identifier.transports.find(ConnectionTransport::bluetooth)];
	
	if (serialFirst)
	{
		NSIndexPath* source = [self.tableView indexPathForCell: _selectSerial];
		[self.tableView moveRowAtIndexPath: source toIndexPath: [NSIndexPath indexPathForRow: 0 inSection: 0]];
	}
	else
	{
		NSIndexPath* source = [self.tableView indexPathForCell: _selectName];
		[self.tableView moveRowAtIndexPath: source toIndexPath: [NSIndexPath indexPathForRow: 0 inSection: 0]];
	}
	int r = 0;
	for (auto transport : identifier.transports)
	{
		NSIndexPath* dest = [NSIndexPath indexPathForRow: r inSection: 1];
		r++;
		NSIndexPath* src = nil;
		switch (transport)
		{
			case ConnectionTransport::none:
				continue;
			case ConnectionTransport::usb:
				src = [self.tableView indexPathForCell: _selectUsb];
				break;
			case ConnectionTransport::wifi:
				src = [self.tableView indexPathForCell: _selectWifi];
				break;
			case ConnectionTransport::bluetooth:
				src = [self.tableView indexPathForCell: _selectBluetooth];
				break;
		}
		[self.tableView moveRowAtIndexPath: src toIndexPath: dest];
	}
}

- (void) selectDevice
{
	[self.refreshControl beginRefreshing];
	_brick->fetchDevice([self save]);
	[self updateUI];
	[self.refreshControl endRefreshing];
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
	[textField resignFirstResponder];
	return NO;
}

#pragma mark - TableView Delegate

- (void)tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)sourceIndexPath toIndexPath:(NSIndexPath *)destinationIndexPath
{
}

- (BOOL)tableView:(UITableView *)tableView shouldIndentWhileEditingRowAtIndexPath:(NSIndexPath *)indexPath
{
	return NO;
}

- (BOOL) tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath
{
	return indexPath.section != 2;
}

- (UITableViewCellEditingStyle)tableView:(UITableView *)tableView editingStyleForRowAtIndexPath:(NSIndexPath *)indexPath
{
	return UITableViewCellEditingStyleNone;
}

- (NSIndexPath *)tableView:(UITableView *)tableView targetIndexPathForMoveFromRowAtIndexPath:(NSIndexPath *)sourceIndexPath toProposedIndexPath:(NSIndexPath *)proposedDestinationIndexPath
{
	if (sourceIndexPath.section == 2)
	{
		return  sourceIndexPath;
	}
	if (proposedDestinationIndexPath.section < sourceIndexPath.section)
	{
		return  [NSIndexPath indexPathForRow: 0 inSection: sourceIndexPath.section];
	}
	if (proposedDestinationIndexPath.section > sourceIndexPath.section)
	{
		return  [NSIndexPath indexPathForRow: [self tableView: tableView numberOfRowsInSection: sourceIndexPath.section] - 1 inSection: sourceIndexPath.section];
	}
	return proposedDestinationIndexPath;
}

@end
