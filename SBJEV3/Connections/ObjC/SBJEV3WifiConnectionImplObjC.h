//
//  SBJEV3WifiConnectionImplObjC.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/22/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#import "SBJEV3ConnectionImplObjC.h"
#import "SBJEV3WifiAccessory.h"

@interface EV3WifiConnectionImpl : EV3ConnectionImpl

- (id) init: (SBJ::EV3::Log&) log withAccessory: (SBJ::EV3::WifiAccessory::Ptr&) accessory;

- (bool) lockConnection;

@end

