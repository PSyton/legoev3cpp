//
//  SBJEV3FileUploader.cpp
//  Jove's Landing
//
//  Created by David Giovannini on 1/14/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#include "SBJEV3FileUploader.h"
#include "SBJEV3Brick.h"

using namespace SBJ::EV3;

FileUploader::FileUploader(Brick& brick, std::string file)
: _brick(brick)
, _path(file)
{
}
	
void FileUploader::perform(Upload uploadTask)
{
	// TODO why timeouts on large numbers and some files?
	BeginUpload<1024> begin;
	begin.resource = _path;
	auto file = _brick.systemCommand(60, begin);
	
	uploadTask(file.size, file.data, file.bytesRead, file.status);
	
	if (file.status != 0) return;
	//if (file.handle == 0) return;
	if (file.bytesRead >= file.size) return;
	
	UBYTE handle = file.handle;

	while (true)
	{
		ContinueUpload<begin.BaseSize> cntinue;
		cntinue.handle = handle;
		auto section = _brick.systemCommand(60, cntinue);
		
		uploadTask(file.size, section.data, section.bytesRead, section.status);
		
		if (section.status != 0) return;
		//if (section.handle == 0) return;
		if (section.bytesRead >= file.size) return;
		
		handle = section.handle;
	}
}