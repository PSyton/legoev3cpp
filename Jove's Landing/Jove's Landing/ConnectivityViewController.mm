//
//  FirstViewController.m
//  Jove's Landing
//
//  Created by David Giovannini on 12/14/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#import "ConnectivityViewController.h"
#include "SBJEV3Brick.h"

using namespace SBJ::EV3;

@interface ConnectivityViewController ()
{
	Brick* _brick;
	IBOutlet UISwitch* _connected;
	IBOutlet UILabel* _name;
	IBOutlet UILabel* _serial;
	IBOutlet UIImageView* _connectType;
}

@end

@implementation ConnectivityViewController

- (void)viewDidLoad 
{
	[super viewDidLoad];
	self.tableView.contentInset = UIEdgeInsetsMake(20, 0, 0, 0);
	[self updateUI];
}

- (void) setBrick: (SBJ::EV3::Brick*) brick
{
	_brick = brick;
	[self updateUI];
}

- (IBAction) reconnect:(id)sender
{
	if (_connected.isOn)
	{
		_brick->promptForBluetooth(^(Brick& brick, bool canceled)
		{
			if (canceled) [self updateUI];
		});
	}
	else
	{
		_brick->disconnect();
	}
}

- (void) updateUI
{
	if (_brick)
	{
		_connected.on = _brick->isConnected();
		_name.text = [NSString stringWithUTF8String: _brick->identifier().name.c_str()];
		_serial.text = [NSString stringWithUTF8String: _brick->identifier().serial.c_str()];
		switch (_brick->connectionType())
		{
			case Connection::Type::usb:
				_connectType.image = [UIImage imageNamed: @"USB"];
				break;
			case Connection::Type::bluetooth:
				_connectType.image = [UIImage imageNamed: @"Bluetooth"];
				break;
			case Connection::Type::wifi:
				_connectType.image = [UIImage imageNamed: @"WIFI"];
				break;
			case Connection::Type::simulator:
				_connectType.image = [UIImage imageNamed: @"Simulator"];
				break;
			case Connection::Type::none:
				_connectType.image = [UIImage imageNamed: @"None"];
				break;
		}
	}
	else
	{
		_connected.on = false;
		_name.text = @"N/A";
		_serial.text = @"N/A";
		_connectType.image = [UIImage imageNamed: @"None"];
	}
}

@end
