//
//  DirectoryListingViewController.m
//  Jove's Landing
//
//  Created by David Giovannini on 1/9/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#import "DirectoryListingViewController.h"
#include "SBJEV3DirectoryListing.h"

using namespace SBJ::EV3;

@interface DirectoryListingViewController ()
{
	DirectoryListing* _listing;
}
@end

@implementation DirectoryListingViewController

- (void) setBrick: (Brick*) brick
{
	_listing = new DirectoryListing(*brick);
	if (self.isViewLoaded)
	{
		_listing->refresh();
		[self.tableView reloadData];
	}
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	self.tableView.contentInset = UIEdgeInsetsMake(20, 0, 0, 0);
}

- (void) viewWillAppear:(BOOL)animated
{
	[super viewWillAppear:animated];
	if (_listing)
	{
		_listing->refresh();
		[self.tableView reloadData];
	}
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
	return _listing->size();
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"DirEntry" forIndexPath:indexPath];
	
	auto entry = (*_listing)[indexPath.row];
	if (entry.isDirectory())
	{
		cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
	}
	else
	{
		cell.accessoryType = UITableViewCellAccessoryDetailButton;
	}
    
    cell.textLabel.text = [NSString stringWithUTF8String: entry.simpleName().c_str()];

    return cell;
}

- (void) tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
	auto entry = (*_listing)[indexPath.row];
	if (entry.isDirectory())
	{
		_listing->change(indexPath.row);
		[self.tableView reloadData];
	}
	else
	{
	}
}

@end
