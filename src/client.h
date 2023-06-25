
enum e_state
{
	e_state_main_menu,
	e_state_game,
	e_state_count,
};

enum e_font
{
	e_font_small,
	e_font_medium,
	e_font_big,
	e_font_count,
};

struct s_glyph
{
	int advance_width;
	int width;
	int height;
	int x0;
	int y0;
	int x1;
	int y1;
	s_v2 uv_min;
	s_v2 uv_max;
};

struct s_texture
{
	u32 id;
	s_v2 size;
	s_v2 sub_size;
};

struct s_font
{
	float size;
	float scale;
	int ascent;
	int descent;
	int line_gap;
	s_texture texture;
	s_glyph glyph_arr[1024];
};

struct s_main_menu
{
	const char* error_str;
	s_name player_name;
};

struct s_config
{
	s_name player_name;
	s_name ip;
	int port;
	s_v4 color;
};

struct s_game
{
	s_config config;
	f64 update_timer;
};



void update(s_config config);
void render(float dt);
b8 check_for_shader_errors(u32 id, char* out_error);
void input_system(int start, int count);
void draw_system(int start, int count, float dt);
void parse_packet(ENetEvent event, s_config config);
void enet_loop(ENetHost* client, int timeout, s_config config);
void revive_every_player(void);
void draw_circle_system(int start, int count, float dt);
void collision_system(int start, int count);
s_font load_font(const char* path, float font_size, s_lin_arena* arena);
s_texture load_texture_from_data(void* data, int width, int height, u32 filtering);
s_v2 get_text_size(const char* text, e_font font_id);
s_v2 get_text_size_with_count(char* text, e_font font_id, int count);
void connect_to_server(s_config config);
u32 load_shader(const char* vertex_path, const char* fragment_path);
void handle_instant_movement_(int entity);
s_config make_default_config(s_rng* in_rng);
s_name make_name(const char* str);
void save_config(s_config config);
s_config read_config_or_make_default(s_lin_arena* arena, s_rng* in_rng);
b8 is_key_down(int key);
b8 is_key_up(int key);
b8 is_key_pressed(int key);
b8 is_key_released(int key);
s_char_event get_char_event();
void gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

#ifdef _WIN32
#ifdef m_debug
void hot_reload_shaders(void);
#endif // m_debug
#endif // _WIN32

