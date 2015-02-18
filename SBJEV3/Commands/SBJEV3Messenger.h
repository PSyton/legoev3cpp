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
#include "SBJEV3Connection.h"

namespace SBJ
{
namespace EV3
{

/*
 * Messenger makes certain the streams are not crossed with multiple transports on a single device.
 */

class Messenger
{
public:
	DeleteDefaultMethods(Messenger);

	Messenger(Log& log, InvocationStack::ReplyKey replyKey)
	: _stack{{log, replyKey}, {log, replyKey}, {log, replyKey}, {log, replyKey}}
	, _messageCounter(0)
	{
	}
	
	bool isConnected(ConnectionTransport transport) const
	{
		return _stack[(int)transport].isConnected();
	}
	
	void connectionChange(ConnectionTransport transport, std::unique_ptr<Connection>& connection)
	{
		_stack[(int)transport].connectionChange(connection);
	}

	template <typename...  Opcodes>
	typename DirectCommand<Opcodes...>::Results directCommand(ConnectionTransport transport, float timeout, Opcodes... opcodes)
	{
		std::unique_lock<std::mutex> lock(_rpcBlock);
		DirectCommand<Opcodes...> command(_messageCounter, timeout, opcodes...);
		_messageCounter++;
		Invocation invocation(std::move(command.invocation()));
		InvocationScope invocationScope(_stack[(int)transport], invocation);
		return command.wait();
	}

	template <typename  Opcode>
	typename SystemCommand<Opcode>::Results systemCommand(ConnectionTransport transport, float timeout, Opcode opcode)
	{
		std::unique_lock<std::mutex> lock(_rpcBlock);
		SystemCommand<Opcode> command(_messageCounter, timeout, opcode);
		_messageCounter++;
		Invocation invocation(std::move(command.invocation()));
		InvocationScope invocationScope(_stack[(int)transport], invocation);
		return command.wait();
	}
	
private:
	std::mutex _rpcBlock;
	unsigned short _messageCounter;
	InvocationStack _stack[ConnectionTransportCount];
};

}
}


