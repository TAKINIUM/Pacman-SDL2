#include "pacman.h"
#include "map.h"
#include <stdio.h>
#include <math.h>

Pacman p;

Pacman* get_pacman() {
    return &p;
}

SDL_Texture* load_pacman_texture(SDL_Renderer* renderer, const char* path) {
    SDL_Surface* surface = SDL_LoadBMP(path);
    if (!surface) {
        printf("Erreur chargement %s : %s\n", path, SDL_GetError());
        return NULL;
    }
    SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0, 0, 0));
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return tex;
}

void reset_pacman() {
    p.x = 1 * TILE_SIZE;
    p.y = 1 * TILE_SIZE;
    p.dir_x = 0;
    p.dir_y = 0;
    p.next_dir_x = 0;
    p.next_dir_y = 0;
    p.speed = 2.0f;
    p.frame = 0;
    // On conserve la direction du sprite par défaut ou on reset
    p.current_sprite_dir = DIR_RIGHT;
}

void init_pacman(SDL_Renderer* renderer) {
    
    reset_pacman();

    p.textures[DIR_RIGHT] = load_pacman_texture(renderer, "assets/PacRight.bmp");
    p.textures[DIR_LEFT]  = load_pacman_texture(renderer, "assets/PacLeft.bmp");
    p.textures[DIR_UP]    = load_pacman_texture(renderer, "assets/PacUp.bmp");
    p.textures[DIR_DOWN]  = load_pacman_texture(renderer, "assets/PacDown.bmp");
}

int can_move(float new_x, float new_y) {
    int left = (int)(new_x) / TILE_SIZE;
    int right = (int)(new_x + TILE_SIZE - 1) / TILE_SIZE;
    int top = (int)(new_y) / TILE_SIZE;
    int bottom = (int)(new_y + TILE_SIZE - 1) / TILE_SIZE;

    if (is_wall(left, top) || is_wall(right, top) || 
        is_wall(left, bottom) || is_wall(right, bottom)) {
        return 0; 
    }
    return 1; 
}

void update_pacman() {
    if (p.next_dir_x != 0 || p.next_dir_y != 0) {
        float test_x = p.x + p.next_dir_x * p.speed;
        float test_y = p.y + p.next_dir_y * p.speed;
        
        if (can_move(test_x, test_y)) {
            // allignement des virages
            if (p.next_dir_x != 0) p.y = (int)(p.y / TILE_SIZE + 0.5) * TILE_SIZE;
            if (p.next_dir_y != 0) p.x = (int)(p.x / TILE_SIZE + 0.5) * TILE_SIZE;
            
            p.dir_x = p.next_dir_x;
            p.dir_y = p.next_dir_y;
            p.next_dir_x = 0;
            p.next_dir_y = 0;
        }
    }

    float new_x = p.x + p.dir_x * p.speed;
    float new_y = p.y + p.dir_y * p.speed;

    int map_limit_x = MAP1_WIDTH * TILE_SIZE;

    if (new_x < -TILE_SIZE) {
        new_x = map_limit_x;
    } else if (new_x > map_limit_x) {
        new_x = -TILE_SIZE;
    }

    if (can_move(new_x, new_y)) {
        p.x = new_x;
        p.y = new_y;
    }

    if (p.dir_x == 1)  p.current_sprite_dir = DIR_RIGHT;
    if (p.dir_x == -1) p.current_sprite_dir = DIR_LEFT;
    if (p.dir_y == -1) p.current_sprite_dir = DIR_UP;
    if (p.dir_y == 1)  p.current_sprite_dir = DIR_DOWN;
    p.frame = (SDL_GetTicks() / 50) % 12; 

    int tile_x = (int)(p.x + TILE_SIZE / 2) / TILE_SIZE;
    int tile_y = (int)(p.y + TILE_SIZE / 2) / TILE_SIZE;

    if (tile_x >= 0 && tile_x < MAP1_WIDTH && tile_y >= 0 && tile_y < MAP1_HEIGHT) {
        if (get_tile(tile_x, tile_y) == 2) {
            set_tile(tile_x, tile_y, 0);
        }
    }
}

void draw_pacman(SDL_Renderer* renderer) {
    SDL_Rect dest = { (int)p.x, (int)p.y, TILE_SIZE, TILE_SIZE };
    
    SDL_Texture* currentTexture = p.textures[p.current_sprite_dir];

    if (currentTexture) {
        SDL_Rect src;
        src.x = p.frame * TILE_SIZE;
        src.y = 0;
        src.w = TILE_SIZE;
        src.h = TILE_SIZE;
        SDL_RenderCopy(renderer, currentTexture, &src, &dest);
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderFillRect(renderer, &dest);
    }
}

void set_pacman_direction(int dx, int dy) {
    p.next_dir_x = dx;
    p.next_dir_y = dy;
}

void clean_pacman() {
    for(int i=0; i<4; i++) {
        if (p.textures[i]) {
            SDL_DestroyTexture(p.textures[i]);
            p.textures[i] = NULL;
        }
    }
}