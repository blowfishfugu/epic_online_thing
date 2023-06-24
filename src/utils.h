

#define zero {0}
#define func static
#define global static
#define null NULL
#define invalid_default_case default: { assert(false); }

#define error(b) do { if(!(b)) { printf("ERROR\n"); exit(1); }} while(0)
#define assert(cond) do { if(!(cond)) { on_failed_assert(#cond, __FILE__, __LINE__); } } while(0)
#define check(cond) do { if(!(cond)) { error(false); }} while(0)
#define unreferenced(thing) (void)thing;

#define c_kb (1024)
#define c_mb (1024 * 1024)

#define array_count(arr) (sizeof((arr)) / sizeof((arr)[0]))

#define true 1
#define false 0

#ifdef m_server
#define log(...) printf("Server: "  __VA_ARGS__); printf("\n")
#else // m_server
#define log(...) printf("Client: " __VA_ARGS__); printf("\n")
#endif

#define make_list(name, type, max_elements) struct name { \
	int count; \
	type elements[max_elements]; \
}; \
func void name##_add(name* list, type element) \
{ \
	assert(list->count < max_elements); \
	list->elements[list->count] = element; \
	list->count += 1; \
}

func void on_failed_assert(const char* cond, const char* file, int line);

func void* buffer_read(u8** cursor, size_t size)
{
	void* result = *cursor;
	*cursor += size;
	return result;
}

func void buffer_write(u8** cursor, void* data, size_t size)
{
	memcpy(*cursor, data, size);
	*cursor += size;
}

func char* format_text(const char* format, ...)
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
	if(index >= max_format_text_buffers) { index = 0; }

	return current_buffer;
}

func void on_failed_assert(const char* cond, const char* file, int line)
{
	char* text = format_text("FAILED ASSERT IN %s (%i)\n%s\n", file, line, cond);
	printf("%s\n", text);
	int result = MessageBox(null, text, "Assertion failed", MB_RETRYCANCEL | MB_TOPMOST);
	if(result != IDRETRY)
	{
		if(IsDebuggerPresent())
		{
			__debugbreak();
		}
		else
		{
			exit(1);
		}
	}
}
