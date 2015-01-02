//
//  SBJEV3Log.cpp
//  Jove's Landing
//
//  Created by David Giovannini on 1/2/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#include "SBJEV3Log.h"

// Baed on https://github.com/18446744073709551615/reDroid

using namespace SBJ::EV3;

static inline char hdigit(unsigned long n)
{
	return "0123456789abcdef"[n & 0xf];
}

static const char* dumpline(char* dest, int linelen, const char* src, const char* srcend)
{
	if (src >= srcend)
	{
		return 0;
	}
	unsigned long s = (unsigned long)src;
	
	for (int i = 0; i < 8; i++)
	{
		dest[i] = hdigit(s>>(28-i*4));
	}
	
	dest[8] = ' ';
	dest += 9;
	
	int i = 0;
	for (; i < linelen / 4; i++)
	{
		if (src+i < srcend)
		{
			dest[i*3] = hdigit(src[i]>>4);
			dest[i*3+1] = hdigit(src[i]);
			dest[i*3+2] = ((i+1+(unsigned long)src)&3)?':':' ';
			dest[linelen/4*3+i] = src[i] >= ' ' && src[i] < 0x7f ? src[i] : '`';
		}
		else
		{
			dest[i*3] = dest[i*3+1] = dest[i*3+2] = dest[linelen/4*3+i] = ' ';
		}
	}
	dest[linelen] = 0;
	return src + i;
}


void Log::hexDump(const void* addr,int len, int linelen)
{
	hexDump("%s\n", addr,len, linelen);
}

void Log::hexDump(const char* fmt, const void* addr, int len, int linelen)
{
	if (len > linelen*32)
	{
		len = linelen*32;
	}
	
	linelen *= 4;
	static char _buf[4096];
	char* buf = _buf;//(char*)alloca(linelen+1); // alloca() causes the initialization to fail!!!!
	buf[linelen]=0;
	const char*start = (char*)addr;
	const char*cur = start;
	while(!!(cur = dumpline(buf,linelen,cur,start+len)))
	{
		char line[1024];
		sprintf(line, fmt, buf);
		_stream << line;
	}
}
