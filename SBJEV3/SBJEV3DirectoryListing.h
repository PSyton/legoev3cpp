//
//  SBJEV3DirectoryListing.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/10/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//
#pragma once

#include "SBJEV3Brick.h"

namespace SBJ
{
namespace EV3
{

class DirectoryListing
{
public:
	DirectoryListing(Brick& brick, std::string root = ROOTDIR)
	: _brick(brick)
	, _paths{root}
	{
		// according to docs, this happens automatically, just being explicit to remove any possible side-effects
		if (root.find(PARENTDIR) == 0 || root.find(CURRENTDIR) == 0)
		{
			root  = ROOTDIR + root;
		}
		load();
	}
	
	const SysDirResource::Entry& operator [] (size_t i) const
	{
		return _listing.entries[i];
	}
	
	size_t size() const
	{
		return _listing.entries.size();
	}
	
	void load(size_t i)
	{
		auto e = _listing.entries[i].name;
		if (e == PARENTDIR)
		{
			_paths.pop_back();
		}
		else if (e != CURRENTDIR)
		{
			_paths.push_back(_paths.back() + e);
		}
		load();
	}
	
private:
	Brick& _brick;
	std::vector<std::string> _paths;
	SysDirResource _listing;
	
	void load()
	{
		ListFiles listFiles;
		listFiles.resource = _paths.back();
		auto t = _brick.systemCommand(2.0, listFiles);
		_listing = std::get<0>(t);
	}
};

}
}