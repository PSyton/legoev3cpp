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
#include <thread>

template<typename T, typename V = bool>
struct is_objc_class : std::false_type { };

#ifdef __OBJC__

template<typename T>
struct is_objc_class<T, typename std::enable_if<std::is_convertible<T, id>::value, bool>::type > : std::true_type
{
};

template <class T, class = typename std::enable_if<is_objc_class<T>::value>::type>
std::ostream& operator << (std::ostream& stream, T const & t)
{
    stream << [[t description] UTF8String];
    return stream;
}

#endif

namespace SBJ
{
namespace EV3
{

/*
 * TODO:
 *   - More hex dump code cleanup
 */

class Log
{
public:
	Log(std::ostream& s)
	: _stream(s)
	{
	}

    template <class T>
    Log& operator << (T const & t)
	{
		if (_enabled)
		{
			std::unique_lock<std::mutex> lock(_mutex);
			_stream << t;
		}
        return *this;
    }
	
	typedef std::ostream& (*StreamInject)(std::ostream&);
	Log& operator << (StreamInject inject)
	{
		if (_enabled)
		{
			std::unique_lock<std::mutex> lock(_mutex);
			inject(_stream);
		}
		return *this;
	}
	
	template <typename...  Items>
	Log& write(const std::string& domain, Items... items) // cannot pass in std::endl into items!
	{
		if (_enabled)
		{
			std::unique_lock<std::mutex> lock(_mutex);
			prefix(domain).output(items...);
			_stream << std::endl;
		}
		return *this;
	}
	
	void hexDump(const void* addr,int len, int linelen);
	
	void hexDump(const char* fmt, const void* addr, int len, int linelen);

private:
#ifdef DEBUG
	static constexpr bool _enabled = true;
#else
	static constexpr bool _enabled = false;
#endif
	mutable std::mutex _mutex;
    std::ostream& _stream;
	
	inline Log& prefix(const std::string& domain)
	{
		_stream << domain << ": ";
		return *this;
	}
	
	template <typename T, typename ...P>
	inline Log& output(T t, P ...p)
	{
		_stream << t;
		return output(p...);
	}

	inline Log&  output()
	{
		_stream << std::endl;
		return *this;
	}
};

}
}