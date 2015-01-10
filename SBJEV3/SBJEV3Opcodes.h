//
//  SBJEV3Opcodes.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/7/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3Results.h"
#include <algorithm>

namespace SBJ
{
namespace EV3
{

/*
 * Each opcode structure is the exact byte representation of an instruction for the EV3.
 * A result type is defined. Any value that must be constant for the opcode to function is
 * defined as constant.
 *
 * If an opcode is variable sized then derive from VariableLenOpcode and implement the
 * pack method.
 *
 * TODO: support use of optional LValues and GValues for opcode parameters
 */

// Until detectable custom attributes or template concepts (c++17++) - we use a parent class to tag classes as packable
struct VariableLenOpcode
{
	size_t pack(UBYTE* into) const
	{
		assert(false);
	}
};

struct OpcodeAccumulation
{
	size_t opcodeSize = 0;
	UWORD globalSize = 0;
	UWORD localSize = 0;
};

static inline size_t alignReply(size_t offset)
{
	//return (((offset / 4 ) - 1) * 4) + 4;
	return offset;
}

#define   MAX_COMMAND_SIZE    65534

#if 1

template<typename Opcode, typename std::enable_if<std::is_base_of<VariableLenOpcode, Opcode>::value == false>::type* = nullptr>
static inline size_t packOpcode(const Opcode& opcode, uint8_t* buffer)
{
	if (buffer) ::memcpy(buffer, &opcode, sizeof(Opcode));
	return sizeof(Opcode);
}

template<typename Opcode, typename std::enable_if<std::is_base_of<VariableLenOpcode, Opcode>::value == true>::type* = nullptr>
static inline size_t packOpcode(const Opcode& opcode, uint8_t* buffer)
{
	return opcode.pack(buffer);
}

#else // pre C++11 equiv

template<bool NeedsPacking = false>
struct PackOpcodeImpl
{
	template <typename Opcode>
	inline size_t operator()(const Opcode& opcode, uint8_t* buffer)
	{
		if (buffer) ::memcpy(buffer, &opcode, sizeof(Opcode));
		return sizeof(Opcode);
	}
};

template<>
struct PackOpcodeImpl<true>
{
	template <typename Opcode>
	inline size_t operator()(const Opcode& opcode, uint8_t* buffer)
	{
		return opcode.pack(buffer);
	}
};

template <typename Opcode>
static inline size_t packOpcode(const Opcode& opcode, uint8_t* buffer)
{
	return PackOpcodeImpl<std::is_base_of<VariableLenOpcode, Opcode>::value>()(opcode, buffer);
}

#endif

}
}