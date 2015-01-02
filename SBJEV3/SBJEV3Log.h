//
//  SBJEV3Log.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/2/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#pragma once

#include <iostream>
#include <type_traits>


template<typename T, typename V = bool>
struct is_objc_class : std::false_type { };

#ifdef __OBJC__
template<typename T>
struct is_objc_class<T, typename std::enable_if<std::is_convertible<T, id>::value, bool>::type > : std::true_type
{
};
#endif

namespace SBJ
{
namespace EV3
{

/*
 * TODO:
 *   - Thread safety
 *   - Domains
 *   - prefixes
 *   - More hex dump code cleanup
 *   - No output optimizations
 */

class Log
{
public:
	Log(std::ostream& s)
	: _stream(s)
	{
	}
	
	void hexDump(const void* addr,int len, int linelen);
	
	void hexDump(const char* fmt, const void* addr, int len, int linelen);

    template <class T>
    typename std::enable_if<!is_objc_class<T>::value, Log&>::type operator << (T const & t)
	{
        _stream << t;
        return *this;
    }
	
	typedef std::ostream& (*StreamInject)(std::ostream&);

	Log& operator << (StreamInject inject)
	{
		inject(_stream);
		return *this;
	}

#ifdef __OBJC__
    template <class T>
    typename std::enable_if<is_objc_class<T>::value, Log&>::type operator << (T const & t)
	{
        _stream << [[t description] UTF8String];
        return *this;
    }
#endif

private:
    std::ostream& _stream;
};

}
}