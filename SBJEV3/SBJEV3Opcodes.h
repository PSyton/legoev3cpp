//
//  SBJEV3Opcodes.h
//  LEGO Control
//
//  Created by David Giovannini on 11/21/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3Results.h"
#include <algorithm>

namespace SBJ
{
namespace EV3
{

/*
 * Each opcode structure is the exact byte representation of an instruction for the EV3.
 * A result type is defined. Any value that must be constant for the opcode to function is
 * defined as constant.
 *
 * TODO: support runtime sized opcodes (null terminated string params)
 * TODO: support use of optional LValues and GValues for opcode parameters
 */
	
#pragma pack(push, 1)

struct VariableLenOpcode
{
	size_t size() const { return sizeof(*this); }
};
 
template <typename Opcode>
inline size_t opcodeSize(const Opcode& opcode)
{
	return sizeof(Opcode);
}
 
inline size_t opcodeSize(const VariableLenOpcode& opcode)
{
	return opcode.size();
}

#pragma mark - 

struct GetBrickName
{
	constexpr static size_t MaxLength = vmNAMESIZE-1;
	const UBYTE code = opCOM_GET;
	const CUValue subcode = GET_BRICKNAME;
	const CUValue length = MaxLength;
	using Result = StringResult<MaxLength>;
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
	CSValue offset;
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

struct OutputPower
{
	const UBYTE code = opOUTPUT_POWER;
	CUValue brick;
	COutputPort port = OutputPort::A;
	CSpeed power;
	using Result = VoidResult;
};

struct OutputStart
{
	const UBYTE code = opOUTPUT_START;
	CUValue brick;
	COutputPort port = OutputPort::A;
	using Result = VoidResult;
};

struct OutputTimeSpeed
{
	const UBYTE code = opOUTPUT_TIME_SPEED;
	CUValue brick;
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
	CUValue brick;
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
	CUValue brick;
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
	CUValue brick;
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
	CUValue brick;
	COutputPort port = OutputPort::A;
	CPolarity polarity;
	using Result = VoidResult;
};
	
#pragma mark - Input

// TODO:
// - Mode and type enums
// - Determine if mode determines result type (different opcodes with const mode)

template <UBYTE NumValues = 1>
struct ReadValues
{
	const UBYTE code = opINPUT_DEVICE;
	const CUValue subcode =  READY_SI;
	CUValue brick;
	CInputPort port = OutputPort::A;
	CUValue type;
	CUValue mode;
	const CUValue numValues = NumValues;
	using Result = ArrayResult<ULONG, NumValues>;
};
	
#pragma pack(pop)
	
}
}

