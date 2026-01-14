#ifndef PACMAN_H
#define PACMAN_H

#include <SDL.h>

#define DIR_RIGHT 0
#define DIR_LEFT  1
#define DIR_UP    2
#define DIR_DOWN  3

typedef struct {
    float x, y;
    int dir_x, dir_y;
    int next_dir_x, next_dir_y;
    float speed;
    
    int frame;
    int current_sprite_dir;
    
    SDL_Texture* textures[4];
} Pacman;

void init_pacman(SDL_Renderer* renderer);
void update_pacman();
void draw_pacman(SDL_Renderer* renderer);
void clean_pacman();
void set_pacman_direction(int dx, int dy);
Pacman* get_pacman();
void reset_pacman();

#endif