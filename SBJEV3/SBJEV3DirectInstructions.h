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
#include <type_traits>
#include <stdio.h>

namespace SBJ
{
namespace EV3
{

template<typename Opcode, typename std::enable_if<std::is_base_of<VariableLenOpcode, Opcode>::value == false>::type* = nullptr>
static size_t packOpcode(const Opcode& opcode, uint8_t* buffer)
{
	::memcpy(buffer, &opcode, sizeof(Opcode));
	return sizeof(Opcode);
	
}

template<typename Opcode, typename std::enable_if<std::is_base_of<VariableLenOpcode, Opcode>::value == true>::type* = nullptr>
static size_t packOpcode(const Opcode& opcode, uint8_t* buffer)
{
	return opcode.pack(buffer);
}

struct OpcodeAccumulation
{
	uint8_t* data = nullptr;
	size_t opcodeSize = 0;
	UWORD globalSize = 0;
	UWORD localSize = 0;
};
	
template <typename Opcode>
class ExtendedOpcode
{
public:
	ExtendedOpcode(const Opcode& opcode)
	: _opcode(opcode)
	{
	}
	
	void accume(OpcodeAccumulation& accume)
	{
		accume.globalSize += setReplyPositions(accume.globalSize);
		const size_t extendedSize = pack(accume.data);
		accume.opcodeSize += extendedSize;
		accume.data += extendedSize;
	}
	
private:
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
	
	size_t pack(uint8_t* buffer) const
	{
		size_t baseSize = packOpcode(_opcode, buffer);
		::memcpy(buffer + baseSize, _pos, sizeof(_pos));
		return baseSize + sizeof(_pos);
	}
	
#pragma pack(push, 1)
	Opcode _opcode;
	// TODO: This may have to have an LValue as well
	GUValue _pos[Opcode::Result::ResultCount];
#pragma pack(pop)
};

/*
 * DirectInstructions creates a buffer of a mini-program that can be sent to the EV3.
 */

template <typename... Opcodes>
class DirectInstructions
{
public:
	DirectInstructions(unsigned short counter, bool forceReply, Opcodes... opcodes)
	{
		AllOpcodes allOpcodes((ExtendedOpcode<Opcodes>(opcodes))...);
		OpcodeAccumulation accume;
		accume.data = _data;
		calculateStructure(allOpcodes, accume, SizeT<0>());
		setHeader(counter, forceReply, accume);
	}
	
	Invocation invocation(Invocation::Reply reply)
	{
		custodian_ptr<uint8_t> data((uint8_t*)&_cmd, [](uint8_t*v){});
		return { _cmd.MsgCnt, data, sizeof(CMDSIZE) + _cmd.CmdSize, reply };
	}
	
private:

#pragma pack(push, 1)
	using AllOpcodes = std::tuple<ExtendedOpcode<Opcodes>...>;
	COMCMD _cmd = {0, 0, 0}; // bytes { {0, 1}, {2, 3}, {4} }
	DIRCMD _vars = {0, 0}; // bytes {5, 6}
	uint8_t _data[sizeof(AllOpcodes)];
#pragma pack(pop)
	
	template <size_t N>
	inline void calculateStructure(AllOpcodes& opcodes, OpcodeAccumulation& accume, SizeT<N>)
	{
		auto& opcode = std::get<N>(opcodes);
		opcode.accume(accume);
		calculateStructure(opcodes, accume, SizeT<N+1>());
	}
	
	inline void calculateStructure(AllOpcodes& opcodes, OpcodeAccumulation& accume, SizeT<std::tuple_size<AllOpcodes>::value>)
	{
	}
	
	// The reply buffer is a snapshot of the global space.
	// TODO: determine how LValues are used.
	inline void setHeader(unsigned short counter, bool forceReply, const OpcodeAccumulation& accume)
	{
		assert(accume.globalSize <= MAX_COMMAND_GLOBALS);
		assert(accume.localSize <= MAX_COMMAND_LOCALS);
		
		_cmd.CmdSize = sizeof(COMCMD) - sizeof(CMDSIZE) + sizeof(DIRCMD) + accume.opcodeSize;
		_cmd.MsgCnt = counter;
		_cmd.Cmd = (forceReply || accume.globalSize > 0) ? DIRECT_COMMAND_REPLY : DIRECT_COMMAND_NO_REPLY;
		_vars.Globals = (UBYTE)(0x00FF & accume.globalSize);
		_vars.Locals = (UBYTE)((0xFF00 & accume.globalSize) >> 8);
		_vars.Locals |= (UBYTE)(accume.localSize << 2);
	}
};


}
}