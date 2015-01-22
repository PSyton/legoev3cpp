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

template <typename T>
struct UpgradeToDirectOpcode
{
	using type = std::conditional_t<std::is_base_of<IsDirectOpcode, T>::value, T, DirectOpcode<typename T::Result, T>>;
};
template <typename T> using UpgradeToDirectOpcode_t = typename UpgradeToDirectOpcode<T>::type;

/*
 * DirectInstructions creates a buffer of a mini-program that can be sent to the EV3.
 */

template <typename... Opcodes>
class DirectInstructions
{
public:
	DirectInstructions() = delete;
	DirectInstructions(DirectInstructions&) = delete;
	DirectInstructions(DirectInstructions&&) = delete;
	DirectInstructions& operator = (DirectInstructions&) = delete;
	DirectInstructions& operator = (DirectInstructions&&) = delete;
	
	DirectInstructions(unsigned short counter, bool forceReply, Opcodes... opcodes)
	{
		AllOpcodes allOpcodes(opcodes...);
		OpcodeAccumulation accume;
		tuple_for_each(allOpcodes, [&accume](auto N, auto& opcode)
		{
			opcode.accumulate(accume);
			return true;
		});
		setHeader(counter, forceReply, accume);
		packTuple(allOpcodes, _data);
	}
	
	Invocation invocation(Invocation::Reply reply)
	{
		custodian_ptr<uint8_t> data((uint8_t*)&_cmd, [](uint8_t*v){});
		return { _cmd.MsgCnt, data, sizeof(CMDSIZE) + _cmd.CmdSize, reply };
	}
	
private:
#pragma pack(push, 1)
	using AllOpcodes = std::tuple<UpgradeToDirectOpcode_t<std::remove_reference_t<Opcodes>>...>;
	COMCMD _cmd = {0, 0, 0}; // bytes { {0, 1}, {2, 3}, {4} }
	DIRCMD _vars = {0, 0}; // bytes {5, 6}
	uint8_t _data[sizeof(AllOpcodes)];
#pragma pack(pop)
	
	// The reply buffer is a snapshot of the global space.
	inline void setHeader(unsigned short counter, bool forceReply, const OpcodeAccumulation& accume)
	{
		size_t commandSize = sizeof(COMCMD) - sizeof(CMDSIZE) + sizeof(DIRCMD) + accume.opcodeSize;
		
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