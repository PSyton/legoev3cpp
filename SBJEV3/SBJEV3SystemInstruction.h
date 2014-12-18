//
//  SBJEV3DirectInstructions.h
//  LEGO Control
//
//  Created by David Giovannini on 12/1/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3Opcodes.h"
#include "SBJEV3Invocation.h"

#include <tuple>
#include <cassert>
#include <stdio.h>

namespace SBJ
{
namespace EV3
{

// TODO: support runtime sized opcodes. The previous NXT opcodes were all fixed size.
// EV3 opcodes use null terminated strings with variable allocated size. This
// DirectInstructions implementation assumes fixed allocation sizes for strings.
	
/*
 * DirectInstructions creates a buffer of a mini-program that can be sent to the EV3.
 */

template <typename Opcode>
class SystemInstruction
{
public:
	
	SystemInstruction(unsigned short counter, bool forceReply, const Opcode& opcode)
	: _data(nullptr)
	, _cmd
	{
		0,
		counter,
		static_cast<UBYTE>(forceReply ? SYSTEM_COMMAND_REPLY : SYSTEM_COMMAND_NO_REPLY)
	}
	, _opcode(opcode)
	{
		calculateStructure();
	}
	
	Invocation invocation(Invocation::Reply reply)
	{
		return { _cmd.MsgCnt, _data, sizeof(CMDSIZE) + _cmd.CmdSize, reply};
	}
	
private:
	
#pragma pack(push, 1)
	uint8_t* _data;
	COMCMD _cmd; // bytes { {0, 1}, {2, 3}, {4} }
	Opcode _opcode; // payload
#pragma pack(pop)
	
	inline void calculateStructure()
	{
		size_t accumeOpcodeSize = opcodeSize(_opcode);
		_cmd.CmdSize = sizeof(COMCMD) - sizeof(CMDSIZE) + accumeOpcodeSize;
		_data = new uint8_t[sizeof(CMDSIZE) + _cmd.CmdSize];
		if (accumeOpcodeSize == sizeof(Opcode))
		{
			::memcpy(_data, &_cmd, sizeof(CMDSIZE) + _cmd.CmdSize);
		}
		else
		{
			::memcpy(_data, &_cmd, sizeof(COMCMD));
			::memcpy(_data + sizeof(COMCMD), &_opcode, accumeOpcodeSize);
		}
	}
};


}
}