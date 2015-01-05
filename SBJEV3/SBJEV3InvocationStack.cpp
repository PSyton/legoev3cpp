//
//  SBJEV3InvocationStack.cpp
//  LEGO Control
//
//  Created by David Giovannini on 11/30/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#include "SBJEV3InvocationStack.h"
#include "SBJEV3Connection.h"
#include "SBJEV3Log.h"

using namespace SBJ::EV3;

static const std::string LogDomian = "Invocation";

InvocationStack::InvocationStack(Log& log, ReplyKey replyKey)
: _log(log)
, _replyKey(replyKey)
{	
}

InvocationStack::~InvocationStack()
{
}

#pragma mark - public thread-safe

void InvocationStack::connectionChange(std::unique_ptr<Connection>& connection)
{
	_connection = std::move(connection);
	if (_connection)
	{
		_connection->start([this](auto buffer, auto len)
		{
			connectionReplied(buffer, len);
		});
	}
}

void InvocationStack::invoke(Invocation& invocation)
{
	const Invocation* actual = nullptr;
	{
		std::unique_lock<std::mutex> lock(_mutex);
		actual = &(pushInvocation(invocation));
	}
	if (_connection)
	{
		if (_connection->write(actual->data(), actual->size()) == false)
		{
			remove(invocation.ID());
		}
	}
	else
	{
		remove(invocation.ID());
	}
}

void InvocationStack::remove(unsigned short messageId)
{
	std::unique_lock<std::mutex> lock(_mutex);
	removeInvocation(messageId);
}

void InvocationStack::connectionReplied(const uint8_t* buffer, size_t len)
{
	unsigned short invocationKey= _replyKey(buffer);
	{
		std::unique_lock<std::mutex> lock(_mutex);
		replyInvocation(invocationKey, buffer, len);
	}
}

#pragma mark - private

const Invocation& InvocationStack::pushInvocation(Invocation& invocation)
{
	_log.write(LogDomian, "Call ", invocation.ID());
	_log.hexDump(invocation.data(), invocation.size(), 16);
	_invocations.insert(std::make_pair(invocation.ID(), std::move(invocation)));
	return _invocations.find(invocation.ID())->second;
}

void InvocationStack::replyInvocation(unsigned short messageId, const uint8_t* buffer, size_t len)
{
	auto i = _invocations.find(messageId);
	if (i != _invocations.end())
	{
		ReplyStatus complete = i->second.reply(buffer, len);
		_log.write(LogDomian, "Reply ", messageId, " - ", (int)complete);
		_log.hexDump(buffer, len, 16);
		_invocations.erase(i);
	}
	else
	{
		errorInvocation(messageId);
	}
}

void InvocationStack::errorInvocation(unsigned short messageId)
{
	auto i = _invocations.find(messageId);
	if (i != _invocations.end())
	{
		i->second.reply(nullptr, 0);
		_invocations.erase(i);
		_log.write(LogDomian, "Error ", messageId);
	}
	else
	{
		_log.write(LogDomian, "Unknown ", messageId);
	}
}

void InvocationStack::removeInvocation(unsigned short messageId)
{
	auto i = _invocations.find(messageId);
	if (i != _invocations.end())
	{
		i->second.reply(nullptr, 0);
		_invocations.erase(i);
		_log.write(LogDomian, "Timeout ", messageId);
	}
}
