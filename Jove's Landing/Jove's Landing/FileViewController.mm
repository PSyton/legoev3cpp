//
//  FileViewController.m
//  Jove's Landing
//
//  Created by David Giovannini on 1/11/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#import "FileViewController.h"

using namespace SBJ::EV3;

@interface FileViewController ()
{
	Brick* _brick;
	std::string _pathStr;
	DirectoryEntry _file;
	IBOutlet UITableViewCell* _up;
	IBOutlet UITableViewCell* _download;
	IBOutlet UITableViewCell* _path;
	IBOutlet UITableViewCell* _name;
	IBOutlet UITableViewCell* _size;
	IBOutlet UITableViewCell* _hash;
}

@end

@implementation FileViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
	self.tableView.contentInset = UIEdgeInsetsMake(20, 0, 0, 0);
}

- (void) viewWillAppear:(BOOL)animated
{
	[super viewWillAppear:animated];
	[self updateUI];
}

- (void) setBrick: (SBJ::EV3::Brick*) brick path: (const std::string&) path andFile: (const SBJ::EV3::DirectoryEntry&) file
{
	_brick = brick;
	_pathStr = path;
	_file = file;
	if (self.isViewLoaded)
	{
		[self updateUI];
	}
}

- (void) updateUI
{
	_path.detailTextLabel.text = [NSString stringWithUTF8String: _pathStr.c_str()];
	_name.detailTextLabel.text = [NSString stringWithUTF8String: _file.name().c_str()];
	_size.detailTextLabel.text = [NSString stringWithUTF8String: _file.sizeStr().c_str()];
	_hash.detailTextLabel.text = [NSString stringWithUTF8String: _file.hashStr().c_str()];
	
	UIImage* image = nil;
	if (_file.isDirectory())
	{
		image = [UIImage imageNamed: @"document.dir"];
	}
	else
	{
		image = [UIImage imageNamed: [NSString stringWithUTF8String: ("document" + _file.extension()).c_str()]];
	}
	if (image == nil)
	{
		image = [UIImage imageNamed: @"document.*"];
	}
	_download.imageView.image = image;
}

- (void) tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
	UITableViewCell* cell = [self.tableView cellForRowAtIndexPath: indexPath];
	if (cell == _up)
	{
		[self.presentingViewController dismissViewControllerAnimated: YES completion: nil];
	}
	else if (cell == _download)
	{
		BeginUpload<1024> upload;
		upload.resource = _file.pathRelativeToSys(_pathStr);
		_brick->systemCommand(60, upload);
	}
}

@end
