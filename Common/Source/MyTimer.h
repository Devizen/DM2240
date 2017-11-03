#ifndef MYTIMER_H
#define MYTIMER_H

#include "MyMath.h"

class MyTimer
{
	double elapsed_time;
	double duration;
public:
	MyTimer() : elapsed_time(0.0), duration(0.0) {}
	MyTimer(const MyTimer& other) { this->elapsed_time = other.elapsed_time; this->duration = other.duration; }
	~MyTimer() {}
	void set_duration(double duration) { this->duration = duration; }
	void update_timer(double dt) { elapsed_time = Math::Min(elapsed_time + dt, duration); }
	bool is_Duration_Passed() { return elapsed_time >= duration; }
	void reset_timer() { elapsed_time = 0.0; }
	double get_current_percent() { return elapsed_time / duration; }

	//getter
	double get_elapsed_time() { return elapsed_time; }
	double get_duration() { return duration; }
};

#endif // !MYTIMER_H

