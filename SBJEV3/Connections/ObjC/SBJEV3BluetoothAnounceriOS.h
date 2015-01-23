//
//  SBJEV3BluetoothAnounceriOS.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/22/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#import <Foundation/Foundation.h>

#include "SBJEV3Log.h"
#include "SBJEV3ConnectionToken.h"

typedef void(^EV3BluetoothAnouncerChange)(void);

@class EV3ConnectionImpl;

/*
 *	Sometimes the bluetooth iphone/EV3 pairing gets corrupt. Updating firmware fixes this.
 */

@interface EV3BluetoothAnouncer : NSObject

- (id) initWithChange: (EV3BluetoothAnouncerChange) change;

- (EV3ConnectionImpl*) findConnection: (SBJ::EV3::Log&)log identifier: (SBJ::EV3::DeviceIdentifier&) identifier;

- (void) prompt: (SBJ::EV3::PromptAccessoryErrored) errored;

@end
