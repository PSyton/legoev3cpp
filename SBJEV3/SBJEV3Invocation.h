//
//  SBJEV3Invocation.h
//  LEGO Control
//
//  Created by David Giovannini on 12/11/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include <functional>
#include <string>

namespace SBJ
{
namespace EV3
{

enum class ReplyStatus
{
	none = 0,
	success,
	sendError,
	timeout,
	unknownMsg,
	malformedError,
	lengthError,
};

inline std::string ReplyStatusStr(ReplyStatus r)
{
	const static std::string s[] = {"none", "success", "sendError", "timeout", "unknownMsg", "malformedError", "lengthError" };
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
		_status = _reply(buffer, size);
		return _status;
	}
	
private:
	unsigned short _messageId;
	custodian_ptr<uint8_t> _data;
	size_t _size;
	Reply _reply;
	ReplyStatus _status = ReplyStatus::none;
};

}
}
