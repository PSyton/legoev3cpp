//
//  EV3BluetoothConnectionImplIOS.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/22/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#import "SBJEV3ConnectionImplObjC.h"

@class EAAccessory;

extern "C" NSString* const LEGOAccessoryProtocol;

@interface EV3BluetoothConnectionImpl : EV3ConnectionImpl

- (id) init: (SBJ::EV3::Log&) log withAccessory: (EAAccessory*) accessory;

@end
