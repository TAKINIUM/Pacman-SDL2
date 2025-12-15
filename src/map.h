#ifndef MAP_H

#define MAP_H
#include <SDL.h>

#define TILE_SIZE 24
#define MAP1_WIDTH 28
#define MAP1_HEIGHT 31

void init_map(SDL_Renderer* renderer);
void draw_map(SDL_Renderer* renderer);
void clean_map(void);

#endif