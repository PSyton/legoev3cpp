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

template <typename... T>
static inline void Expand(const T&...) { }

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
			_stream << domain << ": ";
			Expand(_stream << items...);
			_stream << std::endl;
		}
		return *this;
	}
	
	void hexDump(const void* addr, size_t len, size_t linelen = 16);
	
private:
#ifdef DEBUG
	static constexpr bool _enabled = true;
#else
	static constexpr bool _enabled = false;
#endif
	mutable std::mutex _mutex;
    std::ostream& _stream;
};

}
}