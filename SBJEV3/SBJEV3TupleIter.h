//
//  SBJEV3TupleIter.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/18/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#pragma once

#include <tuple>
#include <utility>

namespace SBJ
{
namespace EV3
{

/*
 *	There are hundreds of ways of iterating over a tuple.
 *	- The pattern below has constexpr, const, and non-const variants.
 *	- The iteration index is captured as a constexpr paramrter to the functor invocation. 
 *	- C++14 polymorphic lambdas (auto param) is required. 
 *	- The functor must return true/false to continue/break the iteration.
 *	- If tuple_element and get have been previously overridden for the tuple type, the overrides will be used.
 */

template <size_t s>
using size_type = std::integral_constant<size_t, s>;

template <typename Tuple, size_t N, typename Function, std::enable_if_t<(N == std::tuple_size<Tuple>::value)>* = nullptr>
constexpr inline bool tuple_for_each_item(Function function)
{
	return true;
}

template <typename Tuple, size_t N, typename Function, std::enable_if_t<(N < std::tuple_size<Tuple>::value)>* = nullptr>
constexpr inline bool tuple_for_each_item(Function function)
{
	using std::tuple_element;
	auto entity = tuple_element<N, Tuple>();
	if (function(size_type<N>(), entity))
	{
		return tuple_for_each_item<Tuple, N+1>(function);
	}
	return false;
}

template <typename Tuple, typename Function>
constexpr inline bool tuple_for_each(Function function)
{
	return tuple_for_each_item<Tuple, 0>(function);
}


template <typename Tuple, size_t N, typename Function, std::enable_if_t<(N == std::tuple_size<Tuple>::value)>* = nullptr>
inline bool tuple_for_each_item(const Tuple& tuple, Function function)
{
	return true;
}

template <typename Tuple, size_t N, typename Function, std::enable_if_t<(N < std::tuple_size<Tuple>::value)>* = nullptr>
inline bool tuple_for_each_item(const Tuple& tuple, Function function)
{
	using std::get;
	const auto& entity = get<N>(tuple);
	if (function(size_type<N>(), entity))
	{
		return tuple_for_each_item<Tuple, N+1>(tuple, function);
	}
	return false;
}

template <typename Tuple, typename Function>
inline bool tuple_for_each(const Tuple& tuple, Function function)
{
	return tuple_for_each_item<Tuple, 0>(tuple, function);
}


template <typename Tuple, size_t N, typename Function, std::enable_if_t<(N == std::tuple_size<Tuple>::value)>* = nullptr>
inline bool tuple_for_each_item(Tuple& tuple, Function function)
{
	return true;
}

template <typename Tuple, size_t N, typename Function, std::enable_if_t<(N < std::tuple_size<Tuple>::value)>* = nullptr>
inline bool tuple_for_each_item(Tuple& tuple, Function function)
{
	using std::get;
	auto& entity = get<N>(tuple);
	if (function(size_type<N>(), entity))
	{
		return tuple_for_each_item<Tuple, N+1>(tuple, function);
	}
	return false;
}

template <typename Tuple, typename Function>
inline bool tuple_for_each(Tuple& tuple, Function function)
{
	return tuple_for_each_item<Tuple, 0>(tuple, function);
}

}
}