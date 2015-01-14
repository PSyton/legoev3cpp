//
//  Connection.cpp
//  LEGO Control
//
//  Created by David Giovannini on 11/21/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#import <ExternalAccessory/ExternalAccessory.h>

#include "SBJEV3BluetoothConnectionIOS.h"
#include "SBJEV3Log.h"
#include "SBJEV3Chunk.h"

#include <thread>

NSString* const LEGOAccessoryProtocol = @"COM.LEGO.MINDSTORMS.EV3";

static const std::string LogDomian = "Bluetooth";

using namespace SBJ::EV3;

/*
 * Only an objective c class can be a NSStreamDelegate to handle replies
 * The BluetoothConnectionIOS acts as a smart pointer to the EV3Accessory
 */

@interface EV3Accessory : NSObject<NSStreamDelegate>
{
	std::mutex _mutex;
	std::condition_variable _isReady;
	int _openStreams;
}

- (id) init: (Log&) log withAccessory: (EAAccessory*) accessory;
- (void) start: (Connection::Read) read;
- (bool) write: (const uint8_t*) buffer len: (size_t) len;
- (void) close;

@end

BluetoothConnectionIOS::BluetoothConnectionIOS(Log& log, EAAccessory* accessory)
: _delegate ([[EV3Accessory alloc] init: log withAccessory: accessory])
{
}

BluetoothConnectionIOS::~BluetoothConnectionIOS()
{
	// Sessions use strong reference delegates - must break circular reference
	[_delegate close];
	_delegate = nil;
}

Connection::Type BluetoothConnectionIOS::type() const
{
#if (TARGET_IPHONE_SIMULATOR)
	return Connection::Type::simulator;
#else
	return Connection::Type::bluetooth;
#endif
}


void BluetoothConnectionIOS::start(Read read)
{
	[_delegate start: read];
}

bool BluetoothConnectionIOS::write(const uint8_t* buffer, size_t len)
{
	return [_delegate write: buffer len: len];
}

@interface SendPackage : NSObject
@property (nonatomic) NSData* data;
@property (atomic) bool sent;
@end

@implementation SendPackage
@end

@implementation EV3Accessory
{
	EAAccessory* _accessory;
	EASession* _session;
	NSRunLoop* _runLoop;
	NSThread* _thread;
	Connection::Read _read;
	Log* _log;
}

- (id) init: (Log&) log withAccessory: (EAAccessory*) accessory;
{
	self = [super init];
	_accessory = accessory;
	_log = &log;
	return self;
}

- (void) dealloc
{
	[self close];
}

- (void) close
{
	[self closeStream: _session.inputStream];
	[self closeStream: _session.outputStream];
	_session = nil;
}

- (void) closeStream: (NSStream*) stream
{
	[stream close];
	[stream removeFromRunLoop: _runLoop forMode: NSDefaultRunLoopMode];
	stream.delegate = nil;
}

- (void) start: (Connection::Read) read
{
	_read = read;
#if (TARGET_IPHONE_SIMULATOR)
	dispatch_async(dispatch_get_global_queue(QOS_CLASS_BACKGROUND, 0), ^
	{
		{
			std::unique_lock<std::mutex> lock(_mutex);
			_openStreams=3;
		}
		_isReady.notify_one();
	});
#else
	_thread = [[NSThread alloc] initWithTarget: self selector: @selector(createSession) object: nil];
	_thread.qualityOfService = NSQualityOfServiceUserInteractive;
	[_thread start];
	{
		std::unique_lock<std::mutex> lock(_mutex);
		_isReady.wait_for(lock, std::chrono::milliseconds(1000), ^{return _openStreams == 3;});
	}
#endif
	_log->write(LogDomian, "Ready");
}


- (void) createSession
{
#if (TARGET_IPHONE_SIMULATOR)
#else
	if (_accessory)
	{
		_session = [[EASession alloc] initWithAccessory: _accessory forProtocol: LEGOAccessoryProtocol];
		if (_session)
		{
			_runLoop = [NSRunLoop currentRunLoop];
			_thread = [NSThread currentThread];
			
			NSInputStream* inputStream = _session.inputStream;
			inputStream.delegate = self;
			[inputStream scheduleInRunLoop: _runLoop forMode:NSDefaultRunLoopMode];
			[inputStream open];
			
			NSOutputStream* outputStream = _session.outputStream;
			outputStream.delegate = self;
			[outputStream scheduleInRunLoop: _runLoop forMode:NSDefaultRunLoopMode];
			[outputStream open];
			
			// Streams expect a runloop running in their thread
			[_runLoop run];
		}
	}
#endif
}

- (bool) write: (const uint8_t*) buffer len: (size_t) len
{
	if (_session == nil) return false;
	SendPackage* package = [[SendPackage alloc] init];
	package.data = [NSData dataWithBytes: buffer length: len];
	[self performSelector: @selector(sendData:) onThread: _thread withObject: package waitUntilDone: YES modes: @[NSDefaultRunLoopMode]];
	return package.sent;
}

- (void) sendData: (SendPackage*) package
{
	NSOutputStream* stream = _session.outputStream;
	const uint8_t* writing = (uint8_t*)package.data.bytes;
	size_t bytesToWrite = package.data.length;
	
	while (bytesToWrite > 0 and [stream hasSpaceAvailable])
	{
		NSInteger bytesWritten = [stream write: writing maxLength: bytesToWrite];
		if (bytesWritten == -1)
		{
			break;
		}
		bytesToWrite -= bytesWritten;
		writing += bytesWritten;
	}
	package.sent = (bytesToWrite == 0);
}

#define ExtendedLogging 1

- (void)stream:(NSStream*)theStream handleEvent:(NSStreamEvent)streamEvent
{
	bool ready = false;
	switch (streamEvent)
	{
		case NSStreamEventHasSpaceAvailable:
		{
#if ExtendedLogging
			_log->write(LogDomian, theStream.class.description, " - Space ");
#endif
			NSOutputStream* output = [_session outputStream];
			if (output == theStream)
			{
				std::unique_lock<std::mutex> lock(_mutex);
				_openStreams++;
				ready = (_openStreams==3);
			}
			break;
		}
		case NSStreamEventOpenCompleted:
		{
#if ExtendedLogging
			_log->write(LogDomian, theStream.class.description, " - Open ");
#endif
			{
				std::unique_lock<std::mutex> lock(_mutex);
				_openStreams++;
				ready = (_openStreams==3);
			}
			break;
		}
		case NSStreamEventHasBytesAvailable:
		{
#if ExtendedLogging
			_log->write(LogDomian, theStream.class.description, " - Bytes ");
#endif
			NSInputStream* input = [_session inputStream];
			if (input == theStream)
			{
				[self readData];
			}
			break;
		}
		case NSStreamEventEndEncountered:
		{
#if ExtendedLogging
			_log->write(LogDomian, theStream.class.description, " - End ");
#endif
			break;
		}
		case NSStreamEventErrorOccurred:
		{
			NSError* theError = [theStream streamError];
			_log->write(LogDomian, theStream.class.description, " - Error(", theError.code, ") ", theError.localizedDescription);
			[self close];
			break;
		}
	}
	if (ready)
	{
		_isReady.notify_one();
	}
}

- (void) readData
{
	NSInputStream* stream = _session.inputStream;
	Chunk<2048> buffer;
	while (stream.hasBytesAvailable)
	{
		uint8_t* ptr = buffer.writePtr();
		NSInteger bytesRead = [stream read: ptr maxLength: buffer.NaturalSize];
		if (bytesRead == -1)
		{
			break;
		}
		buffer.actualBytesWrittenTo(bytesRead, ptr);
	}
	if (_read) _read(buffer, buffer.size());
}

@end