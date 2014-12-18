//
//  RailSwitch.m
//  LEGO Control
//
//  Created by David Giovannini on 12/7/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#import "RailSwitch.h"
#include "SBJEV3Opcodes.h"
#include <cmath>

using namespace SBJ::EV3;

@interface RailSwitch()<NSCoding>

@end

static RailSwitch* _switches[4];

@implementation RailSwitch
{
	Brick* _brick;
}

+ (void) installOnBrick: (Brick*) brick
{
	[self installPort: OutputPort::A onBrick: brick];
	[self installPort: OutputPort::B onBrick: brick];
	[self installPort: OutputPort::C onBrick: brick];
	[self installPort: OutputPort::D onBrick: brick];
}

+ (void) installPort: (OutputPort) port onBrick: (Brick*) brick
{
	NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
	NSData* data = [defaults objectForKey: [NSString stringWithFormat: @"RailSwitch%d", port]];
	RailSwitch* obj = data ? [NSKeyedUnarchiver unarchiveObjectWithData: data] : nil;
	if (obj == nil)
	{
		obj = [[RailSwitch alloc] initWithPort: port];
	}
	obj->_brick = brick;
	_switches[((int)::log2((float)(int)port))] = obj;
}

+ (RailSwitch*) switchForPort: (SBJ::EV3::OutputPort) port
{
	return _switches[((int)::log2((float)(int)port))];
}

- (void) save
{
	NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
	NSData* data = [NSKeyedArchiver archivedDataWithRootObject: self];
	[defaults setObject: data forKey: [NSString stringWithFormat: @"RailSwitch%d", _port]];
}

- (id) initWithPort: (OutputPort) port
{
	self = [super init];
	_port = port;
	switch (_port)
	{
		case OutputPort::A:
			_name = @"Mountain Pass";
			_power = 50;
			_time = 1000;
			break;
		case OutputPort::B:
			_name = @"Town Center";
			_power = 100;
			_time = 750;
			break;
		case OutputPort::C:
			_name = @"Town Bypass";
			_power = 100;
			_time = 750;
			break;
		case OutputPort::D:
			_name = @"Water Front";
			_power = 50;
			_time = 1000;
			break;
		default:
			break;
	}
	
	return self;
}

- (id) initWithCoder: (NSCoder*) aCoder
{
	self = [super init];
	_name = [aCoder decodeObjectForKey: @"name"];
	_port = (OutputPort)[aCoder decodeIntForKey: @"port"];
	_power = [aCoder decodeIntForKey: @"power"];
	_time = [aCoder decodeFloatForKey: @"time"];
	_open = [aCoder decodeBoolForKey: @"open"];
	return self;
}

- (void) encodeWithCoder:(NSCoder *)aCoder
{
	[aCoder encodeObject: _name forKey: @"name"];
	[aCoder encodeInt: (int)_port forKey: @"port"];
	[aCoder encodeInt: _power forKey: @"power"];
	[aCoder encodeFloat: _time forKey: @"time"];
	[aCoder encodeBool: _open forKey: @"open"];
}

- (void) toggle
{
	OutputTimePower motor;
	motor.port = _port;
	motor.power = _power * (_open ? 1 : -1);
	motor.runTime = _time * 1,000;
	
	OutputStart start;
	ReadValues<> read;
	
	_open = !_open;
	
	auto results = _brick->directCommand(1.0, read, motor, start);
	NSLog(@"%d", std::get<0>(results)[0]);
	
	[self save];
}

@end