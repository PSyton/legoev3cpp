//
//  SBJEV3FileUploader.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/14/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#pragma once

#include <string>
#include <functional>

namespace SBJ
{
namespace EV3
{

class Brick;

class FileUploader
{
public:
	using Upload = std::function<void(size_t fullSize, const uint8_t* buffer, size_t size, uint8_t status)>;
	
	FileUploader(Brick& brick, std::string file);
	
	void perform(Upload uploadTask);
	
private:
	Brick& _brick;
	std::string _path;
	
};

}
}
