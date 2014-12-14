//
//  Connection.cpp
//  LEGO Control
//
//  Created by David Giovannini on 11/21/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#import <ExternalAccessory/ExternalAccessory.h>

#include "SBJEV3ConnectionIOS.h"
#include "re_dump.h"

NSString* const LEGOAccessoryProtocol = @"COM.LEGO.MINDSTORMS.EV3";

using namespace SBJ::EV3;

/*
 * Only an objective c class can be a NSStreamDelegate to handle replies
 * The ConnectionIOS acts as a smart pointer to the EV3Accessory
 */

@interface EV3Accessory : NSObject<NSStreamDelegate>

- (id) initWithAccessory: (EAAccessory*) accessory;
- (void) start: (Connection::Read) read;
- (bool) write: (const uint8_t*) buffer len: (size_t) len;
- (void) close;

@end

ConnectionIOS::ConnectionIOS(EAAccessory* accessory)
: _delegate ([[EV3Accessory alloc] initWithAccessory: accessory])
{
}

ConnectionIOS::~ConnectionIOS()
{
	[_delegate close]; // dealloc not called from below...
	_delegate = nil;
}

Connection::Type ConnectionIOS::type() const
{
#if (TARGET_IPHONE_SIMULATOR)
	return Connection::Type::simulator;
#else
	return Connection::Type::bluetooth;
#endif
}


void ConnectionIOS::start(Read read)
{
	[_delegate start: read];
}

bool ConnectionIOS::write(const uint8_t* buffer, size_t len)
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
}

- (id) initWithAccessory: (EAAccessory*) accessory
{
	self = [super init];
	_accessory = accessory;
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
#else
	_thread = [[NSThread alloc] initWithTarget: self selector: @selector(createSession) object: nil];
	_thread.qualityOfService = NSQualityOfServiceUserInteractive;
	[_thread start];
#endif
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
#if (TARGET_IPHONE_SIMULATOR)
	log_dump(buffer, (int)len, 16);
	return false;
//	dispatch_async(dispatch_get_global_queue(long identifier, unsigned long flags), ^
//	{
//		TODO: come up with a decent simulated reply
//		if (_read) _read(buffer, bytesRead);
//	});
//  return true;
#else
	if (_session == nil) return false;
	// Post the package onto the background run loop
	SendPackage* package = [[SendPackage alloc] init];
	package.data = [NSData dataWithBytes: buffer length: len];
	[self performSelector: @selector(sendData:) onThread: _thread withObject: package waitUntilDone: YES modes: @[NSDefaultRunLoopMode]];
	return package.sent;
#endif
}

- (void) sendData: (SendPackage*) package
{
	NSOutputStream* stream = _session.outputStream;
	const uint8_t* writing = (uint8_t*)package.data.bytes;
	size_t bytesToWrite = package.data.length;
	
	while (bytesToWrite > 0 && [stream hasSpaceAvailable])
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

- (void)stream:(NSStream*)theStream handleEvent:(NSStreamEvent)streamEvent
{
	switch (streamEvent)
	{
		case NSStreamEventHasSpaceAvailable:
		// TODO: implement a logging mechanism
			//NSLog(@"NSStreamEventHasSpaceAvailable %@", theStream.class.description);
			break;
		case NSStreamEventOpenCompleted:
			//NSLog(@"NSStreamEventOpenCompleted %@", theStream.class.description);
			break;
		case NSStreamEventHasBytesAvailable:
		{
			//NSLog(@"NSStreamEventHasBytesAvailable %@", theStream.class.description);
			NSInputStream* input = [_session inputStream];
			if (input == theStream)
			{
				[self readData];
			}
			break;
		}
		case NSStreamEventErrorOccurred:
			//NSLog(@"NSStreamEventErrorOccurred %@", theStream.class.description);
			break;
		case NSStreamEventEndEncountered:
			//NSLog(@"NSStreamEventEndEncountered %@", theStream.class.description);
			break;
	}
}

- (void) readData
{
	NSInputStream* stream = _session.inputStream;
	uint8_t buffer[1024];
	uint8_t* reading = buffer;
	size_t bytesNotRead = sizeof(buffer);
	
	while ([stream hasBytesAvailable])
	{
		NSInteger bytesRead = [stream read: reading maxLength: bytesNotRead];
		if (bytesRead == -1)
		{
			bytesNotRead = sizeof(buffer);
			break;
		}
		bytesNotRead -= bytesRead;
		reading += bytesRead;
	}
	uint8_t bytesRead = sizeof(buffer)-bytesNotRead;
	if (_read) _read(buffer, bytesRead);
}

@end