//
//  SBJEV3DirectOpcode.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/7/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3Results.h"
#include "SBJEV3VariableSizedEntity.h"
#include "SBJEV3DeleteMethods.h"

#include <algorithm>

namespace SBJ
{
namespace EV3
{

#pragma pack(push, 1)

struct IsDirectOpcode
{
};

template <typename ResultType, typename... ParameterTypes>
class DirectOpcode : public IsDirectOpcode
{
public:
	DeleteDefaultMethods(DirectOpcode);
	
	using Result = ResultType;
	using Parameters = std::tuple<typename NativeToVMType<ParameterTypes>::type...>;
	using ResultStorage = ResultStorage<ResultType>;
	
	DirectOpcode(ParameterTypes... params)
	: _params(params...)
	{
	}
	
	bool accumulate(OpcodeAccumulation& accume)
	{
		if (ResultStorage::globalCount())
		{
			accume.globalSize += setReplyPositions(accume.globalSize);
		}
		if (ResultStorage::localCount())
		{
			accume.localSize += setReplyPositions(accume.localSize);
		}
		size_t actualSize = size();
		accume.opcodeSize += actualSize;
		return (actualSize != sizeof(*this));
		return 0;
	}
	
	size_t size() const
	{
		return pack(nullptr) + sizeof(_resultPos);
	}

	size_t pack(uint8_t* buffer) const
	{
		return packTuple(_params, buffer);
	}

private:
	Parameters _params;
	typename ResultStorage::Allocation _resultPos;
	
	size_t setReplyPositions(size_t startPosition)
	{
		size_t replySize = 0;
		for (size_t i = 0; i < ResultStorage::scopedCount(); i++)
		{
			size_t variableAddress = startPosition + replySize;
			_resultPos[i] = (UWORD)variableAddress;
			size_t allocatedSize = roundUp(ResultStorage::allocatedSize(i));
			replySize += allocatedSize;
		}
		return replySize;
	}
};

#pragma pack(pop)

}
}