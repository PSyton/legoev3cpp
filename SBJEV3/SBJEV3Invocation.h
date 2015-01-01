//
//  SBJEV3Invocation.h
//  LEGO Control
//
//  Created by David Giovannini on 12/11/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include <functional>

namespace SBJ
{
namespace EV3
{

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
	using Reply = std::function<bool(const uint8_t* buffer, size_t size)>;
	
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
	
	Invocation(Invocation&& rhs)
	: _messageId(rhs._messageId)
	, _data(std::move(rhs._data))
	, _size(rhs._size)
	, _reply(rhs._reply)
	{
		rhs._data = nullptr;
	}
	
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
	
	bool reply(const uint8_t* buffer, size_t size)
	{
		return _reply(buffer, size);
	}
	
private:
	unsigned short _messageId;
	custodian_ptr<uint8_t> _data;
	size_t _size;
	Reply _reply;
};

}
}
