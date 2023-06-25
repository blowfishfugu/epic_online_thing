#pragma once
#include "audio.h" //struct s_sound

#define m_gl_funcs \
X(PFNGLBUFFERDATAPROC, glBufferData) \
X(PFNGLBUFFERSUBDATAPROC, glBufferSubData) \
X(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays) \
X(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray) \
X(PFNGLGENBUFFERSPROC, glGenBuffers) \
X(PFNGLBINDBUFFERPROC, glBindBuffer) \
X(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer) \
X(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray) \
X(PFNGLCREATESHADERPROC, glCreateShader) \
X(PFNGLSHADERSOURCEPROC, glShaderSource) \
X(PFNGLCREATEPROGRAMPROC, glCreateProgram) \
X(PFNGLATTACHSHADERPROC, glAttachShader) \
X(PFNGLLINKPROGRAMPROC, glLinkProgram) \
X(PFNGLCOMPILESHADERPROC, glCompileShader) \
X(PFNGLVERTEXATTRIBDIVISORPROC, glVertexAttribDivisor) \
X(PFNGLDRAWARRAYSINSTANCEDPROC, glDrawArraysInstanced) \
X(PFNGLDEBUGMESSAGECALLBACKPROC, glDebugMessageCallback) \
X(PFNGLBINDBUFFERBASEPROC, glBindBufferBase) \
X(PFNGLUNIFORM2FVPROC, glUniform2fv) \
X(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation) \
X(PFNGLUSEPROGRAMPROC, glUseProgram) \
X(PFNGLGETSHADERIVPROC, glGetShaderiv) \
X(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog) \
X(PFNGLGENFRAMEBUFFERSPROC, glGenFramebuffers) \
X(PFNGLBINDFRAMEBUFFERPROC, glBindFramebuffer) \
X(PFNGLFRAMEBUFFERTEXTURE2DPROC, glFramebufferTexture2D) \
X(PFNGLCHECKFRAMEBUFFERSTATUSPROC, glCheckFramebufferStatus) \
X(PFNGLACTIVETEXTUREPROC, glActiveTexture) \
X(PFNWGLSWAPINTERVALEXTPROC, wglSwapIntervalEXT) \
X(PFNWGLGETSWAPINTERVALEXTPROC, wglGetSwapIntervalEXT) \
X(PFNGLBLENDEQUATIONPROC, glBlendEquation) \
X(PFNGLDELETEPROGRAMPROC, glDeleteProgram) \
X(PFNGLDELETESHADERPROC, glDeleteShader) \
X(PFNGLUNIFORM1FPROC, glUniform1f)


constexpr int key_backspace = 0x08;
constexpr int key_tab = 0x09;
constexpr int key_enter = 0x0D;
constexpr int key_alt = 0x12;
constexpr int key_escape = 0x1B;
constexpr int key_space = 0x20;
constexpr int key_end = 0x23;
constexpr int key_home = 0x24;
constexpr int key_left = 0x25;
constexpr int key_up = 0x26;
constexpr int key_right = 0x27;
constexpr int key_down = 0x28;
constexpr int key_delete = 0x2E;
constexpr int key_0 = 0x30;
constexpr int key_1 = 0x31;
constexpr int key_2 = 0x32;
constexpr int key_3 = 0x33;
constexpr int key_4 = 0x34;
constexpr int key_5 = 0x35;
constexpr int key_6 = 0x36;
constexpr int key_7 = 0x37;
constexpr int key_8 = 0x38;
constexpr int key_9 = 0x39;
constexpr int key_a = 0x41;
constexpr int key_b = 0x42;
constexpr int key_c = 0x43;
constexpr int key_d = 0x44;
constexpr int key_e = 0x45;
constexpr int key_f = 0x46;
constexpr int key_g = 0x47;
constexpr int key_h = 0x48;
constexpr int key_i = 0x49;
constexpr int key_j = 0x4A;
constexpr int key_k = 0x4B;
constexpr int key_l = 0x4C;
constexpr int key_m = 0x4D;
constexpr int key_n = 0x4E;
constexpr int key_o = 0x4F;
constexpr int key_p = 0x50;
constexpr int key_q = 0x51;
constexpr int key_r = 0x52;
constexpr int key_s = 0x53;
constexpr int key_t = 0x54;
constexpr int key_u = 0x55;
constexpr int key_v = 0x56;
constexpr int key_w = 0x57;
constexpr int key_x = 0x58;
constexpr int key_y = 0x59;
constexpr int key_z = 0x5A;
constexpr int key_add = 0x6B;
constexpr int key_subtract = 0x6D;
constexpr int key_f1 = 0x70;
constexpr int key_f2 = 0x71;
constexpr int key_f3 = 0x72;
constexpr int key_f4 = 0x73;
constexpr int key_f5 = 0x74;
constexpr int key_f6 = 0x75;
constexpr int key_f7 = 0x76;
constexpr int key_f8 = 0x77;
constexpr int key_f9 = 0x78;
constexpr int key_f10 = 0x79;
constexpr int key_f11 = 0x7A;
constexpr int key_f12 = 0x7B;
constexpr int key_left_shift = 0xA0;
constexpr int key_right_shift = 0xA1;
constexpr int key_left_control = 0xA2;
constexpr int key_right_control = 0xA3;
constexpr int c_max_keys = 1024;


typedef void* (*t_load_gl_func)(const char*);
typedef b8 (*t_play_sound)(s_sound);

struct s_game_window
{
	int width;
	int height;

	// @Note(tkap, 24/06/2023): Set by the game
	s_v2 size;
	s_v2 center;
};

struct s_char_event
{
	b8 is_symbol;
	int c;
};

struct s_stored_input
{
	b8 is_down;
	int key;
};

struct s_key
{
	b8 is_down;
	int count;
};

struct s_input
{
	s_key keys[c_max_keys];
};

struct s_platform_data
{
	b8 quit_after_this_frame;
	int window_width;
	int window_height;
	f64 time_passed;
	s_input* input;
	s_sarray<s_char_event, 1024>* char_event_arr;
};

struct s_platform_funcs
{
	t_load_gl_func load_gl_func;
	t_play_sound play_sound;
};


void update_game(s_platform_data platform_data, s_platform_funcs platform_funcs);