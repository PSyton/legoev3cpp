//
//  SBJEV3Invocation.h
//  LEGO Control
//
//  Created by David Giovannini on 12/11/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3DeleteMethods.h"

#include <functional>
#include <string>
#include <cassert>

namespace SBJ
{
namespace EV3
{


enum class ReplyStatus
{
	ready = 0,
	success,
	sendError,
	timeout,
	unknownMsg,
	malformedError,
	lengthError,
};

inline std::string ReplyStatusStr(ReplyStatus r)
{
	const static std::string s[] = {"ready", "success", "sendError", "timeout", "unknownMsg", "malformedError", "lengthError" };
	return s[static_cast<int>(r)];
}

template <typename T>
using Deleter = std::function<void(T*)>;

template <typename T>
using custodian_ptr = std::unique_ptr<T, Deleter<T>>;

/*
 * Invocation wraps the inputs, outputs, and message identifier into a single structure
 */
	
class Invocation
{
public:
	DeleteCopyMethods(Invocation);
	
	using Reply = std::function<ReplyStatus(const uint8_t* buffer, size_t size)>;
	
	Invocation(
		unsigned short messageId,
		custodian_ptr<uint8_t>& data,
		size_t size,
		Reply reply)
	: _messageId(messageId)
	, _data(std::move(data))
	, _size(size)
	, _reply(reply)
	, _status(ReplyStatus::ready)
	{
		constexpr size_t maxInvocationSize = 65534 + 2;
		assert(size <= maxInvocationSize);
	}
	
	Invocation()
	: _messageId(0)
	, _data()
	, _size(0)
	, _reply()
	, _status(ReplyStatus::unknownMsg)
	{
	}
	
	Invocation(Invocation&& rhs) = default;
	
	Invocation& operator = (Invocation&& rhs) = default;
	
	~Invocation()
	{
	}
	
	unsigned short ID() const
	{
		return _messageId;
	}
	
	bool wantsReply() const
	{
		return (bool)_reply;
	}
	
	size_t size() const
	{
		return _size;
	}
	
	const uint8_t* data() const
	{
		return _data.get();
	}
	
	ReplyStatus status() const
	{
		return _status;
	}
	
	ReplyStatus reply(const uint8_t* buffer, size_t size)
	{
		if (_reply)
		{
			_status = _reply(buffer, size);
		}
		return _status;
	}
	
private:
	unsigned short _messageId;
	custodian_ptr<uint8_t> _data;
	size_t _size;
	Reply _reply;
	ReplyStatus _status;
};

}
}
