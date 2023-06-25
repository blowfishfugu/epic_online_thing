#pragma once


struct s_window
{
	HDC dc;
	HWND handle;
	int width;
	int height;
};

struct s_gamepad
{
	int left_thumb_x;
	int previous_left_thumb_x;
	int previous_buttons;
	int buttons;
};

struct s_voice : IXAudio2VoiceCallback
{
	IXAudio2SourceVoice* voice;

	volatile int playing;
	// s_sound sound;

	void OnStreamEnd()
	{
		// assert(sound.sample_count > 0);
		// assert(sound.samples);
		// free(sound.samples);
		InterlockedExchange((LONG*)&playing, false);
		voice->Stop();
	}

	void OnBufferStart(void * pBufferContext)
	{
		unreferenced(pBufferContext);
		InterlockedExchange((LONG*)&playing, true);
	}

	void OnVoiceProcessingPassEnd() { }
	void OnVoiceProcessingPassStart(UINT32 SamplesRequired) { unreferenced(SamplesRequired); }
	void OnBufferEnd(void * pBufferContext) { unreferenced(pBufferContext); }
	void OnLoopEnd(void * pBufferContext) { unreferenced(pBufferContext); }
	void OnVoiceError(void * pBufferContext, HRESULT Error) { unreferenced(pBufferContext); unreferenced(Error);}
};



void create_window(int width, int height);
WPARAM remap_key_if_necessary(WPARAM vk, LPARAM lparam);
PROC load_gl_func( const char* name);
bool init_audio();
b8 thread_safe_set_bool_to_true(volatile int* var);
b8 play_sound(s_sound sound);
void init_performance();
f64 get_seconds();
void do_gamepad_shit(void);