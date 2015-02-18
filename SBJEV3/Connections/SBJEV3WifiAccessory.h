//
//  SBJEV3WifiAccessory.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/25/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3WifiAccessorySpec.h"

#include <string>
#include <thread>
#include <memory>
#include <chrono>

/*
 *	WifiAccessory is a heavy-weight class that handles connection states of a WifiAccessorySpec
 */

namespace SBJ
{
namespace EV3
{

class WifiAccessory : public WifiAccessorySpec
{
public:
	enum class State
	{
		discovered, // created
		locked, //locked
		errored, // i/o discovered disconnect
		stale // unused
	};

	using Ptr = std::shared_ptr<WifiAccessory>;
	
	WifiAccessory(const WifiAccessorySpec& spec);
	
	bool tryLock(const uint8_t* response, size_t length);
	
	bool waitForLock();
	
	void unlock(bool withError);
	
	State evaluateStaleness();
	
	void udpPing();
	
private:
	std::mutex _mutex;
	std::condition_variable _isReady;
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	State _state;
	std::chrono::system_clock::time_point _ping;
};

}
}
