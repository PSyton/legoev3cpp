//
//  SBJEV3Invocation.h
//  LEGO Control
//
//  Created by David Giovannini on 12/11/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include <functional>

namespace SBJ
{
namespace EV3
{

/*
 * Invocation wraps the inputs, outputs, and message identifier into a single structure
 */
	
struct Invocation
{
	using Reply = std::function<bool(const uint8_t* buffer, size_t size)>;
	
	unsigned short messageId;
	const uint8_t* data;
	size_t size;
	Reply reply;
};

}
}
