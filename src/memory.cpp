
s_lin_arena make_lin_arena(size_t capacity)
{
	assert(capacity > 0);
	capacity = (capacity + 7) & ~7;
	s_lin_arena result = zero;
	result.capacity = capacity;
	result.memory = VirtualAlloc(nullptr, capacity, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	return result;
}

void* la_get(s_lin_arena* arena, size_t in_requested)
{
	assert(in_requested > 0);
	size_t requested = (in_requested + 7) & ~7;
	assert(arena->used + requested <= arena->capacity);
	void* result = (u8*)arena->memory + arena->used;
	arena->used += requested;
	return result;
}

void la_push(s_lin_arena* arena)
{
	assert(arena->push_count < c_max_arena_push);
	arena->push[arena->push_count++] = arena->used;
}

void la_pop(s_lin_arena* arena)
{
	assert(arena->push_count > 0);
	arena->used = arena->push[--arena->push_count];
}