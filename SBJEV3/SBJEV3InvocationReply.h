//
//  SBJEV3InvocationReply.h
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
 * InvocationReply receives a buffer response and extracts the requested results from the opcodes.
 * The reponse buffer is a snapshot of the global space for the mini-program.
*/
 
template <typename... Opcodes>
class InvocationReply
{
public:
	
	using Results = std::tuple<typename StorageSpecs<typename Opcodes::Result>::Output...>;

	InvocationReply(float timeout)
	: _timeout(timeout)
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
			long long time = _timeout * 1000.0;
			_waitOn.wait_for(lock, std::chrono::milliseconds(time), [this]{return _status > ReplyStatus::none;});
		}
		else
		{
			_status = ReplyStatus::success;
		}
		return _results;
	}
	
private:
	using Converters = std::tuple<typename Opcodes::Result...>;
	
	mutable std::mutex _mutex;
	std::condition_variable _waitOn;
	
	const float _timeout;
	Converters _converters;
	Results _results;
	ReplyStatus _status = ReplyStatus::none;
	
	ReplyStatus replied(const uint8_t* buffer, size_t len)
	{
		{
			std::unique_lock<std::mutex> lock(_mutex);
			if (len == 0)
			{
				_status = buffer != nullptr ? ReplyStatus::sendError : ReplyStatus::timeout;
				// TODO: allow the coverter objects to put themselves into a "no-response" state;
			}
			else
			{
				COMRPL* header = (COMRPL*)buffer;
				if (header->Cmd == DIRECT_REPLY_ERROR)
				{
					_status = ReplyStatus::malformedError;
				}
				else
				{
					const size_t payloadLen = len - sizeof(COMRPL);
					const uint8_t* payload = buffer + sizeof(COMRPL);
					if (itemizedCopy(size_type<0>(), payload, payloadLen, header->Cmd))
					{
						_status = ReplyStatus::success;
					}
					else
					{
						_status = ReplyStatus::lengthError;
					}
				}
			}
		}
		_waitOn.notify_one();
		return _status;
	}

	template <size_t N>
	inline bool itemizedCopy(size_type<N>, const uint8_t* buffer, size_t maxLen, UBYTE cmdState)
	{
		auto& converter = std::get<N>(_converters);
		using ConverterRef = decltype(converter);
		using ConverterType = typename std::remove_reference<ConverterRef>::type;
		using InputType = typename ConverterType::Input;
		
		size_t size = 0;
		// Calculate full allocation size and check for boundary condition
		for(size_t i = 0; i < StorageSpecs<ConverterType>::globalCount(); i++)
		{
			size += alignReply(converter.allocatedSize(i));
			// system cmd errors are handled by the result object
			if (cmdState ==  DIRECT_REPLY && size > maxLen)
			{
				return false;
			}
		}
		
		// Convert the low level value to the requested high level type
		auto& result = std::get<N>(_results);
		converter.convert((InputType*)buffer, result, maxLen);
		
		return itemizedCopy(size_type<N+1>(), buffer + size, maxLen - size, cmdState);
	}
	
	inline bool itemizedCopy(size_type<std::tuple_size<Results>::value>, const uint8_t* buffer, size_t maxLen, UBYTE cmdState)
	{
		return true;
	}
};


}
}