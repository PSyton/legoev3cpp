//
//  SBJEV3Results.h
//  LEGO Control
//
//  Created by David Giovannini on 12/20/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3ByteCodes.h"

#include <array>

namespace SBJ
{
namespace EV3
{

struct OpcodeAccumulation
{
	size_t opcodeSize = 0;
	UWORD globalSize = 0;
	UWORD localSize = 0;
};

static inline size_t roundUp(size_t n, size_t base = 4)
{
	return ((n + base - 1) / base) * base;
}

enum class VarScope
{
	global,
	local
};

class VoidResult;

template <typename Result>
struct ResultStorage
{
	constexpr static inline size_t localCount()
	{
		return Result::ResultCount * (Result::Scope == VarScope::local ? 1 : 0);
	}
	
	constexpr static inline size_t globalCount()
	{
		return Result::ResultCount * (Result::Scope == VarScope::global ? 1 : 0);
	}
	
	constexpr static inline size_t scopedCount()
	{
		return Result::ResultCount;
	}
	
	constexpr static size_t allocatedSize(size_t resultIdx)
	{
		return Result::allocatedSize(resultIdx);
	};
	
	using Allocation = std::conditional_t<
			Result::Scope == VarScope::local,
				LUShort[Result::ResultCount],
				GUShort[Result::ResultCount]>;
	
	using Reply = std::conditional_t<
			Result::Scope == VarScope::local,
				std::tuple<>,
				std::tuple<typename Result::Output>>;
};


template <>
struct ResultStorage<VoidResult>
{
	constexpr static inline size_t localCount()
	{
		return 0;
	}
	
	constexpr static inline size_t globalCount()
	{
		return 0;
	}
	
	constexpr static inline size_t scopedCount()
	{
		return 0;
	}
	
	constexpr static size_t allocatedSize(size_t resultIdx)
	{
		return 0;
	}
	
	using Allocation = int[0];
	
	using Reply = std::tuple<>;
};

/*
 * The Result structures do not require any storage space.
 * They define types and starategies used for defining how the EV3
 * produces an opcode return value and how the values are converted to high level types.
 */

struct VoidResult
{
};

template<VarScope varScope, typename InputType, typename OutputType = InputType>
struct BasicResult
{
	using Input = InputType;
	using Output = OutputType;
	
	constexpr static VarScope Scope = varScope;
	constexpr static size_t ResultCount = 1;
	
	constexpr static size_t allocatedSize(size_t resultIdx)
	{
		return sizeof(Input);
	}
	
	static inline void convert(const Input* input, Output& output, size_t)
	{
		if (input == nullptr) return;
		output = static_cast<Output>(*input);
	};
};

template <VarScope varScope, size_t MaxSize>
struct StringResult
{
	using Input = const char;
	using Output = std::string;
	
	constexpr static VarScope Scope = varScope;
	constexpr static size_t ResultCount = 1;
	
	constexpr static size_t allocatedSize(size_t resultIdx)
	{
		return MaxSize;
	}
	
	static inline void convert(const Input* input, Output& output, size_t)
	{
		if (input == nullptr) return;
		output = input;
	};
};

template<VarScope varScope, typename InputType, size_t Count, typename OutputType = InputType>
struct ArrayResult
{
	using Input = InputType;
	using Output = std::array<OutputType, Count>;
	
	constexpr static VarScope Scope = varScope;
	constexpr static size_t ResultCount = 1;
	
	constexpr static size_t allocatedSize(size_t resultIdx)
	{
		return Count * sizeof(Input);
	}
	
	static inline void convert(const Input* input, Output& output, size_t)
	{
		if (input == nullptr) return;
		for (size_t i = 0; i < Count; i++)
		{
			output[i] = static_cast<OutputType>(input[i]);
		}
	};
};
	
}
}
