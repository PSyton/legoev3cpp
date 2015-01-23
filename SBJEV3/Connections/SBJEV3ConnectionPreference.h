//
//  ConnectionPreference.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/22/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#pragma once

namespace SBJ
{
namespace EV3
{

enum class ConnectionTransport : char
{
	none = '\0',
	bluetooth = 'b',
	wifi = 'w',
	usb = 'u',
};

class ConnectionPreference
{
public:

	ConnectionPreference()
#if (TARGET_IPHONE_SIMULATOR)
	: _transport{ConnectionTransport::wifi, ConnectionTransport::bluetooth, ConnectionTransport::usb}
#else
	: _transport{ConnectionTransport::usb, ConnectionTransport::bluetooth, ConnectionTransport::wifi}
#endif
	{
	}
	
	void makePriority(ConnectionTransport transport)
	{
		ConnectionTransport t0 = _transport[0];
		if (t0 != transport)
		{
			_transport[0] = transport;
			ConnectionTransport t1 = _transport[1];
			_transport[1] = t0;
			if (t1 != transport)
			{
				_transport[2] = t1;
			}
		}
	}
	
	const ConnectionTransport* begin() const
	{
		return _transport;
	}
	
	const ConnectionTransport* end() const
	{
		return _transport + 3;
	}
	
private:
	ConnectionTransport _transport[3];
};

}
}