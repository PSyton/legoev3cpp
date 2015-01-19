//
//  SBJEV3DeleteMethods.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/19/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#pragma once

#define DeleteDefaultMethods(ClassName) \
	ClassName() = delete; \
	ClassName(const ClassName&) = delete; \
	ClassName(ClassName&&) = delete; \
	ClassName& operator = (const ClassName&) = delete; \
	ClassName& operator = (ClassName&&) = delete

#define DeleteAllCopyMethods(ClassName) \
	ClassName(const ClassName&) = delete; \
	ClassName(ClassName&&) = delete; \
	ClassName& operator = (const ClassName&) = delete; \
	ClassName& operator = (ClassName&&) = delete

#define DeleteCopyMethods(ClassName) \
	ClassName(const ClassName&) = delete; \
	ClassName& operator = (const ClassName&) = delete

