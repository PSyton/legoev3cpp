//
//  EV3ConnectionImplObjC.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/21/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBJEV3Connection.h"
#include "SBJEV3Log.h"

@interface EV3ConnectionImplObjC : NSObject

- (id) init: (SBJ::EV3::Log&) log;
- (SBJ::EV3::Connection::Type) type;
- (void) start: (SBJ::EV3::Connection::Read) read;
- (bool) write: (const uint8_t*) buffer len: (size_t) len;
- (void) close;

@end
