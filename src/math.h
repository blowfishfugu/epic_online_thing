#pragma once
#include "types.h"
#include <numbers>
constexpr const float pi=static_cast<float>(std::numbers::pi);

struct s_v2
{
	float x;
	float y;
};

struct s_v4
{
	float x;
	float y;
	float z;
	float w;
};

s_v2 v2(float x, float y);

s_v2 v22i(int x, int y);

s_v2 v2ii(int x, int y);

s_v2 v2_mul(s_v2 a, float b);

s_v4 v4(float x, float y, float z, float w);
s_v4 v41f(float v);

b8 rect_collides_circle(s_v2 rect_center, s_v2 rect_size, s_v2 center, float radius);

s_v2 v2_from_angle(float angle);

s_v2 v2_sub(s_v2 a, s_v2 b);

float v2_angle(s_v2 v);

float lerp(float a, float b, float t);

int roundfi(float x);
