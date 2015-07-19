#pragma once

class StopWatch
{
private:
	LARGE_INTEGER   mFreq, mStart, mEnd;
	float                   mTimeforDuration;
public:
	StopWatch() : mTimeforDuration(0)
	{
		mFreq.LowPart = mFreq.HighPart = 0;
		mStart = mFreq;
		mEnd = mFreq;
		QueryPerformanceFrequency(&mFreq);
	}
	~StopWatch()
	{
	}

public:
	void Start(){ QueryPerformanceCounter(&mStart); }
	void Stop()
	{
		QueryPerformanceCounter(&mEnd);
		mTimeforDuration = (mEnd.QuadPart - mStart.QuadPart) / (float)mFreq.QuadPart;
	}
	float GetDurationSecond() { return mTimeforDuration; }
	float GetDurationMilliSecond() { return mTimeforDuration * 1000.f; }

};