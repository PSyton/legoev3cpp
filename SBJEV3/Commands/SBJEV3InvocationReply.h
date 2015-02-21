//
//  SBJEV3InvocationReply.h
//  LEGO Control
//
//  Created by David Giovannini on 12/1/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//


#pragma once

#include "SBJEV3Results.h"
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
	using Results = decltype(std::tuple_cat(typename ResultStorage<typename Opcodes::Result>::Reply()...));
	using Converters = decltype(std::tuple_cat(typename ResultStorage<typename Opcodes::Result>::Converter()...));

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
			_waitOn.wait_for(lock, std::chrono::milliseconds(time), [this]{return _status > ReplyStatus::ready;});
		}
		else
		{
			_status = ReplyStatus::success;
		}
		return _results;
	}
	
private:
	
	mutable std::mutex _mutex;
	std::condition_variable _waitOn;
	
	const float _timeout;
	Converters _converters;
	Results _results;
	ReplyStatus _status = ReplyStatus::ready;
	
	ReplyStatus replied(const uint8_t* buffer, size_t len)
	{
		{
			std::unique_lock<std::mutex> lock(_mutex);
			if (len == 0)
			{
				itemizeFail();
				_status = buffer != nullptr ? ReplyStatus::sendError : ReplyStatus::timeout;
			}
			else
			{
				COMRPL* header = (COMRPL*)buffer;
				if (header->Cmd == DIRECT_REPLY_ERROR)
				{
					_status = ReplyStatus::malformedMsg;
				}
				else
				{
					const size_t payloadLen = len - sizeof(COMRPL);
					const uint8_t* payload = buffer + sizeof(COMRPL);
					if (itemizedCopy(payload, payloadLen, header->Cmd))
					{
						_status = ReplyStatus::success;
					}
					else
					{
						_status = ReplyStatus::malformedReply;
					}
				}
			}
		}
		_waitOn.notify_one();
		return _status;
	}
	
	inline void itemizeFail()
	{
		tuple_for_each(_converters, [this](auto N, const auto& converter)
		{
			using ConverterRef = decltype(converter);
			using ConverterType = std::remove_reference_t<ConverterRef>;
			using InputType = typename ConverterType::Input;
			
			auto& result = std::get<N>(_results);
			converter.convert((InputType*)nullptr, result, 0);
			return true;
		});
	}
	
	inline bool itemizedCopy(const uint8_t* buffer, size_t maxLen, UBYTE cmdState)
	{
		size_t size = 0;
		return tuple_for_each(_converters, [&size, this, buffer, maxLen, cmdState](auto N, const auto& converter)
		{
			using ConverterRef = decltype(converter);
			using ConverterRef = decltype(converter);
			using ConverterType = std::remove_reference_t<ConverterRef>;
			using InputType = typename ConverterType::Input;
			
			const uint8_t* address = buffer + size;
			
			// Calculate full allocation size and check for boundary condition
			for(size_t i = 0; i < ResultStorage<ConverterType>::globalCount(); i++)
			{
				size += roundUp(converter.allocatedSize(i));
				// system cmd errors are handled by the result object
				if (cmdState == DIRECT_REPLY and size > maxLen)
				{
					return false;
				}
			}
			
			// Convert the low level value to the requested high level type
			auto& result = std::get<N>(_results);
			converter.convert((InputType*)address, result, maxLen);
			return true;
		});
	}
};


}
}