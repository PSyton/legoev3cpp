//
//  SBJEV3DirectCommand.h
//  LEGO Control
//
//  Created by David Giovannini on 11/21/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3DirectInstructions.h"
#include "SBJEV3SystemInstruction.h"
#include "SBJEV3DirectReply.h"

namespace SBJ
{
namespace EV3
{

/*
 * DirectCommand merges DirectInstructions with a DirectReply to create an Invocation
 */
 
template <typename... Opcodes>
class DirectCommand
{
public:
	using Results = typename DirectReply<Opcodes...>::Results;
	
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
	DirectReply<Opcodes...> _reply;
};
/*
template <typename Opcode>
class SystemCommand
{
public:
	using Results = typename DirectReply<Opcode>::Results;
	
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
	DirectReply<Opcode> _reply;
};
*/

}
}