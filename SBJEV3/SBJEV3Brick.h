//
//  SBJEV3Brick.h
//  LEGO Control
//
//  Created by David Giovannini on 11/25/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3InvocationStack.h"
#include "SBJEV3DirectCommand.h"
#include "SBJEV3SystemCommand.h"
#include "SBJEV3Connection.h"
#include "SBJEV3DeviceIdentifier.h"
#include "SBJEV3DeleteMethods.h"

#include <memory>

namespace SBJ
{
namespace EV3
{

class ConnectionFactory;
class ConnectionToken;
enum class PromptBluetoothError : int;
	
/*
 * The brick is the high-level object that represents an EV3.
 * It knows how to respond to connection events and create direct commands.
 */
 
class Brick
{
public:
	DeleteDefaultMethods(Brick);
	
	struct Battery
	{
		uint8_t level;
		float voltage;
		float current;
		float tempuratureRise;
		
		float indicator() const { return (float)level / 100.0; }
		float power() const { return voltage * current; }
		float resistence() const { return voltage / current; }
	};
	
	struct Version
	{
		std::string hardwareVersion;
		std::string firmwareVersion;
		std::string firmwareBuild;
		std::string oSVersion;
		std::string oSBuild;
		std::string fullVersion;
		
		void clear()
		{
			hardwareVersion.clear();
			firmwareVersion.clear();
			firmwareBuild.clear();
			oSVersion.clear();
			oSBuild.clear();
			fullVersion.clear();
		}
	};

	using ConnectionChanged = std::function<void(Brick& brick)>;
	using PromptBluetoothErrored =  std::function<void(Brick& brick, PromptBluetoothError error)>;

	Brick(ConnectionFactory& factory, const DeviceIdentifier& identifier = DeviceIdentifier());
	
	~Brick();
	
	ConnectionChanged connectionEvent;
	
	bool isConnected() const;
	
	void promptForBluetooth(PromptBluetoothErrored errored);
	
	void disconnect();

	const DeviceIdentifier& identifier() const
	{
		return _identifier;
	}
	
	Connection::Type connectionType() const
	{
		return _connectionType;
	}
	
	const std::string& name() const
	{
		return _name;
	}
	
	Battery battery();
	
	Log& log() const
	{
		return _log;
	}
	
	void setName(const std::string& name);

	template <typename...  Opcodes>
	typename DirectCommand<Opcodes...>::Results directCommand(float timeout, Opcodes... opcodes)
	{
		DirectCommand<Opcodes...> command(_messageCounter, timeout, opcodes...);
		_messageCounter++;
		Invocation invocation(std::move(command.invocation()));
		InvocationScope invocationScope(_stack, invocation);
		return command.wait();
	}

	template <typename  Opcode>
	typename SystemCommand<Opcode>::Results systemCommand(float timeout, Opcode opcode)
	{
		SystemCommand<Opcode> command(_messageCounter, timeout, opcode);
		_messageCounter++;
		Invocation invocation(std::move(command.invocation()));
		InvocationScope invocationScope(_stack, invocation);
		return command.wait();
	}

private:
	Log& _log;
	DeviceIdentifier _identifier;
	std::string _name;
	Version _version;
	InvocationStack _stack;
	Connection::Type _connectionType = Connection::Type::none;
	std::unique_ptr<ConnectionToken> _token;
	unsigned short _messageCounter = 0;
	
	void handleConnectionChange(const DeviceIdentifier& updatedIdentifier, std::unique_ptr<Connection>& connection);
};
	
}
}
