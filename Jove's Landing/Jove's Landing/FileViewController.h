//
//  FileViewController.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/11/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#import <UIKit/UIKit.h>
#include "SBJEV3Brick.h"

@interface FileViewController : UITableViewController

- (void) setBrick: (SBJ::EV3::Brick*) brick path: (const std::string&) path andFile: (const SBJ::EV3::SysDirEntry&) file;

@end
