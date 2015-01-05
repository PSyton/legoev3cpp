//
//  SBJEV3Log.cpp
//  Jove's Landing
//
//  Created by David Giovannini on 1/2/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#include "SBJEV3Log.h"

#include <cctype>

using namespace SBJ::EV3;

template <typename T>
static inline char hdigit(T n)
{
	return "0123456789abcdef"[n & 0xF];
}

static inline char toChar(uint8_t v)
{
	return std::isprint(v) ? v : '`';
}

template <typename T>
static char* hdstr(T t, char* dest)
{
	constexpr size_t nibbleCount = sizeof(T) * 2;
	for (size_t i = 0; i < nibbleCount; i++)
	{
		unsigned long long value = (unsigned long long)(t);
		unsigned long long nibble = (unsigned long long)value >> ((nibbleCount - 1 - i) * 4);
		*dest = hdigit(nibble);
		dest++;
	}
	return dest;
}

static const uint8_t* dumpline(char* dest, size_t linelen, const uint8_t* src, const uint8_t* srcend)
{
	if (src >= srcend)
	{
		return nullptr;
	}
	
	dest = hdstr(src, dest);
	*dest = ' ';
	dest++;
	
	{
		constexpr int byteCount = sizeof(uint8_t*);
		const uint8_t* iter = src;
		int i = 0;
		for (; i < linelen; i++)
		{
			if (iter >= srcend)
			{
				break;
			}
			dest = hdstr(*iter, dest);
			*dest = (((i + byteCount + 1) % byteCount != 0)) ? ':' : ' ';
			dest++;
			iter++;
		}
		for (; i < linelen; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				*dest = ' ';
				dest++;
			}
		}
	}
	{
		const uint8_t* iter = src;
		int i = 0;
		for (; i < linelen; i++)
		{
			if (iter == srcend) break;
			*dest = toChar(*iter);
			dest++;
			iter++;
		}
		for (; i < linelen; i++)
		{
			*dest = ' ';
			dest++;
		}
		src = iter;
	}
	*dest = 0;
	return src;
}

void Log::hexDump(const void* addr, size_t len, size_t linelen)
{
	if (!_enabled) return;
	std::unique_lock<std::mutex> lock(_mutex);

	_stream << "  Length: " << len << std::endl;
	
	const size_t bufferLen = (sizeof(uint8_t*) * 2) + 1 + (linelen * 3) + linelen + 1;
	
	char line[bufferLen];
	const uint8_t* start = (const uint8_t*)addr;
	const uint8_t* cur = start;
	
	while((cur = dumpline(line, linelen, cur, start+len)))
	{
		_stream << "  " << line << std::endl;
	}
}
