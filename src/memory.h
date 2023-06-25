#pragma once
#define c_max_arena_push 16

struct s_lin_arena
{
	int push_count;
	size_t push[c_max_arena_push];
	size_t used;
	size_t capacity;
	void* memory;
};

s_lin_arena make_lin_arena(size_t capacity);

void* la_get(s_lin_arena* arena, size_t in_requested);

void la_push(s_lin_arena* arena);

void la_pop(s_lin_arena* arena);
