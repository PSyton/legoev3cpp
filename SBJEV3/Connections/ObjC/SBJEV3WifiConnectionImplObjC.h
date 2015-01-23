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
@property (nonatomic) unsigned int port;
@property (nonatomic) NSString* protocol;
@property (nonatomic) NSString* serial;
@property (nonatomic) NSString* name;
@property (nonatomic) NSString* acceptance;

- (bool) connect: (SBJ::EV3::Log&) log;

@end

@interface EV3WifiConnectionImpl : EV3ConnectionImpl

- (id) init: (SBJ::EV3::Log&) log withAccessory: (EV3WifiAccessory*) accessory;

@end

