//
//  DirectoryListingViewController.m
//  Jove's Landing
//
//  Created by David Giovannini on 1/9/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#import "DirectoryListingViewController.h"
#import "FileViewController.h"
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

- (NSString*) tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
	return [NSString stringWithUTF8String: _listing->path().c_str()];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
	return _listing->size();
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
	if (_listing->depth() == 1)
	{
		auto entry = (*_listing)[indexPath.row];
		if (entry.name() == PARENTDIR)
		{
			return 0;
		}
	}
    return [super tableView:tableView heightForRowAtIndexPath:indexPath];
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
	
	UIImage* image = nil;
	NSString* name = nil;
	if (entry.isDirectory())
	{
		image = [UIImage imageNamed: @"document.dir"];
		name = [NSString stringWithUTF8String: entry.simpleName().c_str()];
	}
	else
	{
		image = [UIImage imageNamed: [NSString stringWithUTF8String: ("document" + entry.extension()).c_str()]];
		name = [NSString stringWithUTF8String: entry.simpleName().c_str()];
	}
	if (image == nil)
	{
		image = [UIImage imageNamed: @"document.*"];
		name =  [NSString stringWithUTF8String: entry.name().c_str()];
	}
	cell.imageView.image = image;
    cell.textLabel.text = name;

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
		FileViewController* vc = [self.storyboard instantiateViewControllerWithIdentifier: @"FileViewController"];
		auto entry = (*_listing)[indexPath.row];
		[vc setBrick: &_listing->brick() path: _listing->path() andFile: entry];
		[self presentViewController: vc animated: YES completion: nil];
	}
}

@end
