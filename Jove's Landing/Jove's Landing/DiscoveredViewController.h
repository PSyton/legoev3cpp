//
//  DiscoveredViewController.h
//  Jove's Landing
//
//  Created by David Giovannini on 2/12/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#import <UIKit/UIKit.h>
#include "SBJEV3ConnectionFactory.h"

@interface DiscoveredViewController : UITableViewController

- (void) setConnectionFactory: (SBJ::EV3::ConnectionFactory*) connectionFactory;

- (void) updateUI;

@end
