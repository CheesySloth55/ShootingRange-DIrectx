#pragma once

//skipping some headers to increase build speed
#define WIN32_LEAN_AND_MEAN

//includes
#include <windows.h>
#include <thread>
//my classes
#include "../peripheral/inputclass.h"
#include "applicationclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: SystemClass
////////////////////////////////////////////////////////////////////////////////
class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

	void startThread();
private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	bool m_done;


	InputClass* m_Input;
	ApplicationClass* m_Application;
	std::jthread m_frameThread;
	std::jthread m_inputThread;
	std::jthread m_renderThread1;
	std::jthread m_renderThread2;

};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Globals
static SystemClass* ApplicationHandle = 0;