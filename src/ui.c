#include "ui.h"
#include <stdio.h>
#include "map.h"

#define NUM_WIDTH 24
#define NUM_HEIGHT 24

static SDL_Texture* numbers_texture = NULL;

void init_ui(SDL_Renderer* renderer) {
    SDL_Surface* surface = SDL_LoadBMP("assets/Chiffres.bmp");
    if (!surface) {
        printf("Erreur chargement UI assets/Chiffres.bmp : %s\n", SDL_GetError());
        return;
    }
    SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0, 0, 0));
    numbers_texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}

void clean_ui() {
    if (numbers_texture) {
        SDL_DestroyTexture(numbers_texture);
        numbers_texture = NULL;
    }
}

void draw_number(SDL_Renderer* renderer, int digit, int x, int y) {
    if (!numbers_texture) return;
    if (digit < 0 || digit > 9) return;

    SDL_Rect src;
    src.w = NUM_WIDTH;
    src.h = NUM_HEIGHT;
    
    if (digit < 5) {
        src.x = 0;
        src.y = digit * NUM_HEIGHT;
    } else {
        src.x = NUM_WIDTH;
        src.y = (digit - 5) * NUM_HEIGHT;
    }

    SDL_Rect dest = { x, y, NUM_WIDTH, NUM_HEIGHT };
    SDL_RenderCopy(renderer, numbers_texture, &src, &dest);
}

void draw_score_value(SDL_Renderer* renderer, int value, int x, int y) {
    char buf[32];
    sprintf(buf, "%d", value);
    
    int cx = x;
    for(int i=0; buf[i]; i++) {
        if (buf[i] >= '0' && buf[i] <= '9') {
            draw_number(renderer, buf[i] - '0', cx, y);
            cx += NUM_WIDTH;
        }
    }
}

void draw_hud(SDL_Renderer* renderer, int score, int lives, int map_height_px) {
    int y = map_height_px + 10;
    int x = 10;
    draw_number(renderer, lives, x, y);
    draw_score_value(renderer, score, MAP1_WIDTH * TILE_SIZE - 150, y);
}

void draw_game_over(SDL_Renderer* renderer, int score, int screen_w, int screen_h) {

}
