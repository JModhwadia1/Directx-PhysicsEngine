#pragma once
#include "windows.h"
#include <stdio.h>
class Debug
{
public:
	Debug();
	~Debug();

	static int VDebugPrintF(const char* format, va_list args);

	
		
	static int DebugPrintF(const char* format, ...);
	
	
	
};

