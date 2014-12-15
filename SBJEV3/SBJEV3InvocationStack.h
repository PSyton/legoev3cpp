//
//  SBJEV3InvocationStack.h
//  LEGO Control
//
//  Created by David Giovannini on 11/30/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3Invocation.h"

#include <map>
#include <thread>

namespace SBJ
{
namespace EV3
{
	
class Connection;

/*
 * InvocationStack sends an invocation and matches it to a response buffer
 */
	
class InvocationStack
{
public:
	InvocationStack();
	
	~InvocationStack();
		
	void connectionChange(Connection* connection);
	
	void invoke(Invocation& invocation);
	
	void remove(unsigned short invocation);
	
private:
	std::unique_ptr<Connection> _connection;
	std::mutex _mutex;
	std::map<unsigned short, Invocation> _invocations;
	
	void connectionReplied(const uint8_t* buffer, size_t len);
	
	void pushInvocation(Invocation& invocation);
	void replyInvocation(unsigned short messageId, const uint8_t* buffer, size_t len);
	void errorInvocation(unsigned short messageId);
	void removeInvocation(unsigned short messageId);
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
