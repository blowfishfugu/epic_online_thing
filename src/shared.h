
#ifdef m_client
#define handle_instant_movement(entity) handle_instant_movement_(entity)
#define handle_instant_resize(entity) handle_instant_resize_(entity)
#else // m_client
#define handle_instant_movement(entity)
#define handle_instant_resize(entity)
#endif

#define m_speed(val) (1000.0f / val)

enum e_curve_type
{
	e_curve_type_linear,
	e_curve_type_count,
};

struct s_float_curve
{
	e_curve_type type[4];
	float start_seconds[4];
	float end_seconds[4];
	float multiplier[4];
};


enum e_on_spawn
{
	e_on_spawn_invalid,
	e_on_spawn_spiral,
	e_on_spawn_cross,
	e_on_spawn_bottom_diagonal,
};

struct s_projectile_spawn_data
{
	float delay;
	float x[2];
	float y[2];
	float speed[2];
	float size[2];

	// @Note(tkap, 30/06/2023): We store this as "turns", meaning that 1 is equal to "pi * 2", 0.5f is equal to "pi", and so on
	float angle[2];

	float r[2];
	float g[2];
	float b[2];
	float a[2];

	// @Fixme(tkap, 30/06/2023): probably dont want this now
	float speed_multiplier = 1;
	float size_multiplier = 1;

	s_float_curve speed_curve;
	s_float_curve size_curve;
	float spiral_multiplier = 1;
	float spiral_offset;
	s_v4 color_overide = v4(c_max_f32, c_max_f32, c_max_f32, c_max_f32);
	b8 collide_ground_only;
	b8 collide_air_only;

	e_on_spawn on_spawn;
};


enum e_background
{
	e_background_invalid,
	e_background_default,
};

struct s_level
{
	s_v2 spawn_pos;
	b8 infinite_jumps;
	int duration;
	s_sarray<s_projectile_spawn_data, c_max_spawns_per_level> spawn_data;
	e_background background;
};


enum e_packet
{
	e_packet_welcome,
	e_packet_already_connected_player,
	e_packet_another_player_connected,
	e_packet_player_update,
	e_packet_player_disconnected,
	e_packet_player_died,
	e_packet_start_level,
	e_packet_set_level,
	e_packet_beat_level,
	e_packet_reset_level,
	e_packet_player_got_hit,
	e_packet_player_appearance,
	e_packet_cheat_next_level,
	e_packet_cheat_previous_level,
	e_packet_cheat_last_level,
	e_packet_all_levels_beat,
	e_packet_periodic_data,
	e_packet_update_levels,
	e_packet_chat_msg,

	e_packet_connect = 9998,
	e_packet_disconnect = 9999,
};


#pragma pack(push, 1)

struct s_welcome_from_server
{
	u32 id;
	int current_level;
	u32 seed;
	int attempt_count_on_current_level;
	s_level levels[c_max_levels];
};

struct s_welcome_from_client
{
	int unused;
};

struct s_already_connected_player_from_server
{
	u32 id;
	b8 dead;
	s_small_str name;
	s_v4 color;
};

struct s_already_connected_player_from_client
{
	int unused;
};

struct s_another_player_connected_from_server
{
	u32 id;
	b8 dead;
};

struct s_another_player_connected_from_client
{
	int unused;
};

struct s_player_update_from_server
{
	u32 id;
	float x;
	float y;
};

struct s_player_update_from_client
{
	float x;
	float y;
};

struct s_player_disconnected_from_server
{
	u32 id;
};

struct s_player_disconnected_from_client
{
	int unused;
};

struct s_player_died_from_server
{
	int unused;
};

struct s_player_died_from_client
{
	int unused;
};

struct s_start_level_from_server
{
	int unused;
};

struct s_start_level_from_client
{
	int unused;
};

struct s_set_level_from_server
{
	int unused;
};

struct s_set_level_from_client
{
	int unused;
};

struct s_beat_level_from_server
{
	int current_level;
	u32 seed;
};

struct s_beat_level_from_client
{
	int unused;
};

struct s_reset_level_from_server
{
	int current_level;
	u32 seed;
	int attempt_count_on_current_level;
};

struct s_reset_level_from_client
{
	int unused;
};

struct s_player_got_hit_from_server
{
	u32 id;
};

struct s_player_got_hit_from_client
{
	int unused;
};

struct s_player_appearance_from_server
{
	u32 id;
	s_small_str name;
	s_v4 color;
};

struct s_player_appearance_from_client
{
	s_small_str name;
	s_v4 color;
};

struct s_cheat_previous_level_from_server
{
	int current_level;
	u32 seed;
};

struct s_cheat_last_level_from_server
{
	int current_level;
	u32 seed;
};

struct s_periodic_data_from_server
{
	u32 id;
	float time_on_current_level;

	// @Note(tkap, 30/06/2023): These are player specific
	float time_lived;
	float best_time_on_level;
};

struct s_update_levels_from_server
{
	s_level levels[c_max_levels];
};

struct s_chat_msg_from_server
{
	u32 id;
	s_medium_str msg;
};

struct s_chat_msg_from_client
{
	s_medium_str msg;
};


#pragma pack(pop)

enum e_particle_spawner_type
{
	e_particle_spawner_default, //Not sure what to call the random 2 blob spawner thingamajig
	e_particle_spawner_cross,
	e_particle_spawner_x,
	e_particle_spawner_type_count,
};

struct s_particle_spawner
{
	e_particle_spawner_type type;
	float spawn_timer;
	float spawn_delay;
};

enum e_entity_flag
{
	e_entity_flag_move,
	e_entity_flag_player_movement,
	e_entity_flag_physics_movement,
	e_entity_flag_input,
	e_entity_flag_draw,
	e_entity_flag_draw_circle,
	e_entity_flag_gravity,
	e_entity_flag_player_bounds_check,
	e_entity_flag_projectile_bounds_check,
	e_entity_flag_expire,
	e_entity_flag_collide,
	e_entity_flag_collide_ground_only,
	e_entity_flag_collide_air_only,
	e_entity_flag_projectile_spawner,
	e_entity_flag_increase_time_lived,
	e_entity_flag_modify_speed,
	e_entity_flag_modify_size,
	e_entity_flag_count,
};

enum e_entity_type
{
	e_entity_type_player,
	e_entity_type_projectile,
	e_entity_type_count,
};

struct s_entities
{
	int count;
	int next_id;

	b8 active[c_max_entities];
	b8 flags[c_max_entities][e_entity_flag_count];
	b8 jumping[c_max_entities];
	b8 on_ground[c_max_entities];
	b8 dead[c_max_entities];
	b8 drawn_last_render[c_max_entities];
	e_entity_type type[c_max_entities];
	int id[c_max_entities];
	int jumps_done[c_max_entities];
	u32 player_id[c_max_entities];
	float prev_x[c_max_entities];
	float prev_y[c_max_entities];
	float x[c_max_entities];
	float y[c_max_entities];
	float prev_sx[c_max_entities];
	float prev_sy[c_max_entities];
	float base_sx[c_max_entities];
	float base_sy[c_max_entities];
	float modified_sx[c_max_entities];
	float modified_sy[c_max_entities];
	float dir_x[c_max_entities];
	float dir_y[c_max_entities];
	float vel_x[c_max_entities];
	float vel_y[c_max_entities];
	float base_speed[c_max_entities];
	float modified_speed[c_max_entities];
	float time_lived[c_max_entities];
	float duration[c_max_entities];
	float best_time_on_level[c_max_entities];
	s_particle_spawner particle_spawner[c_max_entities];
	s_float_curve speed_curve[c_max_entities];
	s_float_curve size_curve[c_max_entities];
	s_v4 color[c_max_entities];
	s_small_str name[c_max_entities];
};




func int make_entity(void);
func void zero_entity(int index);
func int find_player_by_id(u32 id);
func void gravity_system(int start, int count);
func int make_player(u32 player_id, b8 dead, s_v4 color);
func void init_levels(void);
func void expire_system(int start, int count);
func s_small_str str_to_name(char* str);
func int make_entity(void);
func void init_levels(void);

func void apply_projectile_modifiers(int entity, s_projectile_spawn_data data);
func void set_speed(int entity, float speed);
func void set_size(int entity, float sx, float sy);
func s_projectile_spawn_data make_basic_top_projectile(float speed);
func s_projectile_spawn_data make_basic_side_projectile(float speed, int side);
func s_projectile_spawn_data make_top_diagonal_projectile(float speed, int side);
func s_projectile_spawn_data make_cross_projectile(float speed);
func s_projectile_spawn_data make_ground_shot_projectile(float speed);
func s_projectile_spawn_data make_air_shot_projectile(float speed);
func s_projectile_spawn_data make_bottom_diagonal_projectile(float speed, int side);
func s_projectile_spawn_data make_basic_top_projectile(float speed, int side);


func void on_spawn(int entity, s_projectile_spawn_data data);