//
//  SBJEV3InvocationStack.h
//  LEGO Control
//
//  Created by David Giovannini on 11/30/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3Invocation.h"
#include "SBJEV3DeleteMethods.h"

#include <map>
#include <thread>

namespace SBJ
{
namespace EV3
{
	
class Log;
class Connection;

/*
 * InvocationStack sends an invocation and matches it to a response function
 */
	
class InvocationStack
{
public:
	DeleteDefaultMethods(InvocationStack);
	
	using ReplyKey = std::function<unsigned short(const uint8_t* buffer)>;
	
	InvocationStack(Log& log, ReplyKey replyKey);
	
	~InvocationStack();
	
	bool isConnected() const
	{
		return (bool)_connection;
	}
		
	void connectionChange(std::unique_ptr<Connection>& connection);
	
	void invoke(Invocation& invocation);
	
	void remove(unsigned short invocation);
	
private:
	Log& _log;
	ReplyKey _replyKey;
	std::unique_ptr<Connection> _connection;
	std::mutex _mutex;
	std::map<unsigned short, Invocation> _invocations;
		
	const Invocation& pushInvocation(Invocation& invocation);
	void replyInvocation(unsigned short messageId, const uint8_t* buffer, size_t len);
};


/*
 * InvocationScope is an RAII invocation life cycle. It makes certain an invocation is forgotten on time-out.
 */
	
class InvocationScope
{
public:
	InvocationScope(InvocationStack& stack, Invocation& invocation)
	: _stack(stack)
	, _messageId(invocation.ID())
	{
		_stack.invoke(invocation);
	}
	
	~InvocationScope()
	{
		_stack.remove(_messageId);
	}
	
private:
	InvocationStack& _stack;
	const unsigned short _messageId;
};
	
	
}
}
