#pragma once
#include "types.h"

#define zero {}
#define error(b) do { if(!(b)) { printf("ERROR\n"); exit(1); }} while(0)
#define assert(cond) do { if(!(cond)) { on_failed_assert(#cond, __FILE__, __LINE__); } } while(0)
#define check(cond) do { if(!(cond)) { error(false); }} while(0)
#define unreferenced(thing) (void)thing;

#define c_kb (1024)
#define c_mb (1024 * 1024)

#define array_count(arr) (sizeof((arr)) / sizeof((arr)[0]))


#ifdef m_server
#define log(...) printf("Server: "  __VA_ARGS__); printf("\n")
#else // m_server
#define log(...) printf("Client: " __VA_ARGS__); printf("\n")
#endif

void on_failed_assert(const char* cond, const char* file, int line);

void* buffer_read(u8 * *cursor, size_t size);

void buffer_write(u8 * *cursor, void* data, size_t size);

char* format_text(const char* format, ...);

template <typename T, int N>
struct s_sarray
{
	static_assert(N > 0);
	int count = 0;
	T elements[N];

	constexpr T& operator[](int index)
	{
		assert(index >= 0);
		assert(index < count);
		return elements[index];
	}

	constexpr T get(int index)
	{
		return (*this)[index];
	}

	T pop()
	{
		assert(count > 0);
		return elements[--count];
	}

	constexpr void remove_and_swap(int index)
	{
		assert(index >= 0);
		assert(index < count);
		count -= 1;
		elements[index] = elements[count];
	}

	constexpr T remove_and_shift(int index)
	{
		assert(index >= 0);
		assert(index < count);
		T result = elements[index];
		count -= 1;

		int to_move = count - index;
		if(to_move > 0)
		{
			memcpy(elements + index, elements + index + 1, to_move * sizeof(T));
		}
		return result;
	}

	constexpr T* get_ptr(int index)
	{
		return &(*this)[index];
	}

	constexpr void swap(int index0, int index1)
	{
		assert(index0 >= 0);
		assert(index1 >= 0);
		assert(index0 < count);
		assert(index1 < count);
		assert(index0 != index1);
		T temp = elements[index0];
		elements[index0] = elements[index1];
		elements[index1] = temp;
	}

	constexpr T get_last()
	{
		assert(count > 0);
		return elements[count - 1];
	}

	constexpr T* get_last_ptr()
	{
		assert(count > 0);
		return &elements[count - 1];
	}

	constexpr int add(T element)
	{
		assert(count < N);
		elements[count] = element;
		count += 1;
		return count - 1;
	}

	constexpr b8 add_checked(T element)
	{
		if(count < N)
		{
			add(element);
			return true;
		}
		return false;
	}

	constexpr b8 contains(T what)
	{
		for(int element_i = 0; element_i < count; element_i++)
		{
			if(what == elements[element_i])
			{
				return true;
			}
		}
		return false;
	}

	constexpr void insert(int index, T element)
	{
		assert(index >= 0);
		assert(index < N);
		assert(index <= count);

		int to_move = count - index;
		count += 1;
		if(to_move > 0)
		{
			memmove(&elements[index + 1], &elements[index], to_move * sizeof(T));
		}
		elements[index] = element;
	}

	constexpr int max_elements()
	{
		return N;
	}

	constexpr b8 is_last(int index)
	{
		assert(index >= 0);
		assert(index < count);
		return index == count - 1;
	}

	constexpr b8 is_full()
	{
		return count >= N;
	}

	b8 is_empty()
	{
		return count <= 0;
	}

	void bubble_sort()
	{
		// @Note(tkap, 25/06/2023): Let's not get crazy with bubble sort, bro
		assert(count < 256);

		for(int i = 0; i < count; i++)
		{
			b8 swap = false;
			for(int j = 0; j < count - 1; j++)
			{
				T* a = &elements[j];
				T* b = &elements[j + 1];

				if(*a > *b)
				{
					T temp = *a;
					*a = *b;
					*b = temp;
					swap = true;
				}
			}
			if(!swap) { break; }
		}
	}

};