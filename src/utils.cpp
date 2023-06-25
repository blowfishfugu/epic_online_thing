#include "utils.h"
#include <Windows.h> //memcpy
#include <stdio.h> //vsnprintf

void on_failed_assert(const char* cond, const char* file, int line)
{
	char* text = format_text("FAILED ASSERT IN %s (%i)\n%s\n", file, line, cond);
	printf("%s\n", text);
	int result = MessageBox(nullptr, text, "Assertion failed", MB_RETRYCANCEL | MB_TOPMOST);
	if (result != IDRETRY)
	{
		if (IsDebuggerPresent())
		{
			__debugbreak();
		}
		else
		{
			exit(1);
		}
	}
}

void* buffer_read(u8** cursor, size_t size)
{
	void* result = *cursor;
	*cursor += size;
	return result;
}

void buffer_write(u8** cursor, void* data, size_t size)
{
	memcpy(*cursor, data, size);
	*cursor += size;
}

char* format_text(const char* format, ...)
{
#define max_format_text_buffers 16
#define max_text_buffer_length 256

	static char buffers[max_format_text_buffers][max_text_buffer_length] = zero;
	static int index = 0;

	char* current_buffer = buffers[index];
	memset(current_buffer, 0, max_text_buffer_length);

	va_list args;
	va_start(args, format);
#ifdef m_debug
	int written = vsnprintf(current_buffer, max_text_buffer_length, format, args);
	assert(written > 0 && written < max_text_buffer_length);
#else
	vsnprintf(current_buffer, max_text_buffer_length, format, args);
#endif
	va_end(args);

	index += 1;
	if (index >= max_format_text_buffers) { index = 0; }

	return current_buffer;
}
