//
//  SBJEV3SystemResults.cpp
//  Jove's Landing
//
//  Created by David Giovannini on 1/12/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#include "SBJEV3DirectoryEntry.h"
#include "SBJEV3Hex.h"

#include <sstream>

using namespace SBJ::EV3;

static std::array<uint8_t, 16> Hash(const std::string& line)
{
	if (line.back() != '/')
	{
		return hexbytes<16>(line);
	}
	return {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
}

static uint32_t Size(const std::string& line)
{
	if (line.back() != '/')
	{
		std::array<uint8_t, 4> bytes = hexbytes<4>(line.substr(33));
		std::reverse(bytes.begin(), bytes.end()); // little endian
		return *(uint32_t*)&bytes;
	}
	return 0;
}

static std::string Name(const std::string& line)
{
	if (line.back() != '/')
	{
		return line.substr(42);
	}
	return line;
}

std::vector<DirectoryEntry> DirectoryEntry::read(const char* data, size_t len)
{
	std::vector<DirectoryEntry> entries;
	bool hasCurrent = false;
	bool hasParent = false;
	if (data and len > 0)
	{
		char line[1024];
		int j = 0;
		for (int i = 0; i < len; i++)
		{
			switch (data[i])
			{
				case '\n':
				{
					line[j] = 0;
					j = 0;
					DirectoryEntry e(line);
					if (e.name() == CURRENTDIR)
					{
						hasCurrent = true;
					}
					if (e.name() == PARENTDIR)
					{
						hasParent = true;
					}
					entries.push_back(e);
					break;
				}
				default:
					line[j] = data[i];
					j++;
			}
		}
	}
	if (hasCurrent == false)
	{
		entries.push_back(DirectoryEntry(CURRENTDIR));
	}
	if (hasParent == false)
	{
		entries.push_back(DirectoryEntry(PARENTDIR));
	}
	
	//std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b) { return b._name < a._name; });
	
	return entries;
}


DirectoryEntry::DirectoryEntry()
: _hash({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})
, _size(0)
{
}
	
DirectoryEntry::DirectoryEntry(const std::string& line)
: _hash(Hash(line))
, _size(Size(line))
, _name(Name(line))
{
}

bool DirectoryEntry::isDirectory() const
{
	return _name.back() == '/';
}

std::string DirectoryEntry::hashStr() const
{
	return hexstr(_hash);
}

std::string DirectoryEntry::sizeStr() const
{
	std::stringstream str;
	str.precision(2);
	str << std::fixed;
	
	if (_size < 1000)
	{
		const auto size = _size;
		str << size << "B";
	}
	else if (_size < 1000*1000)
	{
		const auto size = _size / 1000.0;
		str << size << "kB";
	}
	else if (_size < 1000*1000*1000)
	{
		const auto size = _size / 1000.0 / 1000.0;
		str << size << "MB";
	}
	else
	{
		const auto size = _size / 1000.0 / 1000.0 / 1000.0;
		str << size << "GB";
	}
	return str.str();
}

std::string DirectoryEntry::simpleName() const
{
	if (_name.back() == '/')
	{
		return _name.substr(0, _name.size()-1);
	}
	size_t ext = _name.rfind('.');
	if (ext != std::string::npos and ext != 0)
	{
		return _name.substr(0, ext);
	}
	return _name;
}

std::string DirectoryEntry::extension() const
{
	if (_name.back() == '/')
	{
		return "/";
	}
	size_t ext = _name.rfind('.');
	if (ext != std::string::npos and ext != 0)
	{
		return _name.substr(ext);
	}
	return "";
}

std::string DirectoryEntry::escapedName() const
{
	return replace(_name, " ", "\\ ");
}

std::string DirectoryEntry::pathRelativeToSys(std::string path) const
{
	if (path.find(ROOTDIR) != std::string::npos)
	{
		return "../" + path.substr(ROOTDIR.size()) + _name;
	}
	return path + _name;
}