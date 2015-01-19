//
//  SBJEV3Messenger.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/19/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3DeleteMethods.h"
#include "SBJEV3InvocationStack.h"
#include "SBJEV3DirectCommand.h"
#include "SBJEV3SystemCommand.h"


namespace SBJ
{
namespace EV3
{


class Messenger
{
public:
	DeleteDefaultMethods(Messenger);

	Messenger(Log& log, InvocationStack::ReplyKey replyKey)
	: _stack(log, replyKey)
	, _messageCounter(0)
	{
	}
	
	void connectionChange(std::unique_ptr<Connection>& connection)
	{
		_stack.connectionChange(connection);
	}

	template <typename...  Opcodes>
	typename DirectCommand<Opcodes...>::Results directCommand(float timeout, Opcodes... opcodes)
	{
		std::unique_lock<std::mutex> lock(_rpcBlock);
		DirectCommand<Opcodes...> command(_messageCounter, timeout, opcodes...);
		_messageCounter++;
		Invocation invocation(std::move(command.invocation()));
		InvocationScope invocationScope(_stack, invocation);
		return command.wait();
	}

	template <typename  Opcode>
	typename SystemCommand<Opcode>::Results systemCommand(float timeout, Opcode opcode)
	{
		std::unique_lock<std::mutex> lock(_rpcBlock);
		SystemCommand<Opcode> command(_messageCounter, timeout, opcode);
		_messageCounter++;
		Invocation invocation(std::move(command.invocation()));
		InvocationScope invocationScope(_stack, invocation);
		return command.wait();
	}
	
private:
	std::mutex _rpcBlock;
	unsigned short _messageCounter;
	InvocationStack _stack;
};

}
}


