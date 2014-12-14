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
	bool connected = _brick ? _brick->isConnected() : false;
	_connected.on = connected;
}

@end
