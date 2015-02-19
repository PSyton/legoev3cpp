//
//  TransportSelection.h
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

enum class ConnectionTransport
{
	none = 0,
	usb,
	wifi,
	bluetooth,
};
#define ConnectionTransportCount 4

/*
 * TransportSelection is an ordered list of ConnectionTransports used for flagging availability and preference
 */

class TransportSelection
{
public:
	TransportSelection()
	: _transport{ConnectionTransport::usb, ConnectionTransport::wifi, ConnectionTransport::bluetooth}
	{
	}
	
	TransportSelection(ConnectionTransport transport)
	: _transport{transport, ConnectionTransport::none, ConnectionTransport::none}
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
	
	void insert(ConnectionTransport transport)
	{
		int place = -1;
		for (int i = 0; i < 3; i++)
		{
			if (_transport[i] == transport)
			{
				return;
			}
			if (_transport[i] == ConnectionTransport::none)
			{
				place = i;
			}
		}
		if (place != -1)
		{
			_transport[place] = transport;
		}
	}
	
	bool erase(ConnectionTransport transport)
	{
		bool stillGotOne = false;
		for (int i = 0; i < 3; i++)
		{
			if (_transport[i] == transport)
			{
				_transport[i] = ConnectionTransport::none;
			}
			else if (_transport[i] != ConnectionTransport::none)
			{
				stillGotOne |= true;
			}
		}
		return stillGotOne;
	}
	
	bool find(ConnectionTransport transport) const
	{
		for (auto i : *this)
		{
			if (i == transport)
			{
				return true;
			}
		}
		return false;
	}
	
	bool findAny() const
	{
		for (auto i : *this)
		{
			if (i != ConnectionTransport::none)
			{
				return true;
			}
		}
		return false;
	}
	
	ConnectionTransport operator [] (int i) const
	{
		return _transport[i];
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