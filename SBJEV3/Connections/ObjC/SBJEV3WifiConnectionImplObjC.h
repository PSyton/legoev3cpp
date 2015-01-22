//
//  SBJEV3WifiConnectionImplObjC.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/22/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#import "SBJEV3ConnectionImplObjC.h"

@interface EV3WifiAccessory : NSObject
@property (nonatomic) NSString* host;
@property (nonatomic) NSString* serial;
@property (nonatomic) NSString* name;
@property (nonatomic) unsigned int port;
@end

@interface EV3WifiConnectionImplIOS : EV3ConnectionImplObjC

- (id) init: (SBJ::EV3::Log&) log withAccessory: (EV3WifiAccessory*) accessory;

@end

