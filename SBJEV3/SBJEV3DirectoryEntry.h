//
//  SBJEV3DirectoryEntry.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/12/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#include <vector>
#include <array>
#include <string>
#include <cstdint>

namespace SBJ
{
namespace EV3
{

/*
  FILETYPE_UNKNOWN              = 0x00,
  TYPE_FOLDER                   = 0x01,
  TYPE_SOUND                    = 0x02,
  TYPE_BYTECODE                 = 0x03,
  TYPE_GRAPHICS                 = 0x04,
  TYPE_DATALOG                  = 0x05,
  TYPE_PROGRAM                  = 0x06,
  TYPE_TEXT                     = 0x07,
  TYPE_SDCARD                   = 0x10,
  TYPE_USBSTICK                 = 0x20,
*/

#define DIREXT std::string("/")
#define BYTECODEEXT std::string(".rbf")
#define DATALOGEXT2 std::string(".raf")
#define GRAPHICEXT std::string(".rgf")
#define SOUNDEXT std::string(".rsf")
#define DATALOGEXT std::string(".rdf")
#define LIBRARYEXT std::string(".so")
#define KERNeLEXT std::string(".ko")



#define CURRENTDIR std::string("./")
#define PARENTDIR std::string("../")
#define ROOTDIR std::string("/home/root/lms2012/")
#define TOOLDIR std::string("/home/root/lms2012/tools/")
#define SYSDIR std::string("/home/root/lms2012/sys/")
#define SOURCEDIR std::string("/home/root/lms2012/source/")
#define APPDIR std::string("/home/root/lms2012/apps/")
#define PROJDIR std::string("/home/root/lms2012/prjs/")

class DirectoryEntry
{
public:
	static std::vector<DirectoryEntry> read(const char* data, size_t len);

	DirectoryEntry();
	
	DirectoryEntry(const std::string& line);
	
	bool isDirectory() const;
	
	const std::string& name() const { return _name; }
	
	const std::array<uint8_t, 16>& hash() const { return _hash; }
	
	uint32_t size() const { return _size; }
	
	std::string hashStr() const;
	
	std::string sizeStr() const;
	
	std::string simpleName() const;
	
	std::string extension() const;
	
	std::string escapedName() const;
	
	std::string pathRelativeToSys(std::string path) const;
	
private:
	std::array<uint8_t, 16> _hash;
	uint32_t _size;
	std::string _name;
};

}
}
