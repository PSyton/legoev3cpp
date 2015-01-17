//
//  SBJEV3Hex.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/11/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#pragma once

#include <string>
#include <array>
#include <type_traits>
#include <cctype>

namespace SBJ
{
namespace EV3
{

inline char printchar(uint8_t v)
{
	return std::isprint(v) ? v : '`';
}

inline std::string replace(const std::string& str, const std::string& from, const std::string& to)
{
    std::string wsRet;
	
    if (str.empty() or from.empty())
	{
        return wsRet;
	}
	
    wsRet.reserve(str.length());
    size_t start_pos = 0, pos;
	
    while((pos = str.find(from, start_pos)) != std::string::npos)
	{
        wsRet += str.substr(start_pos, pos - start_pos);
        wsRet += to;
        pos += from.length();
        start_pos = pos;
    }
    wsRet += str.substr(start_pos);
    return wsRet;
}

inline char hexnibble(uint8_t v)
{
	return "0123456789abcdef"[v & 0xF];
}

inline char* hexstr(const uint8_t* v, size_t size, char* dest)
{
	for (size_t i = 0; i < size; i++)
	{
		const uint8_t b = v[i];
		*dest = hexnibble(b >> 4);
		dest++;
		*dest = hexnibble(b);
		dest++;
	}
	return dest;
}

template <typename T>
inline char* hexstr(T t, char* dest)
{
	return hexstr((const uint8_t*)&t, sizeof(T), dest);
}

template <typename T>
inline std::string hexstr(T t)
{
	char dest[(sizeof(T) * 2) + 1];
	dest[(sizeof(T) * 2)] = 0;
	hexstr(t, dest);
	return dest;
}

inline uint8_t hexvalue(char c)
{
	return std::isalpha(c) ? (c - 'A' + 10) : (c -'0');
}

inline uint8_t hexvalue(char c1, char c2)
{
	uint8_t msn = hexvalue(c1);
	uint8_t lsn = hexvalue(c2);
	return (msn  << 4) | lsn;
}

template <size_t Size>
inline std::array<uint8_t, Size> hexbytes(const std::string& str)
{
	std::array<uint8_t, Size> data;
	const size_t size = std::min(Size, str.size()/2);
	for (int i = 0; i < size; i+=1)
	{
		const char c1 = str[i*2];
		const char c2 = str[i*2+1];
		data[i] = hexvalue(c1, c2);
	}
	return data;
}

}
}