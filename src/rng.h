#pragma once

struct s_rng
{
	u32 seed;
};


u32 randu(s_rng* rng)
{
	rng->seed = rng->seed * 2147001325 + 715136305;
	return 0x31415926 ^ ((rng->seed >> 16) + (rng->seed << 16));
}

f64 randf(s_rng* rng)
{
	return (f64)randu(rng) / (f64)4294967295;
}

float randf32(s_rng* rng)
{
	return (float)randu(rng) / (float)4294967295;
}

f64 randf2(s_rng* rng)
{
	return randf(rng) * 2 - 1;
}

u64 randu64(s_rng* rng)
{
	return (u64)(randf(rng) * (f64)c_max_u64);
}


// min inclusive, max inclusive
int rand_range_ii(s_rng* rng, int min, int max)
{
	if(min > max)
	{
		int temp = min;
		min = max;
		max = temp;
	}

	return min + (randu(rng) % (max - min + 1));
}

// min inclusive, max exclusive
int rand_range_ie(s_rng* rng, int min, int max)
{
	if(min > max)
	{
		int temp = min;
		min = max;
		max = temp;
	}

	return min + (randu(rng) % (max - min));
}

float randf_range(s_rng* rng, float min_val, float max_val)
{
	if(min_val > max_val)
	{
		float temp = min_val;
		min_val = max_val;
		max_val = temp;
	}

	float r = (float)randf(rng);
	return min_val + (max_val - min_val) * r;
}