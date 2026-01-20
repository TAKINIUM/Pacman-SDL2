#ifndef GHOST_H
#define GHOST_H

#include <SDL.h>
#include "pacman.h"

#define GHOST_STATE_NORMAL 0
#define GHOST_STATE_EATABLE 1
#define GHOST_STATE_EATEN   2

void init_ghosts(SDL_Renderer* renderer);
void update_ghosts(Pacman* p);
void draw_ghosts(SDL_Renderer* renderer);
void clean_ghosts();
void toggle_ghost_pathfinding();
void reset_ghosts();
void set_ghosts_eatable(void);

#endif