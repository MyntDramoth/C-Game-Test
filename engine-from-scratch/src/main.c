#include <stdio.h>
#include <stdbool.h>
#include <glad/glad.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "engine/global.h"
#include "engine/time.h"
#include "engine/util.h"
#include "engine/config/config.h"
#include "engine/input/input.h"
#include "engine/physics/physics.h"
#include "engine/entity/entity.h"
#include "engine/render/render.h"
#include "engine/animation/animation.h"
#include "engine/audio/audio.h"

static Mix_Music *MUSIC_STAGE_1;
static Mix_Chunk *SOUND_JUMP;

static const f32 SPEED_ENEMY_LARGE = 200;
static const f32 SPEED_ENEMY_SMALL = 4000;
static const f32 HEALTH_ENEMY_LARGE = 7;
static const f32 HEALTH_ENEMY_SMALL = 3;


typedef enum collision_layer{
	COLLOSION_LAYER_PLAYER = 1,
	COLLOSION_LAYER_ENEMY = 1 << 1,
	COLLOSION_LAYER_TERRAIN = 1 << 2,

}Coliision_Layer;

static bool shouldQuit = false;
static vec2 pos;
vec4 player_color = {0,1,1,1};
bool player_is_grounded = false;

static void input_handle(Body* body_player) {
	
	if (global.input.escape == KS_PRESSED || global.input.escape == KS_HELD) {
		shouldQuit = true;
	}

	f32 velx = 0;
	f32 vely = body_player->velocity[1];

	if (global.input.right) {
		velx += 400;
	}
	if (global.input.left) {
		velx -= 400;
	}
	if (global.input.up && player_is_grounded) {
		player_is_grounded = false;
		vely = 2000;
		audio_sound_play(SOUND_JUMP);
	}
	
	body_player->velocity[0] = velx;
	body_player->velocity[1] = vely;
}

void player_on_hit(Body *self, Body *other, Hit hit) {
	if(other->collision_layer == COLLOSION_LAYER_ENEMY) {
		player_color[0] = 1;
		player_color[2] = 0;
	}
}

void player_on_hit_static(Body *self, Static_Body *other, Hit hit) {
	if(hit.normal[1] > 0) {
		player_is_grounded = true;
	}
}

void enemy_small_on_hit_static(Body *self, Static_Body *other, Hit hit) {
	if(hit.normal[0] > 0) {
		self->velocity[0] = SPEED_ENEMY_SMALL;
	}
	if(hit.normal[0] < 0) {
		self->velocity[0] = -SPEED_ENEMY_SMALL;
	}
}

void enemy_large_on_hit_static(Body *self, Static_Body *other, Hit hit) {
	if(hit.normal[0] > 0) {
		self->velocity[0] = SPEED_ENEMY_LARGE;
	}
	if(hit.normal[0] < 0) {
		self->velocity[0] = -SPEED_ENEMY_LARGE;
	}
}

void fire_on_hit(Body *self, Body *other, Hit hit) {
	if(other->collision_layer == COLLOSION_LAYER_ENEMY) {
		for(usize i =0; i<entity_count();++i) {
			Entity *entity = entity_get(i);

			if(entity->body_id = hit.other_id) {
				Body *body = physics_body_get(entity->body_id);
				body->is_active = false;
				entity->is_active = false;
				break;
			}
		}
	}
}

int main(int argc, char* argv[]) {
	
	time_init(60);
	config_init();
	SDL_Window* window = render_init();
	physics_init();
	entity_init();
	animation_init();
	audio_init();

	audio_sound_load(&SOUND_JUMP,"assets/jump.wav");
	audio_music_load(&MUSIC_STAGE_1,"assets/breezys_mega_quest_2_stage_1.mp3");
	audio_music_play(MUSIC_STAGE_1);

	SDL_ShowCursor(false);

	u8 enemy_mask = COLLOSION_LAYER_PLAYER | COLLOSION_LAYER_TERRAIN;
	u8 player_mask = COLLOSION_LAYER_ENEMY | COLLOSION_LAYER_TERRAIN;
	u8 fire_mask = COLLOSION_LAYER_ENEMY | COLLOSION_LAYER_PLAYER;
	
	usize player_id = entity_create((vec2){100,200},(vec2){24,24}, (vec2){0,0}, COLLOSION_LAYER_PLAYER, player_mask,false, player_on_hit, player_on_hit_static);

	i32 window_width,window_height;
	SDL_GetWindowSize(window,&window_width,&window_height);
	f32 width = window_width / render_get_scale();
	f32 height = window_height / render_get_scale();

	u32 static_body_a_id = physics_static_body_create((vec2) { width * 0.5 - 12.5, height -12.5}, (vec2){width-50, 50}, COLLOSION_LAYER_TERRAIN);
	u32 static_body_b_id = physics_static_body_create((vec2) { width - 12.5, height * 0.5 + 12.5 }, (vec2) { 50, height - 50 }, COLLOSION_LAYER_TERRAIN);
	u32 static_body_c_id = physics_static_body_create((vec2) { width * 0.5 + 12.5, 12.5 }, (vec2) { width - 50, 50 }, COLLOSION_LAYER_TERRAIN);
	u32 static_body_d_id = physics_static_body_create((vec2) { 12.5, height *0.5 - 12.5 }, (vec2) { 50, height - 50 }, COLLOSION_LAYER_TERRAIN);
	u32 static_body_e_id = physics_static_body_create((vec2) { width * 0.5 , height *0.5 }, (vec2) {62.5, 62.5 }, COLLOSION_LAYER_TERRAIN);

	usize entity_fire = entity_create((vec2){370,50},(vec2){25,25},(vec2){0},0,fire_mask,true,fire_on_hit,NULL);

	

	//puts("hello world!");

	Sprite_Sheet sprite_sheet_player;
	render_sprite_sheet_init(&sprite_sheet_player,"assets/player.png",24,24);
	usize adef_player_walk_id = animation_def_create(
		&sprite_sheet_player,
		(f32[]){0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1},
		(u8[]){0,0,0,0,0,0,0},
		(u8[]){1,2,3,4,5,6,7},
		7);
	usize adef_player_idle_id = animation_def_create(&sprite_sheet_player,(f32[]){0},(u8[]){0},(u8[]){0},1);
	usize anim_player_walk_id = animation_create(adef_player_walk_id, true);
	usize anim_player_idle_id = animation_create(adef_player_idle_id, false);

	Entity* player = entity_get(player_id);
	
	player->animation_id = anim_player_idle_id;
	f32 spawn_timer = 0;

	while (!shouldQuit) {
		time_update();
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_QUIT:
				
				shouldQuit = true;
				break;

				break;
			default:
				break;
			}
		}
		Entity* player = entity_get(player_id);
		Body* player_body = physics_body_get(player->body_id);
		

		if(player_body->velocity[0] != 0) {
			player->animation_id = anim_player_walk_id;
		} else {
			player->animation_id = anim_player_idle_id;
		}

		Static_Body* static_body_a = physics_static_body_get(static_body_a_id);
		Static_Body* static_body_b = physics_static_body_get(static_body_b_id);
		Static_Body* static_body_c = physics_static_body_get(static_body_c_id);
		Static_Body* static_body_d = physics_static_body_get(static_body_d_id);
		Static_Body* static_body_e = physics_static_body_get(static_body_e_id);

		input_update();
		input_handle(player_body);
		physics_update();
		animation_update(global.time.delta);

		//spawn enemies
		{
			spawn_timer -= global.time.delta;
			if(spawn_timer <= 0) {
				spawn_timer = (f32)((rand()%200)+200)/100.0f;
				spawn_timer *= 0.2;
				
					bool is_flipped = rand()%100>=50;

					f32 spawn_x = is_flipped ? 540 :100;

					usize entity_id = entity_create(
						(vec2){spawn_x,200},
						(vec2){20,20},
						(vec2){0,0},
						COLLOSION_LAYER_ENEMY,
						enemy_mask,
						false,
						NULL,
						enemy_small_on_hit_static
					);
					Entity *entity = entity_get(entity_id);
					Body *body = physics_body_get(entity->body_id);
					body->velocity[0] = is_flipped ? -SPEED_ENEMY_SMALL : SPEED_ENEMY_SMALL;
				
			}

		}


		render_begin();

		for(usize i = 0;i<entity_count();++i) {
			Entity *entity = entity_get(i);
			Body *body = physics_body_get(entity->body_id);
			if(body->is_active) {
				render_aabb((f32*)&body->aabb,TURQUOISE);
			} else {
				render_aabb((f32*)&body->aabb,RED);
			}

		}

		//render_aabb((f32*)&player_body->aabb, player_color);

		render_aabb((f32*)&static_body_a->aabb, WHITE);
		render_aabb((f32*)&static_body_b->aabb, WHITE);
		render_aabb((f32*)&static_body_c->aabb, WHITE);
		render_aabb((f32*)&static_body_d->aabb, WHITE);
		render_aabb((f32*)&static_body_e->aabb, WHITE);


		

		//render animated entites
		for(usize i = 0;i<entity_count();++i) {
			Entity *entity = entity_get(i);
			if(!entity->is_active) {
				continue;
			}
			if(entity->animation_id == (usize)-1) {
				continue;
			}

			Body *body = physics_body_get(entity->body_id);
			Animation *anim = animation_get(entity->animation_id);
			Animation_Def *adef = anim->definition;
			Animation_Frame *aframe = &adef->frames[anim->current_frame_index];

			if(body->velocity[0] <0) {
				anim->is_flipped = true;
			} else if (body->velocity[0] >0) {
				anim->is_flipped = false;
			}

			render_sprite_sheet_frame(adef->sprite_sheet, aframe->row, aframe->column, body->aabb.position, anim->is_flipped);
		}

		render_end(window,sprite_sheet_player.texture_id);
		//player_color[0] = 0;
		//player_color[2] = 1;
		time_update_late();
	}
	return 0;
}
