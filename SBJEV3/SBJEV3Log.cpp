//
//  SBJEV3Log.cpp
//  Jove's Landing
//
//  Created by David Giovannini on 1/2/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#include "SBJEV3Log.h"
#include "SBJEV3Hex.h"

using namespace SBJ::EV3;

static const uint8_t* dumpline(char* dest, size_t linelen, const uint8_t* src, const uint8_t* srcend)
{
	if (src >= srcend)
	{
		return nullptr;
	}
	
	dest = hexstr(src, dest);
	*dest = ' ';
	dest++;
	
	{
		constexpr size_t byteCount = sizeof(uint8_t*);
		const uint8_t* iter = src;
		int i = 0;
		for (; i < linelen; i++)
		{
			if (iter >= srcend)
			{
				break;
			}
			dest = hexstr(*iter, dest);
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
			*dest = printchar(*iter);
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
	hexDump("", addr, len, linelen);
}

void Log::hexDump(const std::string& header, const void* addr, size_t len, size_t linelen)
{
	if (!_enabled) return;
	std::unique_lock<std::mutex> lock(_mutex);
	
	if (header.length())
	{
		_stream << header << std::endl;
	}
	
	if (addr == nullptr || len == 0)
	{
		_stream << "  Length: " << len << " at 0x" << hexstr(addr) << std::endl;
		return;
	}

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
