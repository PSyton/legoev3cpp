//
//  SBJEV3Chunk.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/14/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3Brick.h"

namespace SBJ
{
namespace EV3
{

#include <cstdint>
#include <memory>
#include <cassert>

template <size_t ChunkSize>
class Chunk
{
public:

	constexpr static size_t NaturalSize = ChunkSize;
	
	~Chunk()
	{
		if (_allocated)
		{
			delete[] _dataPtr;
		}
	}
	
	const size_t size() const
	{
		return _size;
	}
	
	operator const uint8_t* () const
	{
		return _dataPtr;
	}
	
	void append(const uint8_t* data, size_t size)
	{
		allocateFor(_size + size);
		::memcpy(_dataPtr + _size, data, size);
		_size += size;
	}
	
	uint8_t* writePtr(size_t expectedWriteSize = NaturalSize)
	{
		uint8_t* ptr = _dataPtr + _size;
		allocateFor(_size + expectedWriteSize);
		return ptr;
	}
	
	void appendSize(size_t actualWriteSize)
	{
		_size += actualWriteSize;
		allocateFor(_size);
	}
	
private:
	uint8_t _data[ChunkSize];
	uint8_t* _dataPtr = _data;
	size_t _size = 0;
	size_t _allocated = 0;
	
	void allocateFor(size_t size)
	{
		if (size > ChunkSize)
		{
			int requiredChunks = (int)(::ceil((double)size / (double)ChunkSize));
			if (_allocated == 0)
			{
				requiredChunks++;
				_allocated = ChunkSize * requiredChunks;
				_dataPtr = (uint8_t*)malloc(_allocated);
				::memcpy(_dataPtr, _data, _size);
			}
			else if (size > _allocated)
			{
				_allocated += ChunkSize * requiredChunks;
				_dataPtr = (uint8_t*)realloc(_dataPtr, _allocated);
			}
		}
	}
};

}
}