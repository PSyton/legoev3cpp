//
//  SBJEV3ByteCodes.h
//  LEGO Control
//
//  Created by David Giovannini on 11/24/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3Enums.h"

#define PCASM
#define NOPAYLOADS
#define SETALIGNMENT
#include "c_com.h"

#include <array>
#include <limits>
#include <cassert>
#include <string>

namespace SBJ
{
namespace EV3
{

/*
 * Opcode parameters are a series of bytes created using macros in the EV3 SDK.
 * We create conversion structures to define how to translate from a
 * user freiendly type to a POD (if different) and then to the series of bytes.
 *
 */

template <typename InputType>
struct LocalConstBytes0
{
	using Input = InputType;
	using Output = std::array<UBYTE, 1>;
	static Output convert(Input v) { return {LC0(v)}; }
};

template <typename InputType>
struct LocalConstBytes1
{
	using Input = InputType;
	using Output = std::array<UBYTE, 2>;
	static Output convert(Input v) { return {LC1(v)}; }
};

template <typename InputType>
struct LocalConstBytes2
{
	using Input = InputType;
	using Output = std::array<UBYTE, 3>;
	static Output convert(Input v) { return {LC2(v)}; }
};

template <typename InputType>
struct LocalConstBytes4
{
	using Input = InputType;
	using Output = std::array<UBYTE, 5>;
	static Output convert(Input v) { return {LC4(v)}; }
};

template <size_t MaxSize>
struct LocalConstStr
{
	using Input = std::string;
	using Output = std::array<UBYTE, MaxSize+1>;
	static Output convert(Input v)
	{
		Output output = { LCS };
		size_t len = std::min(v.length(), MaxSize-1);
		int i = 0;
		for (; i <= len; i++)
		{
			output[i+1] = v[i];
		}
		output[i] = 0;
		return output;
	}
};

template <typename InputType>
struct GlobalVarBytes0
{
	using Input = InputType;
	using Output = std::array<UBYTE, 1>;
	static Output convert(Input v) { return {GV0(v)}; }
};

template <typename InputType>
struct GlobalVarBytes1
{
	using Input = InputType;
	using Output = std::array<UBYTE, 2>;
	static Output convert(Input v) { return {GV1(v)}; }
};

template <typename InputType>
struct GlobalVarBytes2
{
	using Input = InputType;
	using Output = std::array<UBYTE, 3>;
	static Output convert(Input v) { return {GV2(v)}; }
};

template <typename InputType>
struct GlobalVarBytes4
{
	using Input = InputType;
	using Output = std::array<UBYTE, 5>;
	static Output convert(Input v) { return {GV4(v)}; }
};
	
template <typename InputType>
struct LocalVarBytes0
{
	using Input = InputType;
	using Output = std::array<UBYTE, 1>;
	static Output convert(Input v) { return {LV0(v)}; }
};

template <typename InputType>
struct LocalVarBytes1
{
	using Input = InputType;
	using Output = std::array<UBYTE, 2>;
	static Output convert(Input v) { return {LV1(v)}; }
};

template <typename InputType>
struct LocalVarBytes2
{
	using Input = InputType;
	using Output = std::array<UBYTE, 3>;
	static Output convert(Input v) { return {LV2(v)}; }
};

template <typename InputType>
struct LocalVarBytes4
{
	using Input = InputType;
	using Output = std::array<UBYTE, 5>;
	static Output convert(Input v) { return {LV4(v)}; }
};

template <typename InputType, typename OutputType = InputType>
struct StaticCast
{
	using Input = InputType;
	using Output = OutputType;
	inline static Output convert(Input v) { return static_cast<Output>(v); }
};

template <typename InputType,
			InputType Min = std::numeric_limits<InputType>::min(),
			InputType Max = std::numeric_limits<InputType>::max(), typename OutputType = InputType>
struct RangeCheck
{
	using Input = InputType;
	using Output = OutputType;
	inline static OutputType convert(InputType v) { return (v < Min ? Min : v > Max ? Max : v); }
};

template <typename InputType, typename OutputType = InputType,
			OutputType Min = std::numeric_limits<OutputType>::min(),
			OutputType Max = std::numeric_limits<OutputType>::max()>
struct OverflowCheck
{
	using Input = InputType;
	using Output = OutputType;
	inline static OutputType convert(InputType v) { assert(v >= Min and v <= Max); return static_cast<Output>(v); }
};

struct BoolToByte
{
	using Input = bool;
	using Output = UBYTE;
	inline static UBYTE convert(bool v) { return v ? 1 : 0; }
};
	
struct OutputToInput
{
	using Input = OutputPort;
	using Output = InputPort;
	inline static InputPort convert(OutputPort port)
	{
		switch (port)
		{
			case OutputPort::A:
				return (InputPort)16;
				break;
			case OutputPort::B:
				return (InputPort)17;
				break;
			case OutputPort::C:
				return (InputPort)18;
				break;
			case OutputPort::D:
				return (InputPort)19;
				break;
			default:
				assert(false);
		}
	}
};

/*
 * A Value Store instance is the readonly final representation of an
 * opcode parameter.
 * TODO: I believe the coversion type selection between LValue and CValue
 *    will have to a be a compile-time option once I figure out how local values work.
 */
	
#pragma pack(push, 1)

template <typename OutputType, typename InputType = StaticCast<typename OutputType::Input>>
struct ValueStore
{
public:
	using Input = typename InputType::Input;
	using Output = typename OutputType::Output;
	
	ValueStore(Input v = Input())
	: _data(OutputType::convert(InputType::convert(v)))
	{
	}
	
private:
	Output _data;
};
	
// Types

typedef unsigned long long ULONGLONG;
typedef signed long long SLONGLONG;

typedef ValueStore<LocalConstBytes0<UBYTE>, OverflowCheck<UBYTE, UBYTE, 0, 32>> CUValue;
typedef ValueStore<LocalConstBytes1<UBYTE>, OverflowCheck<UWORD, UBYTE>> CUByte;
typedef ValueStore<LocalConstBytes1<SBYTE>, OverflowCheck<SWORD, SBYTE>> CSByte;
typedef ValueStore<LocalConstBytes2<UWORD>, OverflowCheck<ULONG, UWORD>> CUShort;
typedef ValueStore<LocalConstBytes2<SWORD>, OverflowCheck<SLONG, SWORD>> CSShort;
typedef ValueStore<LocalConstBytes4<ULONG>, OverflowCheck<ULONGLONG, ULONG>> CULong;
typedef ValueStore<LocalConstBytes4<SLONG>, OverflowCheck<SLONGLONG, SLONG>> CSLong;

template <size_t MaxSize = 256, size_t minLen = 0>
struct CString : ValueStore<LocalConstStr<MaxSize>>
{
	using ValueStore<LocalConstStr<MaxSize>>::ValueStore;
	
	size_t differential() const
	{
		size_t len = ::strlen((const char*)this);
		assert(len >= minLen);
		return MaxSize - len;
	}
};

typedef ValueStore<GlobalVarBytes0<UBYTE>, OverflowCheck<UBYTE, UBYTE, 0x00, 0x32>> GUValue;
typedef ValueStore<GlobalVarBytes1<UBYTE>, OverflowCheck<UWORD, UBYTE>> GUByte;
typedef ValueStore<GlobalVarBytes1<SBYTE>, OverflowCheck<SWORD, SBYTE>> GSByte;
typedef ValueStore<GlobalVarBytes2<UWORD>, OverflowCheck<ULONG, UWORD>> GUShort;
typedef ValueStore<GlobalVarBytes2<SWORD>, OverflowCheck<SLONG, SWORD>> GSShort;
typedef ValueStore<GlobalVarBytes4<ULONG>, OverflowCheck<ULONGLONG, ULONG>> GULong;
typedef ValueStore<GlobalVarBytes4<SLONG>, OverflowCheck<SLONGLONG, SLONG>> GSLong;
	
typedef ValueStore<LocalVarBytes0<UBYTE>, OverflowCheck<UBYTE, UBYTE, 0x00, 0x32>> LUValue;
typedef ValueStore<LocalVarBytes1<UBYTE>, OverflowCheck<UWORD, UBYTE>> LUByte;
typedef ValueStore<LocalVarBytes1<SBYTE>, OverflowCheck<SWORD, SBYTE>> LSByte;
typedef ValueStore<LocalVarBytes2<UWORD>, OverflowCheck<ULONG, UWORD>> LUShort;
typedef ValueStore<LocalVarBytes2<SWORD>, OverflowCheck<SLONG, SWORD>> LSShort;
typedef ValueStore<LocalVarBytes4<ULONG>, OverflowCheck<ULONGLONG, ULONG>> LULong;
typedef ValueStore<LocalVarBytes4<SLONG>, OverflowCheck<SLONGLONG, SLONG>> LSLong;
	
typedef ValueStore<LocalConstBytes1<SBYTE>, RangeCheck<SBYTE, -100, +100>> CSpeed;
typedef ValueStore<LocalConstBytes1<SBYTE>, RangeCheck<SBYTE, -1, +17>> CMode;
typedef ValueStore<LocalConstBytes0<UBYTE>, RangeCheck<UBYTE, 0, 3>> CLayer;
typedef ValueStore<LocalConstBytes0<UBYTE>, StaticCast<OutputPort, UBYTE>> COutputPort;
typedef ValueStore<LocalConstBytes0<UBYTE>, StaticCast<Polarity, UBYTE>> CPolarity;
typedef ValueStore<LocalConstBytes0<UBYTE>, BoolToByte> CBool;

// Certain opcodes use special Input port values to treat the Output port like a sensor (read motor states)
struct CInputPort : public ValueStore<LocalConstBytes0<UBYTE>, StaticCast<InputPort, UBYTE>>
{
	using ValueStore::ValueStore;
	
	CInputPort(OutputPort port)
	: ValueStore(OutputToInput::convert(port))
	{
	}
};
	
#pragma pack(pop)
	
}
}
