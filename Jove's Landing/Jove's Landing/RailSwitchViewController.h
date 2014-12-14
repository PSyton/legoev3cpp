//
//  SecondViewController.h
//  Jove's Landing
//
//  Created by David Giovannini on 12/14/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#import <UIKit/UIKit.h>
#include "SBJEV3Brick.h"

@interface RailSwitchViewController : UITableViewController

- (void) setBrick: (SBJ::EV3::Brick*) brick;

- (void) updateUI;

@end

