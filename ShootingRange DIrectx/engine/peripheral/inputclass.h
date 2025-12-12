#pragma once 

//pre processing directives
#define DIRECTINPUT_VERSION 0x0800


//linking 
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "Xinput.lib")

//includes
#include <dinput.h>


class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	bool IsEscapePressed();
	bool IsSpacePressed();
	bool IsControlPressed();
	bool IsF11Pressed();

	bool Is1Pressed();
	bool Is2Pressed();

	bool IsUPArrowPressed();
	bool IsDOWNArrowPressed();
	bool IsLEFTArrowPressed();
	bool IsRIGHTArrowPressed();

	bool IsWKeyPressed();
	bool IsAKeyPressed();
	bool IsSKeyPressed();
	bool IsDKeyPressed();

	void GetMouseLocation(int& x, int& y);
	bool IsLeftMousePressed();
	bool IsRightMousePressed();


private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

private:
	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;

	
	unsigned char m_keyboardState[256];
	DIMOUSESTATE m_mouseState;

	int m_screenWidth, m_screenHeight, m_mouseX, m_mouseY;
};
