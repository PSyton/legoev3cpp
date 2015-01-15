//
//  SBJEV3DirectOpcodes.h
//  LEGO Control
//
//  Created by David Giovannini on 11/21/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3Opcodes.h"
#include "SBJEV3DirectResults.h"

namespace SBJ
{
namespace EV3
{

#pragma pack(push, 1)

// TODO: implement all direct opcodes

#pragma mark - 

struct GetBrickName
{
	constexpr static size_t MaxSize = vmNAMESIZE;
	const UBYTE code = opCOM_GET;
	const CUValue subcode = GET_BRICKNAME;
	const CUValue length = MaxSize;
	using Result = StringResult<VarScope::global, MaxSize>;
};

struct SetBrickName : public VariableLenOpcode
{
	size_t pack(UBYTE* into) const
	{
		const size_t s = sizeof(*this) - name.differential();
		if (into) ::memcpy(into, this, s);
		return s;
	}
	
	constexpr static size_t MaxSize = vmNAMESIZE;
	const UBYTE code = opCOM_SET;
	const CUValue subcode = SET_BRICKNAME;
	CString<MaxSize> name;
	using Result = VoidResult;
};

struct BatteryVoltage
{
	const UBYTE code = opUI_READ;
	const CUValue subcode =  GET_VBATT;
	using Result = BasicResult<VarScope::global, FLOAT>;
};

struct BatteryCurrent
{
	const UBYTE code = opUI_READ;
	const CUValue subcode =  GET_IBATT;
	using Result = BasicResult<VarScope::global, FLOAT>;
};

struct BatteryTempuratureRise
{
	const UBYTE code = opUI_READ;
	const CUValue subcode =  GET_TBATT;
	using Result = BasicResult<VarScope::global, FLOAT>;
};

struct BatteryLevel
{
	static constexpr UBYTE MaxValue = 100;
	const UBYTE code = opUI_READ;
	const CUValue subcode =  GET_LBATT;
	using Result = BasicResult<VarScope::global, UBYTE>;
};

struct HardwareVersion
{
	constexpr static size_t MaxSize = vmNAMESIZE;
	const UBYTE code = opUI_READ;
	const CUValue subcode =  GET_HW_VERS;
	const CUValue length = MaxSize;
	using Result = StringResult<VarScope::global, MaxSize>;
};

struct FirmwareVersion
{
	constexpr static size_t MaxSize = vmNAMESIZE;
	const UBYTE code = opUI_READ;
	const CUValue subcode =  GET_FW_VERS;
	const CUValue length = MaxSize;
	using Result = StringResult<VarScope::global, MaxSize>;
};

struct FirmwareBuild
{
	constexpr static size_t MaxSize = vmNAMESIZE;
	const UBYTE code = opUI_READ;
	const CUValue subcode =  GET_FW_BUILD;
	const CUValue length = MaxSize;
	using Result = StringResult<VarScope::global, MaxSize>;
};

struct OSVersion
{
	constexpr static size_t MaxSize = vmNAMESIZE;
	const UBYTE code = opUI_READ;
	const CUValue subcode =  GET_OS_VERS;
	const CUValue length = MaxSize;
	using Result = StringResult<VarScope::global, MaxSize>;
};

struct OSBuild
{
	constexpr static size_t MaxSize = vmNAMESIZE;
	const UBYTE code = opUI_READ;
	const CUValue subcode =  GET_OS_BUILD;
	const CUValue length = MaxSize;
	using Result = StringResult<VarScope::global, MaxSize>;
};

struct FullVersion
{
	constexpr static size_t MaxSize = vmNAMESIZE;
	const UBYTE code = opUI_READ;
	const CUValue subcode =  GET_VERSION;
	const CUValue length = MaxSize;
	using Result = StringResult<VarScope::global, MaxSize>;
};

#pragma mark - Flow

struct NoOp
{
	const UBYTE code = opNOP;
	using Result = VoidResult;
};

struct ObjectEnd
{
	const UBYTE code = opJR;
	using Result = VoidResult;
};

struct Jump
{
	const UBYTE code = opJR;
	using Result = VoidResult;
	CSShort offset;
};

#pragma mark - UI

struct UIFlush
{
	const UBYTE code = opUI_FLUSH;
	using Result = VoidResult;
};
	
#pragma mark - Timer
	
#pragma mark - Sound
	
struct SoundBreak
{
	const UBYTE code = opSOUND;
	const CUValue subcode =  BREAK;
	using Result = VoidResult;
};
	
struct PlayTone
{
	const UBYTE code = opSOUND;
	const CUValue subcode =  TONE;
	CUByte volume;
	CUShort freq;
	CUShort duration;
	using Result = VoidResult;
};

struct SoundReady
{
	const UBYTE code = opSOUND_READY;
	using Result = VoidResult;
};
	
#pragma mark - Output

// TODO: appears not implemented in ev3 sources!
struct GetOutputType
{
	const UBYTE code = opOUTPUT_GET_TYPE;
	CLayer layer;
	COutputPort port = OutputPort::A;
	using Result = BasicResult<VarScope::global, UBYTE>;
};

struct SetOutputType
{
	const UBYTE code = opOUTPUT_SET_TYPE;
	CLayer layer;
	COutputPort port = OutputPort::A;
	CUValue type = TYPE_TACHO;
	using Result = VoidResult;
};

struct OutputPower
{
	const UBYTE code = opOUTPUT_POWER;
	CLayer layer;
	COutputPort port = OutputPort::A;
	CSpeed power;
	using Result = VoidResult;
};

struct OutputStart
{
	const UBYTE code = opOUTPUT_START;
	CLayer layer;
	COutputPort port = OutputPort::A;
	using Result = VoidResult;
};

struct OutputTimeSpeed
{
	const UBYTE code = opOUTPUT_TIME_SPEED;
	CLayer layer;
	COutputPort port = OutputPort::A;
	CSpeed speed;
	CULong rampUpTime;
	CULong runTime;
	CULong rampDownTime;
	CBool useBrake;
	using Result = VoidResult;
};

struct OutputTimePower
{
	const UBYTE code = opOUTPUT_TIME_POWER;
	CLayer layer;
	COutputPort port = OutputPort::A;
	CSpeed power;
	CULong rampUpTime;
	CULong runTime;
	CULong rampDownTime;
	CBool useBrake;
	using Result = VoidResult;
};

struct OutputStepPower
{
	const UBYTE code = opOUTPUT_STEP_POWER;
	CLayer layer;
	COutputPort port = OutputPort::A;
	CSpeed power;
	CULong rampUpSteps;
	CULong runSteps;
	CULong rampDownSteps;
	CBool useBrake;
	using Result = VoidResult;
};

struct OutputStepSpeed
{
	const UBYTE code = opOUTPUT_STEP_SPEED;
	CLayer layer;
	COutputPort port = OutputPort::A;
	CSpeed speed;
	CULong rampUpSteps;
	CULong runSteps;
	CULong rampDownSteps;
	CBool useBrake;
	using Result = VoidResult;
};

struct OutputPolarity
{
	const UBYTE code = opOUTPUT_POLARITY;
	CLayer layer;
	COutputPort port = OutputPort::A;
	CPolarity polarity;
	using Result = VoidResult;
};
	
#pragma mark - Input
	
struct GetInputType
{
	const UBYTE code = opINPUT_DEVICE;
	const CUValue subcode =  GET_TYPEMODE;
	CLayer layer;
	CInputPort port = OutputPort::A;
	using Result = TypeMode;
};

// TODO: What is mode? Type enum appears incomplete

template <UBYTE NumValues = 1>
struct ReadValues
{
	const UBYTE code = opINPUT_DEVICE;
	const CUValue subcode =  READY_SI;
	CLayer layer;
	CInputPort port = OutputPort::A;
	CUValue type = TYPE_KEEP;
	CMode mode = MODE_KEEP;
	const CUValue numValues = NumValues;
	using Result = ArrayResult<VarScope::global, ULONG, NumValues>;
};
	
#pragma pack(pop)
	
}
}

