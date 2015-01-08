//
//  SBJEV3SystemOpcodes.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/7/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3Opcodes.h"

namespace SBJ
{
namespace EV3
{
	
#pragma pack(push, 1)

// TODO: Implement the 15 System Opcodes
/*
template <size_t ChunkSize>
struct BeginUpload : public VariableLenOpcode
{
	size_t pack(UBYTE* into) const
	{
		const size_t s = sizeof(*this) - filePath.differential();
		if (into) ::memcpy(into, this, s);
		return s;
	}
	
	static constexpr size_t MaxChunk = 64434 - (sizeof(COMCMD) - sizeof(COMCMD::CmdSize)) - sizeof(BeginUpload);
	const UBYTE code = 0;
	const unsigned long chunkSize = ChunkSize;
	SystemStr<vmPATHSIZE> filePath;
	using Result = FileChunk;
};
*/
#pragma pack(pop)
	
}
}