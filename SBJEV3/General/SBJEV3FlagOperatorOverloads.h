//
//  SBJEV3FlagOperatorOverloads.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/22/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#pragma once

#define FlagOperatorOverloads(Type) \
\
inline Type operator | (Type lhs, Type rhs) \
{ \
return (Type)((__underlying_type(Type))lhs | (__underlying_type(Type))rhs); \
} \
\
inline Type& operator |= (Type& lhs, Type rhs) \
{ \
lhs = (Type)((__underlying_type(Type))lhs | (__underlying_type(Type))rhs); \
return lhs; \
} \
\
inline Type operator & (Type lhs, Type rhs) \
{ \
return (Type)((__underlying_type(Type))lhs & (__underlying_type(Type))rhs); \
} \
\
inline Type& operator &= (Type& lhs, Type rhs) \
{ \
lhs = (Type)((__underlying_type(Type))lhs & (__underlying_type(Type))rhs); \
return lhs; \
} \
\
inline bool operator && (Type lhs, Type rhs) \
{ \
return ((__underlying_type(Type))lhs & (__underlying_type(Type))rhs) == (__underlying_type(Type))rhs; \
} \
\
inline bool operator || (Type lhs, Type rhs) \
{ \
    return ((__underlying_type(Type))lhs & (__underlying_type(Type))rhs) != (__underlying_type(Type))0; \
}
