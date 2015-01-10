//
//  DirectoryListingViewController.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/9/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#import <UIKit/UIKit.h>
#include "SBJEV3Brick.h"

@interface DirectoryListingViewController : UITableViewController

- (void) setBrick: (SBJ::EV3::Brick*) brick;

@end
