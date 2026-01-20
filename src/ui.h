#ifndef UI_H
#define UI_H

#include <SDL.h>

void init_ui(SDL_Renderer* renderer);
void clean_ui();
void draw_hud(SDL_Renderer* renderer, int score, int lives, int map_height_px);
void draw_game_over(SDL_Renderer* renderer, int score, int screen_w, int screen_h);

#endif
