//
//  SBJEV3SystemCommand.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/7/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3SystemInstruction.h"
#include "SBJEV3InvocationReply.h"

namespace SBJ
{
namespace EV3
{

template <typename Opcode>
class SystemCommand
{
public:
	using Results = typename InvocationReply<Opcode>::Results;
	
	SystemCommand(unsigned short messageId, float timeout, Opcode opcode)
	: _instructions(messageId, timeout > 0.0, opcode)
	, _reply(timeout)
	{
	}
	
	Invocation invocation()
	{
		return _instructions.invocation(_reply.replyResponse());
	}
	
	ReplyStatus status() const
	{
		return _reply.status();
	}
	
	const Results& wait()
	{
		return _reply.wait();
	}
	
private:
	SystemInstruction<Opcode> _instructions;
	InvocationReply<Opcode> _reply;
};

}
}