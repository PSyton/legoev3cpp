//
//  DiscoveredViewController.m
//  Jove's Landing
//
//  Created by David Giovannini on 2/12/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#import "DiscoveredViewController.h"

using namespace SBJ::EV3;

@interface DiscoveredCell : UITableViewCell
@property (nonatomic) IBOutlet UILabel* name;
@property (nonatomic) IBOutlet UISwitch* wifi;
@property (nonatomic) IBOutlet UISwitch* bluetooth;
@property (nonatomic) IBOutlet UISwitch* usb;
@property (nonatomic) DiscoveredDevice::Ptr device;
@end

@implementation DiscoveredCell

- (void) setDevice:(DiscoveredDevice::Ptr)device
{
	_device = device;
	_name.text = [NSString stringWithUTF8String: device->name().c_str()];
	_wifi.enabled = device->hasTransport(ConnectionTransport::wifi);
	_bluetooth.enabled = device->hasTransport(ConnectionTransport::bluetooth);
	_usb.enabled = device->hasTransport(ConnectionTransport::usb);
	_wifi.on = device->isConnected(ConnectionTransport::wifi);
	_bluetooth.on = device->isConnected(ConnectionTransport::bluetooth);
	_usb.on = device->isConnected(ConnectionTransport::usb);
}

- (IBAction) wifiChange:(id)sender
{
	_device->setIsConnected(ConnectionTransport::wifi, _wifi.on);
}

- (IBAction) bluetoothChange:(id)sender
{
	_device->setIsConnected(ConnectionTransport::bluetooth, _bluetooth.on);
}

- (IBAction) usbChange:(id)sender
{
	_device->setIsConnected(ConnectionTransport::usb, _usb.on);
}

@end

@interface DiscoveredViewController ()
{
	ConnectionFactory* _connectionFactory;
	std::vector<DiscoveredDevice::Ptr> _discovered;
}
@end

@implementation DiscoveredViewController

- (void) setConnectionFactory: (SBJ::EV3::ConnectionFactory*) connectionFactory
{
	_connectionFactory = connectionFactory;
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

- (void) updateUI
{
	if (_connectionFactory)
	{
		_discovered = _connectionFactory->getDiscovered();
		[self.tableView reloadData];
	}
}

- (IBAction)promptBluetooth:(id)sender
{
	_connectionFactory->promptBluetooth(^(PromptAccessoryError error)
	{
		[self updateUI];
	});
}

#pragma mark - TableView Delegate

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 2;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
	return section == 0 ? 1 : _discovered.size();
}


- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
	if (indexPath.section == 0)
	{
		return [tableView dequeueReusableCellWithIdentifier:@"HeaderCell" forIndexPath:indexPath];
	}
	DiscoveredCell* cell = [tableView dequeueReusableCellWithIdentifier:@"DiscoveredCell" forIndexPath:indexPath];
	[cell setDevice: _discovered[indexPath.row]];
    return cell;
}

@end
