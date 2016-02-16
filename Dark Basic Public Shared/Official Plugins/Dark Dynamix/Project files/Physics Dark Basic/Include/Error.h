#pragma once
#ifndef PHY_ERROR
#define PHY_ERROR
#include "Message.h"
#include "Globals.h"

class DBError
{
public:
	static bool objectExists(int objectID, string function);
	static bool limbExists(int objectID, int limbID, string function);
	static bool meshExists(int objectID, string function);
	static bool meshExists(int objectID, int limbID, string function);
    static bool boneMeshExists(int objectID, int limbID, string function);
	static bool fileExists(string file, string function);
};
#endif
