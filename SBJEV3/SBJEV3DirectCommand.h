//
//  SBJEV3DirectCommand.h
//  LEGO Control
//
//  Created by David Giovannini on 11/21/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3DirectInstructions.h"
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
	using Status = typename DirectReply<Opcodes...>::Status;
	using Results = typename DirectReply<Opcodes...>::Results;
	
	DirectCommand(unsigned short messageId, float timeout, Opcodes... opcodes)
	: _instructions(messageId, timeout > 0.0, opcodes...)
	, _reply(timeout)
	{
	}
	
	operator Invocation ()
	{
		return { _instructions.messageId(), _instructions, _instructions.size(), _reply.replyResponse() };
	}
	
	Status status() const
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


}
}