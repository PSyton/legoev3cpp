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
		[self.tableView reloadData];
	}
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	self.tableView.contentInset = UIEdgeInsetsMake(20, 0, 0, 0);
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
		cell.accessoryType = UITableViewCellAccessoryDetailDisclosureButton;
	}
	else
	{
		cell.accessoryType = UITableViewCellAccessoryNone;
	}
    
    cell.textLabel.text = [NSString stringWithUTF8String: entry.simpleName().c_str()];

    return cell;
}

- (void) tableView:(UITableView *)tableView accessoryButtonTappedForRowWithIndexPath:(NSIndexPath *)indexPath
{
	_listing->load(indexPath.row);
	[self.tableView reloadData];
}

@end
