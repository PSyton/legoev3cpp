//
//  SBJEV3FileUploader.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/14/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3Brick.h"

namespace SBJ
{
namespace EV3
{

class FileUploader
{
public:
	using Upload = std::function<void(const uint8_t* buffer, size_t size)>;
	
	FileUploader(Brick& brick, std::string file)
	: _brick(brick)
	, _path(file)
	{
	}
	
	void perform(Upload uploadTask)
	{
		// TODO: why at 150 does the EV3 timeout!!!!
		BeginUpload<128> begin;
		begin.resource = _path;
		auto file = _brick.systemCommand(60, begin);
		auto actualRead = file.actualRead();
		
		uploadTask(file.data, actualRead);
		
		if (actualRead < file.size)
		{
			do
			{
				ContinueUpload<begin.BaseSize> cntinue;
				cntinue.handle = file.handle;
				auto section = _brick.systemCommand(60, cntinue);
				actualRead = section.actualRead(file.size);
				
				uploadTask(section.data, actualRead);
				
			} while (actualRead == begin.BaseSize);
		}
	}
	
private:
	Brick& _brick;
	std::string _path;
	
};

}
}
