//
//  SBJEV3DirectReply.h
//  LEGO Control
//
//  Created by David Giovannini on 12/1/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//


#pragma once

#include "SBJEV3Opcodes.h"
#include "SBJEV3Invocation.h"

#include <tuple>
#include <thread>

namespace SBJ
{
namespace EV3
{
	
/*
 * DirectReply receives a buffer response and extracts the requested results from the opcodes.
 * The reponse buffer is a snapshot of the global space for the mini-program.
 */
 
template <typename... Opcodes>
class DirectReply
{
public:
	
	using Results = std::tuple<typename Opcodes::Result::Output...>;

	DirectReply(float timeout)
	: _timeout(timeout)
	, _status(ReplyStatus::none)
	{
	}
	
	Invocation::Reply replyResponse()
	{
		return expectsReply() == false ? Invocation::Reply() :
			[this](auto buffer, auto size)
			{
				return replied(buffer, size);
			};
	}
	
	bool expectsReply() const
	{
		return _timeout > 0.0;
	}
	
	const Results& wait()
	{
		if (expectsReply())
		{
			std::unique_lock<std::mutex> lock(_mutex);
			int time = _timeout * 1000.0;
			_waitOn.wait_for(lock, std::chrono::milliseconds(time), [this]{return _status > ReplyStatus::building;});
		}
		else
		{
			_status = ReplyStatus::success;
		}
		return _results;
	}
	
	ReplyStatus status() const
	{
		std::unique_lock<std::mutex> lock(_mutex);
		return _status;
	}
	
private:
	using Converters = std::tuple<typename Opcodes::Result...>;
	
	mutable std::mutex _mutex;
	std::condition_variable _waitOn;
	
	const float _timeout;
	Converters _converters;
	Results _results;
	ReplyStatus _status;
	
	ReplyStatus replied(const uint8_t* buffer, size_t len)
	{
		{
			std::unique_lock<std::mutex> lock(_mutex);
			// strange edge case
			if (_status > ReplyStatus::building)
			{
			}
			else
			{
				// set if the send fails
				if (buffer == nullptr || len == 0)
				{
					_status = ReplyStatus::sendError;
				}
				else
				{
					// Malformed opcode(s) reported from the EV3
					COMRPL* header = (COMRPL*)buffer;
					if (header->Cmd == DIRECT_REPLY_ERROR)
					{
						_status = ReplyStatus::malformedError;
					}
					else
					{
						// Convert the values
						_status = ReplyStatus::building;
						const size_t payloadLen = len - sizeof(COMRPL);
						const uint8_t* payload = buffer + sizeof(COMRPL);
						if (itemizedCopy(size_type<0>(), payload, payloadLen))
						{
							_status = ReplyStatus::success;
						}
						// Lengths do not match up
						else
						{
							_status = ReplyStatus::lengthError;
						}
					}
				}
			}
		}
		_waitOn.notify_one();
		return _status;
	}

	template <size_t N>
	inline bool itemizedCopy(size_type<N>, const uint8_t* buffer, size_t maxLen)
	{
		auto& converter = std::get<N>(_converters);
		using ConverterRef = decltype(converter);
		using ConverterType = typename std::remove_reference<ConverterRef>::type;
		using InputType = typename ConverterType::Input;
		
		size_t size = 0;
		// Calculate full allocation size and check for boundary condition
		for(size_t i = 0; i < converter.ResultCount; i++)
		{
			size += converter.allocatedSize(i);
			if (size > maxLen)
			{
				return false;
			}
		}
		
		// Convert the low level value to the requested high level type
		auto& result = std::get<N>(_results);
		converter.convert((InputType*)buffer, result);
		
		return itemizedCopy(size_type<N+1>(), buffer + size, maxLen - size);
	}
	
	inline bool itemizedCopy(size_type<std::tuple_size<Results>::value>, const uint8_t* buffer, size_t maxLen)
	{
		return true;
	}
};


}
}