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
			unsigned short invocationKey = _replyKey(buffer);
			replyInvocation(invocationKey, buffer, len);
		});
	}
}

void InvocationStack::invoke(Invocation& invocation)
{
	static uint8_t SendErrorReply[] = {};
	const Invocation& actual = pushInvocation(invocation);
	if (_connection)
	{
		if (_connection->write(actual.data(), actual.size()) == false)
		{
			replyInvocation(invocation.ID(), SendErrorReply, sizeof(SendErrorReply));
		}
	}
	else
	{
		replyInvocation(invocation.ID(), SendErrorReply, sizeof(SendErrorReply));
	}
}

void InvocationStack::remove(unsigned short messageId)
{
	replyInvocation(messageId, nullptr, 0);
}

#pragma mark - private

const Invocation& InvocationStack::pushInvocation(Invocation& invocation)
{
	_log.write(LogDomian, "Call ", invocation.ID());
	_log.hexDump(invocation.data(), invocation.size());
	{
		std::unique_lock<std::mutex> lock(_mutex);
		_invocations.insert(std::make_pair(invocation.ID(), std::move(invocation)));
		return _invocations.find(invocation.ID())->second;
	}
}

void InvocationStack::replyInvocation(unsigned short messageId, const uint8_t* buffer, size_t len)
{
	Invocation* actual = nullptr;
	{
		std::unique_lock<std::mutex> lock(_mutex);
		auto i = _invocations.find(messageId);
		if (i != _invocations.end())
		{
			actual = &i->second;
			_invocations.erase(i);
		}
	}
	// We have a response, send error, or timeout
	if (actual != nullptr)
	{
		ReplyStatus status = actual->reply(buffer, len);
		_log.write(LogDomian, "Reply ", messageId, " - ", ReplyStatusStr(status));
		_log.hexDump(buffer, len);
	}
	// We do not have a record of the invocation
	else
	{
		// EV3 sent us an unknown reply
		if (buffer != nullptr)
		{
			_log.write(LogDomian, "Reply ", messageId, " - ", ReplyStatusStr(ReplyStatus::unknownMsg));
			_log.hexDump(buffer, len);
		}
		// else Invocation Scope ensuring timeout is handled where replyInvocation was previously called
	}
}
