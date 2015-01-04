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
#include <tuple>
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
 * In order to resolve doOne ambiguity, they must reside in a class templated with Items
 */

template <typename...  Items>
class CompoundStream
{
public:
	using Tuple = std::tuple<Items...>;
	
	CompoundStream(std::ostream& stream, Items... items)
	: _stream(stream)
	{
		const Tuple tuple(items...);
		doOne(tuple, std::integral_constant<size_t, 0>());
	}

private:
    std::ostream& _stream;
	
	template <size_t N>
	inline void doOne(const Tuple& items, std::integral_constant<size_t, N>)
	{
		_stream << std::get<N>(items);
		doOne(items, std::integral_constant<size_t, N+1>());
	}
	
	inline void doOne(const Tuple& items, std::integral_constant<size_t, std::tuple_size<Tuple>::value>)
	{
	}
};

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
			prefix(domain);
			CompoundStream<Items...>(_stream, items...);
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
};

}
}