//
//  SBJEV3DirectInstructions.h
//  LEGO Control
//
//  Created by David Giovannini on 12/1/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3SystemOpcodes.h"
#include "SBJEV3Invocation.h"

#include <tuple>
#include <cassert>
#include <stdio.h>

namespace SBJ
{
namespace EV3
{

struct SystemOpcodeAccumulation
{
	size_t opcodeSize = 0;
	UWORD globalSize = 0;
};

/*
 * SystemInstruction creates a buffer of a single System Instruction used for downloading and uploading of system data
 */

template <typename Opcode>
class SystemInstruction
{
public:
	SystemInstruction(unsigned short counter, bool forceReply, const Opcode& opcode)
	{
		SystemOpcodeAccumulation accume;
		accume.opcodeSize = packOpcode(opcode, _data);
		accume.globalSize = alignReply(Opcode::Reply::allocatedSize(0));
		setHeader(counter, forceReply, accume);
	}
	
	Invocation invocation(Invocation::Reply reply)
	{
		custodian_ptr<uint8_t> data((uint8_t*)&_cmd, [](uint8_t*v){});
		return { _cmd.MsgCnt, data, sizeof(CMDSIZE) + _cmd.CmdSize, reply};
	}
	
private:
	
#pragma pack(push, 1)
	COMCMD _cmd; // bytes { {0, 1}, {2, 3}, {4} }
	uint8_t _data[sizeof(Opcode)]; // payload
#pragma pack(pop)
	
	inline void setHeader(unsigned short counter, bool forceReply, const SystemOpcodeAccumulation& accume)
	{
		_cmd.CmdSize = sizeof(COMCMD) - sizeof(CMDSIZE) + sizeof(DIRCMD) + accume.opcodeSize;
		_cmd.MsgCnt = counter;
		_cmd.Cmd = (forceReply or accume.globalSize > 0) ? DIRECT_COMMAND_REPLY : DIRECT_COMMAND_NO_REPLY;
	}
};


}
}