#define m_client 1

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <gl/GL.h>

#include <winsock2.h>
#include <stdio.h>
#include <math.h>
#include "external\enet\enet.h"

#include "types.h"
#include "utils.h"
#include "math.h"
#include "config.h"
#include "shared.h"
#include "time.h"
#include "window.h"
#include "memory.h"
#include "file.h"
#include "client.h"
#include "rng.h"
#include "shader_shared.h"
#include "external/wglext.h"
#include "external/glcorearb.h"
#include "external/stb_truetype.h"

make_list(s_transform_list, s_transform, 1024);
s_transform_list transforms;
s_transform_list text_arr[e_font_count];

global s_entities e;
global u32 my_id = 0;
global ENetPeer* server;
global s_lin_arena frame_arena;
global s_rng rng;
global s_font g_font_arr[e_font_count];
global e_state state;
global b8 g_connected;
global ENetHost* g_client;
global s_main_menu main_menu;

#include "memory.c"
#include "window.c"
#include "draw.c"
#include "shared.c"
#include "file.c"

int main(int argc, char** argv)
{
	unreferenced(argc);
	unreferenced(argv);

	init_levels();

	assert((c_max_entities % c_num_threads) == 0);

	init_performance();

	if (enet_initialize() != 0)
	{
		error(false);
	}

	frame_arena = make_lin_arena(10 * c_mb);

	ENetHost* client = enet_host_create(
		null /* create a client host */,
		1, /* only allow 1 outgoing connection */
		2, /* allow up 2 channels to be used, 0 and 1 */
		0, /* assume any amount of incoming bandwidth */
		0 /* assume any amount of outgoing bandwidth */
	);

	if(client == null)
	{
		error(false);
	}

	ENetAddress address = zero;
	enet_address_set_host(&address, "at-taxation.at.ply.gg");
	// enet_address_set_host(&address, "127.0.0.1");
	address.port = 62555;

	server = enet_host_connect(client, &address, 2, 0);
	if(server == null)
	{
		error(false);
	}

	create_window();

	if(wglSwapIntervalEXT)
	{
		wglSwapIntervalEXT(1);
	}

	g_font_arr[e_font_small] = load_font("assets/consola.ttf", 24, &frame_arena);
	g_font_arr[e_font_medium] = load_font("assets/consola.ttf", 36, &frame_arena);
	g_font_arr[e_font_big] = load_font("assets/consola.ttf", 72, &frame_arena);

	u32 vao;
	u32 ssbo;
	u32 program;
	{
		u32 vertex = glCreateShader(GL_VERTEX_SHADER);
		u32 fragment = glCreateShader(GL_FRAGMENT_SHADER);
		char* header = "#version 430 core\n";
		char* vertex_src = read_file_quick("shaders/vertex.vertex", &frame_arena);
		char* fragment_src = read_file_quick("shaders/fragment.fragment", &frame_arena);
		assert(vertex_src && fragment_src); // "couldn't locate shaders in working directory";

		char* vertex_src_arr[] = {header, read_file_quick("src/shader_shared.h", &frame_arena), vertex_src};
		char* fragment_src_arr[] = {header, read_file_quick("src/shader_shared.h", &frame_arena), fragment_src};

		glShaderSource(vertex, array_count(vertex_src_arr), vertex_src_arr, null);
		glShaderSource(fragment, array_count(fragment_src_arr), fragment_src_arr, null);
		glCompileShader(vertex);
		char buffer[1024] = zero;
		check_for_shader_errors(vertex, buffer);
		glCompileShader(fragment);
		check_for_shader_errors(fragment, buffer);
		program = glCreateProgram();
		glAttachShader(program, vertex);
		glAttachShader(program, fragment);
		glLinkProgram(program);
		glUseProgram(program);
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(transforms.elements), null, GL_DYNAMIC_DRAW);

	b8 running = true;
	f64 update_timer = 0;
	while(running)
	{

		f64 start_of_frame_seconds = get_seconds();

		if(g_connected)
		{
			enet_loop(g_client, 0);
		}

		MSG msg = zero;
		while(PeekMessage(&msg, null, 0, 0, PM_REMOVE) > 0)
		{
			if(msg.message == WM_QUIT)
			{
				if(g_connected)
				{
					enet_peer_disconnect(server, 0);
					enet_loop(g_client, 1000);
				}
				running = false;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		update_timer += time_passed;
		while(update_timer >= c_update_delay)
		{
			update_timer -= c_update_delay;
			update();

			for(int k_i = 0; k_i < c_max_keys; k_i++)
			{
				g_input.keys[k_i].count = 0;
			}
		}

		render(program);

		frame_arena.used = 0;

		SwapBuffers(g_window.dc);

		time_passed = (float)(get_seconds() - start_of_frame_seconds);
		total_time += time_passed;
	}

	return 0;
}


func void update()
{
	switch(state)
	{
		case e_state_main_menu:
		{
			while(true)
			{
				s_char_event event = get_char_event();
				int c = event.c;
				if(!c) { break; }

				if(event.is_symbol)
				{
					if(c == key_backspace)
					{
						if(main_menu.player_name.len > 0)
						{
							main_menu.player_name.data[--main_menu.player_name.len] = 0;
						}
					}
					else if(c == key_enter)
					{
						if(main_menu.player_name.len < 3)
						{
							main_menu.error_str = "Character name needs to be at least 3 characters";
						}
						else
						{
							main_menu.error_str = null;
							state = e_state_game;
							connect_to_server();
							break;
						}
					}
				}
				else
				{
					if(c >= 32 && c <= 126)
					{
						if(main_menu.player_name.len < max_player_name_length)
						{
							main_menu.player_name.data[main_menu.player_name.len++] = (char)c;
						}
					}
				}
			}
		} break;

		case e_state_game:
		{
			spawn_system(levels[current_level]);

			level_timer += delta;

			for(int i = 0; i < c_num_threads; i++)
			{
				gravity_system(i * c_entities_per_thread, c_entities_per_thread);
			}
			for(int i = 0; i < c_num_threads; i++)
			{
				input_system(i * c_entities_per_thread, c_entities_per_thread);
			}
			for(int i = 0; i < c_num_threads; i++)
			{
				move_system(i * c_entities_per_thread, c_entities_per_thread);
				player_movement_system(i * c_entities_per_thread, c_entities_per_thread);
				physics_movement_system(i * c_entities_per_thread, c_entities_per_thread);
			}
			for(int i = 0; i < c_num_threads; i++)
			{
				bounds_check_system(i * c_entities_per_thread, c_entities_per_thread);
			}
			for(int i = 0; i < c_num_threads; i++)
			{
				projectile_spawner_system(i * c_entities_per_thread, c_entities_per_thread);
			}
			for(int i = 0; i < c_num_threads; i++)
			{
				collision_system(i * c_entities_per_thread, c_entities_per_thread);
			}

			int my_player = find_player_by_id(my_id);
			if(my_player != c_invalid_entity)
			{
				s_player_update_from_client data = zero;
				data.x = e.x[my_player];
				data.y = e.y[my_player];
				send_packet(server, e_packet_player_update, data, 0);
			}
		} break;
	}
}

func void render(u32 program)
{
	switch(state)
	{
		case e_state_main_menu:
		{
			s_v2 pos = g_window.center;
			pos.y -= 100;
			draw_text("Enter character name", pos, 0, v41f(1), e_font_medium, true, (s_transform)zero);
			pos.y += 100;
			if(main_menu.player_name.len > 0)
			{
				draw_text(main_menu.player_name.data, pos, 0, v41f(1), e_font_medium, true, (s_transform)zero);
				pos.y += 100;
			}

			if(main_menu.error_str)
			{
				draw_text(main_menu.error_str, pos, 0, v4(1, 0, 0, 1), e_font_medium, true, (s_transform)zero);
				pos.y += 100;
			}
		} break;

		case e_state_game:
		{
			for(int i = 0; i < c_num_threads; i++)
			{
				draw_system(i * c_entities_per_thread, c_entities_per_thread);
				draw_circle_system(i * c_entities_per_thread, c_entities_per_thread);
			}
		} break;
	}

	{
		// glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearDepth(0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, g_window.width, g_window.height);
		// glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_GREATER);

		int location = glGetUniformLocation(program, "window_size");
		glUniform2fv(location, 1, &g_window.size.x);

		if(transforms.count > 0)
		{
			glEnable(GL_BLEND);
			// glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			glBlendFunc(GL_ONE, GL_ONE);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(*transforms.elements) * transforms.count, transforms.elements);
			glDrawArraysInstanced(GL_TRIANGLES, 0, 6, transforms.count);
			transforms.count = 0;
		}

		for(int font_i = 0; font_i < e_font_count; font_i++)
		{
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			if(text_arr[font_i].count > 0)
			{
				s_font* font = &g_font_arr[font_i];
				glBindTexture(GL_TEXTURE_2D, font->texture.id);
				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(*text_arr[font_i].elements) * text_arr[font_i].count, text_arr[font_i].elements);
				glDrawArraysInstanced(GL_TRIANGLES, 0, 6, text_arr[font_i].count);
				text_arr[font_i].count = 0;
			}
		}
	}
}

func b8 check_for_shader_errors(u32 id, char* out_error)
{
	int compile_success;
	char info_log[1024];
	glGetShaderiv(id, GL_COMPILE_STATUS, &compile_success);

	if(!compile_success)
	{
		glGetShaderInfoLog(id, 1024, null, info_log);
		printf("Failed to compile shader:\n%s", info_log);

		if(out_error)
		{
			strcpy(out_error, info_log);
		}

		return false;
	}
	return true;
}

func void input_system(int start, int count)
{
	b8 go_left = is_key_down(key_a) || is_key_down(key_left);
	b8 go_right = is_key_down(key_d) || is_key_down(key_right);
	b8 go_down = is_key_pressed(key_s) || is_key_pressed(key_down);
	b8 jump = is_key_pressed(key_space) || is_key_pressed(key_w) || is_key_pressed(key_up);
	b8 jump_released = is_key_released(key_space) || is_key_released(key_w) || is_key_released(key_up);

	for(int i = 0; i < count; i++)
	{
		int ii = start + i;
		if(!e.active[ii]) { continue; }
		if(!e.flags[ii][e_entity_flag_input]) { continue; }

		e.dir_x[ii] = 0;
		if(go_right)
		{
			e.dir_x[ii] += 1;
		}
		if(go_left)
		{
			e.dir_x[ii] -= 1;
		}

		if(go_down)
		{
			e.vel_y[ii] = max(e.vel_y[ii], c_fast_fall_speed);
		}

		b8 can_jump = e.jumps_done[ii] < 2;
		if(can_jump && jump)
		{
			float jump_multiplier = e.jumps_done[ii] == 0 ? 1.0f : 0.9f;
			e.vel_y[ii] = c_jump_strength * jump_multiplier;
			e.jumping[ii] = true;
			e.jumps_done[ii] += 1;
		}
		else if(e.jumping[ii] && jump_released && e.vel_y[ii] < 0)
		{
			e.vel_y[ii] *= 0.5f;
		}
	}
}

func void draw_system(int start, int count)
{
	for(int i = 0; i < count; i++)
	{
		int ii = start + i;
		if(!e.active[ii]) { continue; }
		if(!e.flags[ii][e_entity_flag_draw]) { continue; }

		s_v4 color = v41f(1);
		if(e.dead[ii])
		{
			color = v41f(0.25f);
		}
		draw_rect(v2(e.x[ii], e.y[ii]), 0, v2(e.sx[ii], e.sy[ii]), color, (s_transform)zero);

		s_v2 pos = v2(
			e.x[ii], e.y[ii]
		);
		pos.y -= e.sy[ii];

		if(!e.dead[ii])
		{
			if(e.player_id[ii] == my_id)
			{
				draw_text(main_menu.player_name.data, pos, 1, color, e_font_small, true, (s_transform)zero);
			}
			else
			{
				if(e.name[ii].len > 0)
				{
					draw_text(e.name[ii].data, pos, 1, color, e_font_small, true, (s_transform)zero);
				}
			}
		}
	}
}

func void draw_circle_system(int start, int count)
{
	for(int i = 0; i < count; i++)
	{
		int ii = start + i;
		if(!e.active[ii]) { continue; }
		if(!e.flags[ii][e_entity_flag_draw_circle]) { continue; }

		s_v4 light_color = e.color[ii];
		light_color.w *= 0.2f;
		draw_light(v2(e.x[ii], e.y[ii]), 0, e.sx[ii] * 8.0f, light_color, (s_transform)zero);
		draw_circle(v2(e.x[ii], e.y[ii]), 1, e.sx[ii], e.color[ii], (s_transform)zero);
		draw_circle(v2(e.x[ii], e.y[ii]), 2, e.sx[ii] * 0.7f, v41f(1), (s_transform)zero);
	}
}

func void parse_packet(ENetEvent event)
{
	u8* cursor = event.packet->data;
	e_packet packet_id = *(e_packet*)buffer_read(&cursor, sizeof(packet_id));

	switch(packet_id)
	{
		case e_packet_welcome:
		{
			s_welcome_from_server data = *(s_welcome_from_server*)cursor;
			my_id = data.id;
			make_player(data.id, true);
		} break;

		case e_packet_already_connected_player:
		{
			s_already_connected_player_from_server data = *(s_already_connected_player_from_server*)cursor;
			int entity = make_player(data.id, data.dead);
			e.name[entity] = data.name;
		} break;

		case e_packet_another_player_connected:
		{
			s_another_player_connected_from_server data = *(s_another_player_connected_from_server*)cursor;
			make_player(data.id, data.dead);
		} break;

		case e_packet_player_update:
		{
			s_player_update_from_server data = *(s_player_update_from_server*)cursor;

			assert(data.id != my_id);
			int entity = find_player_by_id(data.id);
			if(entity != c_invalid_entity)
			{
				e.x[entity] = data.x;
				e.y[entity] = data.y;
			}

		} break;

		case e_packet_player_disconnected:
		{
			s_player_disconnected_from_server data = *(s_player_disconnected_from_server*)cursor;
			assert(data.id != my_id);
			int entity = find_player_by_id(data.id);
			if(entity != c_invalid_entity)
			{
				e.active[entity] = false;
			}
		} break;

		case e_packet_beat_level:
		{
			s_beat_level_from_server data = *(s_beat_level_from_server*)cursor;
			current_level = data.current_level + 1;
			rng.seed = data.seed;
			reset_level();
			revive_every_player();
			log("Beat level %i", current_level);
		} break;

		case e_packet_reset_level:
		{
			s_reset_level_from_server data = *(s_reset_level_from_server*)cursor;
			current_level = data.current_level;
			log("Reset level %i", current_level + 1);
			rng.seed = data.seed;
			reset_level();
			revive_every_player();
		} break;

		case e_packet_player_got_hit:
		{
			s_player_got_hit_from_server data = *(s_player_got_hit_from_server*)cursor;
			assert(data.id != my_id);
			int entity = find_player_by_id(data.id);
			if(entity != c_invalid_entity)
			{
				log("Player %i with id %u died", entity, data.id);
				e.dead[entity] = true;
			}
		} break;

		case e_packet_player_name:
		{
			s_player_name_from_server data = *(s_player_name_from_server*)cursor;
			assert(data.id != my_id);

			int entity = find_player_by_id(data.id);
			if(entity != c_invalid_entity)
			{
				e.name[entity] = data.name;
				log("Set %u's name to %s", data.id, e.name[entity].data);
			}
		} break;

		invalid_default_case;
	}
}

func void enet_loop(ENetHost* client, int timeout)
{
	ENetEvent event = zero;
	while(enet_host_service(client, &event, timeout) > 0)
	{
		switch(event.type)
		{
			case ENET_EVENT_TYPE_NONE:
			{
			} break;

			case ENET_EVENT_TYPE_CONNECT:
			{
				printf("Client: connected!\n");

				s_player_name_from_client data;
				data.name = main_menu.player_name;
				send_packet(server, e_packet_player_name, data, ENET_PACKET_FLAG_RELIABLE);

			} break;

			case ENET_EVENT_TYPE_DISCONNECT:
			{
				printf("Client: disconnected!\n");
				return;
			} break;

			case ENET_EVENT_TYPE_RECEIVE:
			{
				parse_packet(event);
				enet_packet_destroy(event.packet);
			} break;

			invalid_default_case;
		}
	}
}

func void revive_every_player()
{
	for(int i = 0; i < c_max_entities; i++)
	{
		if(!e.active[i]) { continue; }
		if(!e.player_id[i]) { continue; }
		e.dead[i] = false;
		log("Revived player at index %i with id %u", i, e.player_id[i]);
	}
}

func void collision_system(int start, int count)
{
	for(int i = 0; i < count; i++)
	{
		int ii = start + i;
		if(!e.active[ii]) { continue; }
		if(!e.flags[ii][e_entity_flag_collide]) { continue; }

		for(int j = 0; j < c_max_entities; j++)
		{
			if(!e.active[j]) { continue; }
			if(e.dead[j]) { continue; }
			if(e.type[j] != e_entity_type_player) { continue; }
			if(e.player_id[j] != my_id) { continue; }

			if(
				rect_collides_circle(v2(e.x[j], e.y[j]), v2(e.sx[j], e.sy[j]), v2(e.x[ii], e.y[ii]), e.sx[ii] * 0.48f)
			)
			{
				e.dead[j] = true;
				s_player_got_hit_from_client data = zero;
				send_packet(server, e_packet_player_got_hit, data, ENET_PACKET_FLAG_RELIABLE);
			}
		}
	}
}

func s_font load_font(char* path, float font_size, s_lin_arena* arena)
{
	s_font font = zero;
	font.size = font_size;

	u8* file_data = (u8*)read_file_quick(path, arena);
	assert(file_data);

	stbtt_fontinfo info = zero;
	stbtt_InitFont(&info, file_data, 0);

	stbtt_GetFontVMetrics(&info, &font.ascent, &font.descent, &font.line_gap);

	font.scale = stbtt_ScaleForPixelHeight(&info, font_size);
	#define max_chars 128
	int bitmap_count = 0;
	u8* bitmap_arr[max_chars];
	const int padding = 10;
	int total_width = padding;
	int total_height = 0;
	for(int char_i = 0; char_i < max_chars; char_i++)
	{
		s_glyph glyph = zero;
		u8* bitmap = stbtt_GetCodepointBitmap(&info, 0, font.scale, char_i, &glyph.width, &glyph.height, 0, 0);
		stbtt_GetCodepointBox(&info, char_i, &glyph.x0, &glyph.y0, &glyph.x1, &glyph.y1);
		stbtt_GetGlyphHMetrics(&info, char_i, &glyph.advance_width, null);

		total_width += glyph.width + padding;
		total_height = max(glyph.height + padding * 2, total_height);

		font.glyph_arr[char_i] = glyph;
		bitmap_arr[bitmap_count++] = bitmap;
	}

	// @Fixme(tkap, 23/06/2023): Use arena
	u8* gl_bitmap = calloc(1, sizeof(u8) * 4 * total_width * total_height);

	int current_x = padding;
	for(int char_i = 0; char_i < max_chars; char_i++)
	{
		s_glyph* glyph = &font.glyph_arr[char_i];
		u8* bitmap = bitmap_arr[char_i];
		for(int y = 0; y < glyph->height; y++)
		{
			for(int x = 0; x < glyph->width; x++)
			{
				u8 src_pixel = bitmap[x + y * glyph->width];
				u8* dst_pixel = &gl_bitmap[((current_x + x) + (padding + y) * total_width) * 4];
				dst_pixel[0] = src_pixel;
				dst_pixel[1] = src_pixel;
				dst_pixel[2] = src_pixel;
				dst_pixel[3] = src_pixel;
			}
		}

		glyph->uv_min.x = current_x / (float)total_width;
		glyph->uv_max.x = (current_x + glyph->width) / (float)total_width;

		glyph->uv_min.y = padding / (float)total_height;

		// @Note(tkap, 17/05/2023): For some reason uv_max.y is off by 1 pixel (checked the texture in renderoc), which causes the text to be slightly miss-positioned
		// in the Y axis. "glyph->height - 1" fixes it.
		glyph->uv_max.y = (padding + glyph->height - 1) / (float)total_height;

		// @Note(tkap, 17/05/2023): Otherwise the line above makes the text be cut off at the bottom by 1 pixel...
		glyph->uv_max.y += 0.01f;

		current_x += glyph->width + padding;
	}

	font.texture = load_texture_from_data(gl_bitmap, total_width, total_height, GL_LINEAR);

	for(int bitmap_i = 0; bitmap_i < bitmap_count; bitmap_i++)
	{
		stbtt_FreeBitmap(bitmap_arr[bitmap_i], null);
	}

	free(gl_bitmap);

	#undef max_chars

	return font;
}

func s_texture load_texture_from_data(void* data, int width, int height, u32 filtering)
{
	assert(data);
	u32 id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);

	s_texture texture = zero;
	texture.id = id;
	texture.size = v22i(width, height);
	return texture;
}

func s_v2 get_text_size_with_count(char* text, e_font font_id, int count)
{
	assert(count >= 0);
	if(count <= 0) { return (s_v2)zero; }
	s_font* font = &g_font_arr[font_id];

	s_v2 size = zero;
	size.y = font->size;

	for(int char_i = 0; char_i < count; char_i++)
	{
		char c = text[char_i];
		s_glyph glyph = font->glyph_arr[c];
		if(char_i == count - 1 && c != ' ')
		{
			size.x += glyph.width;
		}
		else
		{
			size.x += glyph.advance_width * font->scale;
		}
	}

	return size;
}

func s_v2 get_text_size(char* text, e_font font_id)
{
	return get_text_size_with_count(text, font_id, (int)strlen(text));
}

func void connect_to_server()
{
	if(enet_initialize() != 0)
	{
		error(false);
	}

	g_client = enet_host_create(
		null /* create a client host */,
		1, /* only allow 1 outgoing connection */
		2, /* allow up 2 channels to be used, 0 and 1 */
		0, /* assume any amount of incoming bandwidth */
		0 /* assume any amount of outgoing bandwidth */
	);

	if(g_client == null)
	{
		error(false);
	}

	ENetAddress address = zero;
	enet_address_set_host(&address, "at-taxation.at.ply.gg");
	// enet_address_set_host(&address, "127.0.0.1");
	address.port = 62555;

	server = enet_host_connect(g_client, &address, 2, 0);
	if(server == null)
	{
		error(false);
	}

	g_connected = true;

}