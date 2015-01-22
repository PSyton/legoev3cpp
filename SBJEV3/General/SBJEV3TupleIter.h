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
 *	There are hundreds of ways of iterating over a tuple. The pattern below...
 *	- The constexpr, const, and non-const variants are implemented.
 *	- The iteration index is captured as a constexpr parameter to the functor invocation.
 *  - Call the _at variant to start at an arbitrary index.
 *	- C++14 polymorphic lambdas (auto param) are required for iteration index and constexpr.
 *	- The functor must return true/false to continue/break the iteration.
 *	- Recursion ends at size-1 for non-empty tuples.
 *	- If /tuple_element/ and /get/ have been previously overridden for the tuple type, the overrides will be used.
 */
 
#pragma mark - constexpr

template <size_t s>
using size_type = std::integral_constant<size_t, s>;

template <size_t N, typename Tuple, typename Function>
constexpr inline bool tuple_function_at(Function function)
{
	using std::tuple_element;
	auto entity = tuple_element<N, Tuple>();
	return function(size_type<N>(), entity);
}

template <size_t N, typename Tuple, typename Function, std::enable_if_t<(std::tuple_size<Tuple>::value == 0)>* = nullptr>
constexpr inline bool tuple_for_each_at(Function function)
{
	return false;
}

template <size_t N, typename Tuple, typename Function, std::enable_if_t<(N == std::tuple_size<Tuple>::value-1 and std::tuple_size<Tuple>::value > 0)>* = nullptr>
constexpr inline bool tuple_for_each_at(Function function)
{
	return tuple_function_at<N, Tuple>(function);
}

template <size_t N, typename Tuple, typename Function, std::enable_if_t<(N <  std::tuple_size<Tuple>::value-1 and std::tuple_size<Tuple>::value > 1)>* = nullptr>
constexpr inline bool tuple_for_each_at(Function function)
{
	if (tuple_function_at<N, Tuple>(function))
	{
		return tuple_for_each_at<N+1, Tuple>(function);
	}
	return false;
}

template <typename Tuple, typename Function>
constexpr inline bool tuple_for_each(Function function)
{
	return tuple_for_each_at<0, Tuple>(function);
}

#pragma mark - const

template <size_t N, typename Tuple, typename Function>
inline bool tuple_function_at(const Tuple& tuple, Function function)
{
	using std::get;
	const auto& entity = get<N>(tuple);
	return function(size_type<N>(), entity);
}

template <size_t N, typename Tuple, typename Function, std::enable_if_t<(std::tuple_size<Tuple>::value == 0)>* = nullptr>
inline bool tuple_for_each_at(const Tuple& tuple, Function function)
{
	return false;
}

template <size_t N, typename Tuple, typename Function, std::enable_if_t<(std::tuple_size<Tuple>::value > 0 and N == std::tuple_size<Tuple>::value-1)>* = nullptr>
inline bool tuple_for_each_at(const Tuple& tuple, Function function)
{
	return tuple_function_at<N>(tuple, function);
}

template <size_t N, typename Tuple, typename Function, std::enable_if_t<(std::tuple_size<Tuple>::value > 1 and N <  std::tuple_size<Tuple>::value-1)>* = nullptr>
inline bool tuple_for_each_at(const Tuple& tuple, Function function)
{
	if (tuple_function_at<N>(tuple, function))
	{
		return tuple_for_each_at<N+1>(tuple, function);
	}
	return false;
}

template <typename Tuple, typename Function>
inline bool tuple_for_each(const Tuple& tuple, Function function)
{
	return tuple_for_each_at<0>(tuple, function);
}

#pragma mark - non-const

template <size_t N, typename Tuple, typename Function>
inline bool tuple_function_at(Tuple& tuple, Function function)
{
	using std::get;
	auto& entity = get<N>(tuple);
	return function(size_type<N>(), entity);
}

template <size_t N, typename Tuple, typename Function, std::enable_if_t<(std::tuple_size<Tuple>::value == 0)>* = nullptr>
inline bool tuple_for_each_at(Tuple& tuple, Function function)
{
	return false;
}

template <size_t N, typename Tuple, typename Function, std::enable_if_t<(std::tuple_size<Tuple>::value > 0 and N == std::tuple_size<Tuple>::value-1)>* = nullptr>
inline bool tuple_for_each_at(Tuple& tuple, Function function)
{
	return tuple_function_at<N>(tuple, function);
}

template <size_t N, typename Tuple, typename Function, std::enable_if_t<(std::tuple_size<Tuple>::value > 1 and N <  std::tuple_size<Tuple>::value-1)>* = nullptr>
inline bool tuple_for_each_at(Tuple& tuple, Function function)
{
	if (tuple_function_at<N>(tuple, function))
	{
		return tuple_for_each_at<N+1>(tuple, function);
	}
	return false;
}

template <typename Tuple, typename Function>
inline bool tuple_for_each(Tuple& tuple, Function function)
{
	return tuple_for_each_at<0>(tuple, function);
}

}
}