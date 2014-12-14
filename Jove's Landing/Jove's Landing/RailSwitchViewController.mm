//
//  SecondViewController.m
//  Jove's Landing
//
//  Created by David Giovannini on 12/14/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#import "RailSwitchViewController.h"
#import "RailSwitch.h"

#include "SBJEV3Brick.h"

using namespace SBJ::EV3;

@interface RailSwitchCell : UITableViewCell
@property (nonatomic) IBOutlet UILabel* name;
@property (nonatomic) IBOutlet UISwitch* open;
@end

@implementation RailSwitchCell
@end

@interface RailSwitchViewController ()
{
	Brick* _brick;
}

@end

@implementation RailSwitchViewController

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

- (void) updateUI
{
	bool connected = _brick ? _brick->isConnected() : false;
	CGFloat alpha = connected ? 1.0 : .5;
	
	NSUInteger cellCount = [self.tableView numberOfRowsInSection:0];
	
	for (NSInteger i = 0; i < cellCount; ++i)
	{
		RailSwitchCell* cell = (RailSwitchCell*)[self.tableView cellForRowAtIndexPath:[NSIndexPath indexPathForRow: i inSection: 0]];
		OutputPort port = (OutputPort)cell.tag;
		RailSwitch* sw = [RailSwitch switchForPort: port];
		
		cell.userInteractionEnabled = connected;
		cell.alpha = alpha;
		cell.name.text = sw.name;
		cell.open.on = sw.open;
	}
}

- (IBAction)switchToggled:(id)sender
{
	OutputPort port = (OutputPort)[sender tag];
	[[RailSwitch switchForPort: port] toggle];
}

@end
