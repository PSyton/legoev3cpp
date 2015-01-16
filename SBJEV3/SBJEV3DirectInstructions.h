//
//  SBJEV3DirectInstructions.h
//  LEGO Control
//
//  Created by David Giovannini on 12/1/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3DirectOpcodes.h"
#include "SBJEV3Invocation.h"

#include <tuple>
#include <cassert>
#include <type_traits>

namespace SBJ
{
namespace EV3
{
	
template <typename Opcode>
class ExtendedDirectOpcode
{
public:
	ExtendedDirectOpcode(const Opcode& opcode)
	: _opcode(opcode)
	{
	}
	
	void accumulate(OpcodeAccumulation& accume)
	{
		if (sizeof(_globals))
		{
			accume.globalSize += setReplyPositions(accume.globalSize);
		}
		else
		{
			accume.localSize += setReplyPositions(accume.localSize);
		}
		accume.opcodeSize += (packOpcode(_opcode, nullptr) + sizeof(_globals) + sizeof(_locals));
	}
	
	size_t pack(uint8_t* buffer) const
	{
		size_t baseSize = packOpcode(_opcode, buffer);
		::memcpy(buffer + baseSize, _globals, sizeof(_globals) + sizeof(_locals));
		return baseSize + sizeof(_globals) + sizeof(_locals);
	}
	
private:
	// Tells the EV3 where in the global or local space to store the resulting values.
	size_t setReplyPositions(size_t startPosition)
	{
		size_t replySize = 0;
		for (size_t i = 0; i < ResultStorage<typename Opcode::Result>::scopedCount(); i++)
		{
			size_t variableAddress = startPosition + replySize;
			if (sizeof(_globals))
			{
				_globals[i] = (UWORD)variableAddress;
			}
			else
			{
				_locals[i] = (UWORD)variableAddress;
			}
			size_t allocatedSize = alignReply(Opcode::Result::allocatedSize(i));
			replySize += allocatedSize;
		}
		return replySize;
	}
	
#pragma pack(push, 1)
	Opcode _opcode;
	GUShort _globals[ResultStorage<typename Opcode::Result>::globalCount()];
	LUShort _locals[ResultStorage<typename Opcode::Result>::localCount()];
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
		AllOpcodes allOpcodes((ExtendedDirectOpcode<Opcodes>(opcodes))...);
		OpcodeAccumulation accume;
		accumulate(allOpcodes, accume, size_type<0>());
		setHeader(counter, forceReply, accume);
		if (accume.opcodeSize == sizeof(AllOpcodes))
		{
			::memcpy(_data, &allOpcodes, sizeof(AllOpcodes));
		}
		else
		{
			pack(allOpcodes, _data, size_type<0>());
		}
	}
	
	Invocation invocation(Invocation::Reply reply)
	{
		custodian_ptr<uint8_t> data((uint8_t*)&_cmd, [](uint8_t*v){});
		return { _cmd.MsgCnt, data, sizeof(CMDSIZE) + _cmd.CmdSize, reply };
	}
	
private:
#pragma pack(push, 1)
	using AllOpcodes = std::tuple<ExtendedDirectOpcode<Opcodes>...>;
	COMCMD _cmd = {0, 0, 0}; // bytes { {0, 1}, {2, 3}, {4} }
	DIRCMD _vars = {0, 0}; // bytes {5, 6}
	uint8_t _data[sizeof(AllOpcodes)];
#pragma pack(pop)
	
	template <size_t N>
	inline void accumulate(AllOpcodes& opcodes, OpcodeAccumulation& accume, size_type<N>)
	{
		auto& opcode = std::get<N>(opcodes);
		opcode.accumulate(accume);
		accumulate(opcodes, accume, size_type<N+1>());
	}
	
	inline void accumulate(AllOpcodes& opcodes, OpcodeAccumulation& accume, size_type<std::tuple_size<AllOpcodes>::value>)
	{
	}
	
	template <size_t N>
	inline void pack(AllOpcodes& opcodes, uint8_t* buffer, size_type<N>)
	{
		auto& opcode = std::get<N>(opcodes);
		buffer += opcode.pack(buffer);
		pack(opcodes, buffer, size_type<N+1>());
	}
	
	inline void pack(AllOpcodes& opcodes, uint8_t* buffer, size_type<std::tuple_size<AllOpcodes>::value>)
	{
	}
	
	// The reply buffer is a snapshot of the global space.
	inline void setHeader(unsigned short counter, bool forceReply, const OpcodeAccumulation& accume)
	{
		size_t commandSize = sizeof(COMCMD) - sizeof(CMDSIZE) + sizeof(DIRCMD) + accume.opcodeSize;
		
		assert(commandSize <= MAX_COMMAND_SIZE);
		assert(accume.globalSize <= MAX_COMMAND_GLOBALS);
		assert(accume.localSize <= MAX_COMMAND_LOCALS);
		
		_cmd.CmdSize = commandSize;
		_cmd.MsgCnt = counter;
		_cmd.Cmd = (forceReply or accume.globalSize > 0) ? DIRECT_COMMAND_REPLY : DIRECT_COMMAND_NO_REPLY;
		_vars.Globals = (UBYTE)(0x00FF & accume.globalSize);
		_vars.Locals = (UBYTE)((0xFF00 & accume.globalSize) >> 8);
		_vars.Locals |= (UBYTE)(accume.localSize << 2);
		
	}
};


}
}