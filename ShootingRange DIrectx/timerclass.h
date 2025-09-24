#pragma once;

//includes
#include <windows.h>


class TimerClass
{
public:
	TimerClass();
	TimerClass(const TimerClass& other);
	~TimerClass();

	bool Initialize();
	void Frame();

	float GetTime();

private:
	float m_frequency;
	INT64 m_startTime;
	float m_frameTime;
};
