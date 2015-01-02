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

/*
 * DirectInstructions creates a buffer of a mini-program that can be sent to the EV3.
 */

template <typename... Opcodes>
class DirectInstructions
{
public:
	
	DirectInstructions(unsigned short counter, bool forceReply, Opcodes... opcodes)
	: _data((uint8_t*)&_cmd, [](uint8_t*v){})
	, _opcodes(ExtendedOpcode<Opcodes>(opcodes)...)
	{
		Accumulation accume;
		calculateStructure(accume, SizeT<0>());
		setHeader(counter, forceReply, accume);
		allocateForVariableSizedOpcodes(accume);
	}
	
	Invocation invocation(Invocation::Reply reply)
	{
		return { _cmd.MsgCnt, _data, sizeof(CMDSIZE) + _cmd.CmdSize, reply};
	}
	
private:
	
#pragma pack(push, 1)
	
	template <typename Opcode>
	class ExtendedOpcode : public Opcode
	{
	public:
		ExtendedOpcode(const Opcode& opcode)
		: Opcode(opcode)
		{
		}
		
		// Tells the EV3 where in the global space to store the resulting values.
		size_t setReplyPositions(UBYTE startPosition, size_t actualSize)
		{
			size_t replySize = 0;
			GUValue* pos = (GUValue*)this + actualSize - Opcode::Result::ResultCount;
			for (size_t i = 0; i < Opcode::Result::ResultCount; i++)
			{
				pos[i] = startPosition + replySize;
				replySize += Opcode::Result::allocatedSize(i);
			}
			return replySize;
		}
		
	private:
		// TODO: This may have to have an LValue as well
		GUValue _pos[Opcode::Result::ResultCount];
	};
	
	using AllOpcodes = std::tuple<ExtendedOpcode<Opcodes>...>;
	
	custodian_ptr<uint8_t> _data;
	COMCMD _cmd = {0, 0, 0}; // bytes { {0, 1}, {2, 3}, {4} }
	DIRCMD _vars = {0, 0}; // bytes {5, 6}
	AllOpcodes _opcodes; // payload
	
#pragma pack(pop)

	template<class Opcode, typename std::enable_if<std::is_base_of<VariableLenOpcode, Opcode>::value == false>::type* = nullptr>
	static size_t opcodeSize(const Opcode& opcode)
	{
		return sizeof(Opcode);
	}
	
	template<class Opcode, typename std::enable_if<std::is_base_of<VariableLenOpcode, Opcode>::value == true>::type* = nullptr>
	static size_t opcodeSize(const Opcode& opcode)
	{
		return opcode.size() + Opcode::Result::ResultCount;
	}
	
	struct Accumulation
	{
		size_t opcodeSize = 0;
		UWORD globalSize = 0;
		UWORD localSize = 0;
		bool mustAllocate = false;
	};
	
	template <size_t N>
	inline void calculateStructure(Accumulation& accume, SizeT<N>)
	{
		// Set the global space positions for this reply
		// And increment the global space size
		auto& opcode = std::get<N>(_opcodes);
		const size_t actualSize = opcodeSize(opcode);
		constexpr size_t fixedSize = sizeof(std::remove_reference<decltype(opcode)>);
		accume.opcodeSize += actualSize;
		accume.globalSize += opcode.setReplyPositions(accume.globalSize, actualSize);
		// If we have a variable sized opcode that is not the tail opcode...
		if (N != (std::tuple_size<AllOpcodes>::value>-1) && actualSize != fixedSize)
		{
			accume.mustAllocate = true;
		}
		calculateStructure(accume, SizeT<N+1>());
	}
	
	inline void calculateStructure(Accumulation& accume, SizeT<std::tuple_size<AllOpcodes>::value>)
	{
	}
	
	// The reply buffer is a snapshot of the global space.
	// TODO: determine how LValues are used.
	inline void setHeader(unsigned short counter, bool forceReply, const Accumulation& accume)
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
	
	void allocateForVariableSizedOpcodes(const Accumulation& accume)
	{
		if (accume.opcodeSize != sizeof(AllOpcodes)/*accume.mustAllocate*/)
		{
			uint8_t* data = new uint8_t[sizeof(CMDSIZE) + _cmd.CmdSize];
			_data = custodian_ptr<uint8_t>(data, [](uint8_t*v){delete[] v;});
			::memcpy(data, &_cmd, sizeof(COMCMD) + sizeof(DIRCMD));
			copyOpcode(data + sizeof(COMCMD) + sizeof(DIRCMD), SizeT<0>());
		}
	}
	
	template <size_t N>
	inline void copyOpcode(uint8_t* data, SizeT<N>)
	{
		auto& opcode = std::get<N>(_opcodes);
		size_t actualSize = opcodeSize(opcode);
		::memcpy(data, &opcode, actualSize);
		copyOpcode(data + actualSize, SizeT<N+1>());
	}
	
	inline void copyOpcode(uint8_t* data, SizeT<std::tuple_size<AllOpcodes>::value>)
	{
	}
};


}
}