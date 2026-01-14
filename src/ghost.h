#ifndef GHOST_H
#define GHOST_H

#include <SDL.h>
#include "pacman.h"

void init_ghosts(SDL_Renderer* renderer);
void update_ghosts(Pacman* p);
void draw_ghosts(SDL_Renderer* renderer);
void clean_ghosts();
void toggle_ghost_pathfinding();
void reset_ghosts();

#endif