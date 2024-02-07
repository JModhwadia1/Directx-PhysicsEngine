#pragma once
#include <chrono>

using namespace std::chrono;
class Timer
{
public:
	Timer();

	float GetDeltaTime(); // Request time since last frame
	void Tick(); // Store time of last frame

private:
	steady_clock::time_point lastFrame; // std::chrono::steady_clock
};

