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

template <typename... Opcodes>
class DirectInstructions
{
public:
	
	DirectInstructions(unsigned short counter, bool forceReply, Opcodes... opcodes)
	: _data(nullptr)
	, _cmd
	{
		0,
		counter,
		static_cast<UBYTE>(forceReply ? DIRECT_COMMAND_REPLY : DIRECT_COMMAND_NO_REPLY)
	}
	, _vars
	{
		0,
		0
	}
	, _opcodes(ExtendedOpcode<Opcodes>(opcodes)...)
	{
		calculateStructure();
	}
	
	Invocation invocation(Invocation::Reply reply)
	{
		return { _cmd.MsgCnt, _data, sizeof(CMDSIZE) + _cmd.CmdSize, reply};
	}
	
private:
	
#pragma pack(push, 1)
	
	template <typename Opcode>
	class ExtendedOpcode : Opcode
	{
	public:
		ExtendedOpcode(const Opcode& opcode)
		: Opcode(opcode)
		{
		}
		
		// Tells the EV3 where in the global space to store the resulting values.
		size_t setReplyPositions(UBYTE startPosition)
		{
			size_t replySize = 0;
			for (size_t i = 0; i < Opcode::Result::ResultCount; i++)
			{
				_pos[i] = startPosition + replySize;
				replySize += Opcode::Result::allocatedSize(i);
			}
			return replySize;
		}
		
	private:
		// TODO: This may have to have an LValue as well
		GUValue _pos[Opcode::Result::ResultCount];
	};
	
	using AllOpcodes = std::tuple<ExtendedOpcode<Opcodes>...>;
	
	uint8_t* _data;
	COMCMD _cmd; // bytes { {0, 1}, {2, 3}, {4} }
	DIRCMD _vars; // bytes {5, 6}
	AllOpcodes _opcodes; // payload
	
	
#pragma pack(pop)
	
	inline void calculateStructure()
	{
		UWORD replySize = 0;
		size_t opcodeSize = calculateStructure(replySize, SizeT<0>());
		setVariables(opcodeSize, replySize, 0);
	}
	
	template <size_t N>
	inline size_t calculateStructure(UWORD& replySize, SizeT<N>)
	{
		// Set the global space positions for this reply
		// And increment the global space size
		auto& opcode = std::get<N>(_opcodes);
		replySize += opcode.setReplyPositions(replySize);
		size_t accumeOpcodeSize = opcodeSize(opcode);
		return accumeOpcodeSize + calculateStructure(replySize, SizeT<N+1>());
	}
	
	inline size_t calculateStructure(UWORD& replySize, SizeT<std::tuple_size<AllOpcodes>::value>)
	{
		return 0;
	}
	
	// The documentation states that direct command replies are always stored in the global space.
	// The reply buffer is a snapshot of the global space.
	// TODO: determine how LValues are used.
	inline void setVariables(size_t opcodeSize, UWORD globalSize, UWORD localSize)
	{
		assert(globalSize <= MAX_COMMAND_GLOBALS);
		assert(localSize <= MAX_COMMAND_LOCALS);
		
		// We want a reply if the opcode has reserved space
		if (localSize > 0 || globalSize > 0)
		{
			_cmd.Cmd = DIRECT_COMMAND_REPLY;
		}
		
		_cmd.CmdSize = sizeof(COMCMD) - sizeof(CMDSIZE) + sizeof(DIRCMD) + opcodeSize;
		_vars.Globals = (UBYTE)(0x00FF & globalSize);
		_vars.Locals = (UBYTE)((0xFF00 & globalSize) >> 8);
		_vars.Locals |= (UBYTE)(localSize << 2);
		
		_data = new uint8_t[sizeof(CMDSIZE) + _cmd.CmdSize];
		if (opcodeSize == sizeof(AllOpcodes))
		{
			::memcpy(_data, &_cmd, sizeof(CMDSIZE) + _cmd.CmdSize);
		}
		else
		{
			::memcpy(_data, &_cmd, sizeof(COMCMD) + sizeof(DIRCMD));
			copyOpcode(_data + sizeof(COMCMD) + sizeof(DIRCMD), SizeT<0>());
		}
	}
	
	template <size_t N>
	inline void copyOpcode(uint8_t*_data, SizeT<N>)
	{
		auto& opcode = std::get<N>(_opcodes);
		size_t accumeOpcodeSize = opcodeSize(opcode);
		::memcpy(_data, &opcode, accumeOpcodeSize);
		copyOpcode(_data + accumeOpcodeSize, SizeT<N+1>());
	}
	
	inline void copyOpcode(uint8_t*_data, SizeT<std::tuple_size<AllOpcodes>::value>)
	{
	}
};


}
}