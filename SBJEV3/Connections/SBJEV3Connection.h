//
//  SBJEV3Connection.h
//  LEGO Control
//
//  Created by David Giovannini on 11/21/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3TransportSelection.h"

#include <string>
#include <functional>

namespace SBJ
{
namespace EV3
{
	
/*
 * This is a platform dependent interface for a connection to read/write data.
 * Read is expected to be called on a background thread.
 */
 
class Connection
{
public:
	using Read = std::function< void(const uint8_t* buffer, size_t size)>;

	virtual ~Connection() {};
	
	virtual ConnectionTransport transport() const = 0;
	
	virtual void start(Connection::Read read) = 0;
	
	virtual bool write(const uint8_t* buffer, size_t len) = 0;
};

}
}