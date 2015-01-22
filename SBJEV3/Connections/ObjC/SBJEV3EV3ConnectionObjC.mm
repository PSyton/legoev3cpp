//
//  ConnectionObjC.cpp
//  LEGO Control
//
//  Created by David Giovannini on 11/21/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#include "SBJEV3ConnectionObjC.h"
#import "SBJEV3ConnectionImplObjC.h"

using namespace SBJ::EV3;

ConnectionObjC::ConnectionObjC(EV3ConnectionImplObjC* impl)
: _impl(impl)
{
}

ConnectionObjC::~ConnectionObjC()
{
	[_impl close];
	_impl = nil;
}

Connection::Type ConnectionObjC::type() const
{
	return _impl.type;
}

void ConnectionObjC::start(Read read)
{
	[_impl start: read];
}

bool ConnectionObjC::write(const uint8_t* buffer, size_t len)
{
	return [_impl write: buffer len: len];
}