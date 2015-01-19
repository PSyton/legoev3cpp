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

template <typename Tuple, size_t N, typename Function, std::enable_if_t<(N == std::tuple_size<Tuple>::value)>* = nullptr>
constexpr inline void tuple_for_each_item(Function function)
{
}

template <typename Tuple, size_t N, typename Function, std::enable_if_t<(N < std::tuple_size<Tuple>::value)>* = nullptr>
constexpr inline void tuple_for_each_item(Function function)
{
	const auto ptr = std::add_pointer_t<typename std::tuple_element<N, Tuple>::type>();
	function(N, ptr);
	tuple_for_each_item<Tuple, N+1>(function);
}

template <typename Tuple, typename Function>
constexpr inline void tuple_for_each(Function function)
{
	tuple_for_each_item<Tuple, 0>(function);
}


template <typename Tuple, size_t N, typename Function, std::enable_if_t<(N == std::tuple_size<Tuple>::value)>* = nullptr>
inline void tuple_for_each_item(Tuple& tuple, Function function)
{
}

template <typename Tuple, size_t N, typename Function, std::enable_if_t<(N < std::tuple_size<Tuple>::value)>* = nullptr>
inline void tuple_for_each_item(Tuple& tuple, Function function)
{
	auto& entity = std::get<N>(tuple);
	function(N, entity);
	tuple_for_each_item<Tuple, N+1>(tuple, function);
}

template <typename Tuple, typename Function>
inline void tuple_for_each(Tuple& tuple, Function function)
{
	tuple_for_each_item<Tuple, 0>(tuple, function);
}


template <typename Tuple, size_t N, typename Function, std::enable_if_t<(N == std::tuple_size<Tuple>::value)>* = nullptr>
inline void tuple_for_each_item(const Tuple& tuple, Function function)
{
}

template <typename Tuple, size_t N, typename Function, std::enable_if_t<(N < std::tuple_size<Tuple>::value)>* = nullptr>
inline void tuple_for_each_item(const Tuple& tuple, Function function)
{
	const auto& entity = std::get<N>(tuple);
	function(N, entity);
	tuple_for_each_item<Tuple, N+1>(tuple, function);
}

template <typename Tuple, typename Function>
inline void tuple_for_each(const Tuple& tuple, Function function)
{
	tuple_for_each_item<Tuple, 0>(tuple, function);
}

}
}