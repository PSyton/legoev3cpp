//
//  SBJEV3WifiAnnouncerObjC.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/22/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#import <Foundation/Foundation.h>

#include "SBJEV3Log.h"
#include "SBJEV3ConnectionToken.h"

@class EV3WifiAccessory;
@class EV3ConnectionImpl;

typedef void(^SBJEV3WifiAnnouncerChange)(NSString* serialNumber, EV3WifiAccessory*);

@interface EV3WifiAnnouncer : NSObject

- (id) initWithChange: (SBJEV3WifiAnnouncerChange) change;

- (EV3ConnectionImpl*) findConnection: (SBJ::EV3::Log&)log identifier: (SBJ::EV3::DeviceIdentifier&) identifier;

- (void) prompt: (SBJ::EV3::PromptAccessoryErrored) errored;

@end
