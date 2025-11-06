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

	QueryPerformanceCounter((LARGE_INTEGER*)&frequency);
	if (frequency == 0)
	{
		return false;
	}
	m_frequency = static_cast<float>(frequency);

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
	m_frameTime = static_cast<float>(elapsedTicks) / m_startTime;

	m_startTime = currTime;

	return;
}

float TimerClass::GetFrameTime()
{
	return m_frameTime;
}