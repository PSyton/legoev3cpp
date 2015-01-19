//
//  SBJEV3ByteCodes.h
//  LEGO Control
//
//  Created by David Giovannini on 11/24/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3Enums.h"
#include "SBJEV3VariableSizedEntity.h"

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

#pragma mark - POD to byte array conversions

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

#pragma mark - High-Level to POD conversions

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

struct BoolToUByte
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

#pragma mark - ValueByteCode

/*
 * A ValueByteCode instance is the readonly final representation of an opcode parameter.
 */
	
#pragma pack(push, 1)

template <typename OutputType, typename InputType = StaticCast<typename OutputType::Input>>
struct ValueByteCode
{
public:
	using Input = typename InputType::Input;
	using Output = typename OutputType::Output;
	
	ValueByteCode(Input v = Input())
	: _data(OutputType::convert(InputType::convert(v)))
	{
	}
	
private:
	Output _data;
};

typedef ValueByteCode<LocalConstBytes0<UBYTE>, OverflowCheck<unsigned char, UBYTE, 0, 32>> CUTiny;
typedef ValueByteCode<LocalConstBytes1<UBYTE>, OverflowCheck<unsigned short, UBYTE>> CUByte;
typedef ValueByteCode<LocalConstBytes1<SBYTE>, OverflowCheck<signed short, SBYTE>> CSByte;
typedef ValueByteCode<LocalConstBytes2<UWORD>, OverflowCheck<unsigned int, UWORD>> CUShort;
typedef ValueByteCode<LocalConstBytes2<SWORD>, OverflowCheck<signed int, SWORD>> CSShort;
typedef ValueByteCode<LocalConstBytes4<ULONG>, OverflowCheck<unsigned long long, ULONG>> CULong;
typedef ValueByteCode<LocalConstBytes4<SLONG>, OverflowCheck<signed long long, SLONG>> CSLong;

template <size_t MaxSize = 256, size_t MinLen = 0>
struct CString : public ValueByteCode<LocalConstStr<MaxSize>>, public VariableSizedEntity
{
	using ValueByteCode<LocalConstStr<MaxSize>>::ValueByteCode;
	
	size_t size() const
	{
		size_t len = ::strlen((const char*)this);
		assert(len > MinLen); // account for LCS
		return len + 1;
	}
};

typedef ValueByteCode<GlobalVarBytes0<UBYTE>, OverflowCheck<unsigned char, UBYTE, 0x00, 0x32>> GUTiny;
typedef ValueByteCode<GlobalVarBytes1<UBYTE>, OverflowCheck<unsigned short, UBYTE>> GUByte;
typedef ValueByteCode<GlobalVarBytes1<SBYTE>, OverflowCheck<signed short, SBYTE>> GSByte;
typedef ValueByteCode<GlobalVarBytes2<UWORD>, OverflowCheck<unsigned int, UWORD>> GUShort;
typedef ValueByteCode<GlobalVarBytes2<SWORD>, OverflowCheck<signed int, SWORD>> GSShort;
typedef ValueByteCode<GlobalVarBytes4<ULONG>, OverflowCheck<unsigned long long, ULONG>> GULong;
typedef ValueByteCode<GlobalVarBytes4<SLONG>, OverflowCheck<signed long long, SLONG>> GSLong;
	
typedef ValueByteCode<LocalVarBytes0<UBYTE>, OverflowCheck<unsigned char, UBYTE, 0x00, 0x32>> LUTiny;
typedef ValueByteCode<LocalVarBytes1<UBYTE>, OverflowCheck<unsigned short, UBYTE>> LUByte;
typedef ValueByteCode<LocalVarBytes1<SBYTE>, OverflowCheck<signed short, SBYTE>> LSByte;
typedef ValueByteCode<LocalVarBytes2<UWORD>, OverflowCheck<unsigned int, UWORD>> LUShort;
typedef ValueByteCode<LocalVarBytes2<SWORD>, OverflowCheck<signed int, SWORD>> LSShort;
typedef ValueByteCode<LocalVarBytes4<ULONG>, OverflowCheck<unsigned long long, ULONG>> LULong;
typedef ValueByteCode<LocalVarBytes4<SLONG>, OverflowCheck<signed long long, SLONG>> LSLong;
	
typedef ValueByteCode<LocalConstBytes1<SBYTE>, RangeCheck<signed char, -100, +100>> CSpeed;
typedef ValueByteCode<LocalConstBytes1<SBYTE>, RangeCheck<signed char, -1, +17>> CMode;
typedef ValueByteCode<LocalConstBytes0<UBYTE>, RangeCheck<unsigned char, 0, 3>> CLayer;
typedef ValueByteCode<LocalConstBytes0<UBYTE>, StaticCast<OutputPort, UBYTE>> COutputPort;
typedef ValueByteCode<LocalConstBytes0<UBYTE>, StaticCast<Polarity, UBYTE>> CPolarity;
typedef ValueByteCode<LocalConstBytes0<UBYTE>, BoolToUByte> CBool;

// Certain opcodes use special Input port values to treat the Output port like a sensor (read motor states)
struct CInputPort : public ValueByteCode<LocalConstBytes0<UBYTE>, StaticCast<InputPort, UBYTE>>
{
	using ValueByteCode::ValueByteCode;
	
	CInputPort(OutputPort port)
	: ValueByteCode(OutputToInput::convert(port))
	{
	}
};

template <typename T, size_t S = sizeof(T)> struct NativeToVMType { using type = T; };

/*
template <typename T, size_t S>
struct TransferSizeConstraint
{
};

template <> struct NativeToVMType<bool> { using type = CBool; };
template <> struct NativeToVMType<UBYTE> { using type = CUByte; };
template <> struct NativeToVMType<SBYTE> { using type = CSByte; };
template <> struct NativeToVMType<UWORD> { using type = CUShort; };
template <> struct NativeToVMType<SWORD> { using type = CSShort; };
template <> struct NativeToVMType<ULONG> { using type = CULong; };
template <> struct NativeToVMType<SLONG> { using type = CSLong; };
template <> struct NativeToVMType<OutputPort> { using type = COutputPort; };
template <> struct NativeToVMType<InputPort> { using type = CInputPort; };
template <> struct NativeToVMType<Polarity> { using type = CPolarity; };
*/	
#pragma pack(pop)
	
}
}
