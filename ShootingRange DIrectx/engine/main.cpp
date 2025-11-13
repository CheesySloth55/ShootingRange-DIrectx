#include "core/systemclass.h"
#include <exception>
#include <fstream>


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System;
	std::ofstream inf{ "ExceptionInfo.txt" };
	bool result;

	try
	{
		System = new SystemClass;

		result = System->Initialize();
		if (result)
		{
			System->Run();
		}

		System->Shutdown();
		delete System;
		System = 0;
	}
	catch (std::exception except)
	{
		if (!inf)
		{
			return false;
		}

		inf << except.what();
	}


	return 0;
}