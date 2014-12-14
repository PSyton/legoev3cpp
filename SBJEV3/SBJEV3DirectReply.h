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

enum class ReplyStatus
{
	none,
	building,
	success,
	sendError,
	argumentError,
	formatError,
};
	
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
			[this](const uint8_t* buffer, size_t size)
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
	
	bool replied(const uint8_t* buffer, size_t len)
	{
		bool ready;
		{
			std::unique_lock<std::mutex> lock(_mutex);
			// strange edge case
			if (_status > ReplyStatus::building)
			{
				ready = true;
			}
			else
			{
				// set if the send fails
				if (buffer == nullptr || len == 0)
				{
					_status = ReplyStatus::sendError;
					ready = true;
				}
				else
				{
					// Malformed opcode(s) reported from the EV3
					COMRPL* header = (COMRPL*)buffer;
					if (header->Cmd == DIRECT_REPLY_ERROR)
					{
						_status = ReplyStatus::argumentError;
						ready = true;
					}
					else
					{
						// Convert the values
						_status = ReplyStatus::building;
						const size_t payloadLen = len - sizeof(COMRPL);
						const uint8_t* payload = buffer + sizeof(COMRPL);
						if (itemizedCopy(SizeT<0>(), payload, payloadLen))
						{
							_status = ReplyStatus::success;
						}
						// Lengths do not match up
						else
						{
							_status = ReplyStatus::formatError;
						}
						ready = true;
					}
				}
			}
		}
		if (ready)
		{
			_waitOn.notify_one();
		}
		return ready;
	}

	template <size_t N>
	inline bool itemizedCopy(SizeT<N>, const uint8_t* buffer, size_t maxLen)
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
		
		return itemizedCopy(SizeT<N+1>(), buffer + size, maxLen - size);
	}
	
	inline bool itemizedCopy(SizeT<std::tuple_size<Results>::value>, const uint8_t* buffer, size_t maxLen)
	{
		return true;
	}
};


}
}