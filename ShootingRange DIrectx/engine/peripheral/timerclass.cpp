#include "timerclass.h"

TimerClass::TimerClass()
{

}

TimerClass::TimerClass(const TimerClass& other)
{

}

TimerClass::~TimerClass()
{

}

bool TimerClass::Initialize()
{
	INT64 frequency{};

	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
	if (frequency == 0)
	{
		return false;
	}
	m_frequency = (float)(frequency);

	QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);

	return true;
}

void TimerClass::Frame()
{
	INT64 currTime{};
	INT64 elapsedTicks{};

	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	elapsedTicks = currTime - m_startTime;

	//calculates frametime
	m_frameTime = (float)(elapsedTicks) / m_frequency;

	m_startTime = currTime;

	return;
}

float TimerClass::GetFrameTime()
{
	return m_frameTime;
}