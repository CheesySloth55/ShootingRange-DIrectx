#pragma once

#include <Windows.h>

class TimerClass
{
public:
	TimerClass();
	TimerClass(const TimerClass& other);
	~TimerClass();

	bool Initialize();
	void Frame();

	float GetFrameTime();

private:
	float m_frequency{};
	INT64 m_startTime{};
	float m_frameTime{};
};