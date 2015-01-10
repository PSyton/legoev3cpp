//
//  SBJEV3DirectCommand.h
//  LEGO Control
//
//  Created by David Giovannini on 11/21/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3DirectInstructions.h"
#include "SBJEV3InvocationReply.h"

namespace SBJ
{
namespace EV3
{

/*
 * DirectCommand merges DirectInstructions with a InvocationReply to create an Invocation
 */
 
template <typename... Opcodes>
class DirectCommand
{
public:
	using Results = typename InvocationReply<Opcodes...>::Results;
	
	DirectCommand(unsigned short messageId, float timeout, Opcodes... opcodes)
	: _instructions(messageId, timeout > 0.0, opcodes...)
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
	DirectInstructions<Opcodes...> _instructions;
	InvocationReply<Opcodes...> _reply;
};

}
}