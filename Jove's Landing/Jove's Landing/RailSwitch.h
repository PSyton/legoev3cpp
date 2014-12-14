//
//  RailSwitch.h
//  
//
//  Created by David Giovannini on 12/7/14.
//
//

#import <Foundation/Foundation.h>
#import "SBJEV3Enums.h"
#import "SBJEV3Brick.h"

@interface RailSwitch : NSObject

@property (nonatomic, readonly) NSString* name;
@property (nonatomic, readonly) SBJ::EV3::OutputPort port;
@property (nonatomic) int power;
@property (nonatomic) float time;
@property (nonatomic, readonly) BOOL open;

+ (void) installOnBrick: (SBJ::EV3::Brick*) brick;

+ (RailSwitch*) switchForPort: (SBJ::EV3::OutputPort) port;

- (void) toggle;

@end
