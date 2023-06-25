#pragma once
struct s_game
{
	float time_alive_packet_timer;
};


void update(void);
void parse_packet(ENetEvent event);
void revive_every_player(void);