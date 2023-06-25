
u64 g_cycle_frequency;
u64 g_start_cycles;

float time_passed;
float total_time;

f64 get_seconds(void)
{
	u64 now;
	QueryPerformanceCounter((LARGE_INTEGER*)&now);
	return (now - g_start_cycles) / (f64)g_cycle_frequency;
}

void init_performance(void)
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&g_cycle_frequency);
	QueryPerformanceCounter((LARGE_INTEGER*)&g_start_cycles);
}