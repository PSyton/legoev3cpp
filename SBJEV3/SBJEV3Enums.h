//
//  SBJEV3Enums.h
//  LEGO Control
//
//  Created by David Giovannini on 11/26/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include <type_traits>

namespace SBJ
{
namespace EV3
{

/*
 * High level enums not defined in the EV3 SDK
 */
	
enum class InputPort
{
	One = 0x00,
	Two = 0x01,
	Three = 0x02,
	Four = 0x03,
};
	
enum class OutputPort
{
	A = 0x01,
	B = 0x02,
	C = 0x04,
	D = 0x08,
	all = A | B | C | D
};
	
enum class Polarity
{
	backwards = -1,
	opposite = 0,
	forward = 1
};

}
}