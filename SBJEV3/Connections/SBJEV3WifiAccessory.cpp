//
//  SBJEV3WifiAccessory.cpp
//  Jove's Landing
//
//  Created by David Giovannini on 1/25/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#include "SBJEV3WifiAccessory.h"

using namespace SBJ::EV3;

WifiAccessory::WifiAccessory(const WifiAccessorySpec& spec)
: WifiAccessorySpec(spec)
, _state(isValid() ? State::discovered : State::errored)
{
	udpPing();
}

bool WifiAccessory::tryLock(const uint8_t* response, size_t length)
{
	bool locked = false;
	{
		std::unique_lock<std::mutex> lock(_mutex);
		locked = unlockResponse(response, length);
		if (locked)
		{
			_state = State::locked;
		}
	}
	if (locked)
	{
		_isReady.notify_all();
	}
	return locked;
}
	
bool WifiAccessory::waitForLock()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_isReady.wait_for(lock, std::chrono::milliseconds(3000), ^{return isUnlocked() > 0;});
	return isUnlocked();
}
	
void WifiAccessory::unlock(bool withError)
{
	std::unique_lock<std::mutex> lock(_mutex);
	forgetLock();
	_state = withError ? State::errored : State::discovered;
}

WifiAccessory::State WifiAccessory::evaluateStaleness()
{
	std::unique_lock<std::mutex> lock(_mutex);
	if (_state == State::discovered)
	{
		const auto stop = std::chrono::system_clock::now();
		const auto d_actual = std::chrono::duration_cast<std::chrono::minutes>(stop - _ping).count();
		if (d_actual >= 30)
		{
			_state = State::stale;
		}
	}
	return _state;
}

void WifiAccessory::udpPing()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_ping = std::chrono::system_clock::now();
	if (_state == State::stale)
	{
		_state = State::discovered;
	}
}