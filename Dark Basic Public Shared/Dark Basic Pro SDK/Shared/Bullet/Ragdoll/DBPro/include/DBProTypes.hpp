/*
            File : Support/DBProTypes.hpp
   DBPro release : 6.6
*/

#ifndef INCLUDE_GUARD_Support_DBProTypes_hpp
#define INCLUDE_GUARD_Support_DBProTypes_hpp

#ifdef DBPRO__COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

// Function pointer definitions
// The first character after the underscore always denotes the return type.
// Further characters denote the argument type, unless it is void when it is
// ignored.
//
// v - void
// i - int

typedef void(__cdecl*function_vv)(void);
typedef void(__cdecl*function_vii)(int,int);

typedef struct tagCameraData sCamera;

#endif

