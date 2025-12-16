#include <stdio.h>
#include <SDL.h>
#include "map.h"

#define MAP1_WIDTH 28
#define MAP1_HEIGHT 31

#define SPRITE_SHEET_COLS 12
#define SPRITE_SHEET_ROWS 4

static SDL_Texture* textures[6] = {NULL};

const char* tileTypes[] = {
    "Chemin",
    "Mur",
    "Point",
    "Power",
    "Fantome",
    "Fruit"
};

int map1[MAP1_HEIGHT][MAP1_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
    {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
    {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,2,1},
    {1,2,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,2,2,1},
    {1,1,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,1,1},
    {0,0,0,0,0,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,0,0,0,0,0},
    {0,0,0,0,0,1,2,1,1,2,2,2,2,2,2,2,2,2,2,1,1,2,1,0,0,0,0,0},
    {0,0,0,0,0,1,2,1,1,2,1,1,1,0,0,1,1,1,2,1,1,2,1,0,0,0,0,0},
    {1,1,1,1,1,1,2,1,1,2,1,0,0,0,0,0,0,1,2,1,1,2,1,1,1,1,1,1},
    {0,0,0,0,0,0,2,2,2,2,1,0,0,0,0,0,0,1,2,2,2,2,0,0,0,0,0,0},
    {1,1,1,1,1,1,2,1,1,2,1,0,0,0,0,0,0,1,2,1,1,2,1,1,1,1,1,1},
    {0,0,0,0,0,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,0,0,0,0,0},
    {0,0,0,0,0,1,2,1,1,2,2,2,2,2,2,2,2,2,2,1,1,2,1,0,0,0,0,0},
    {0,0,0,0,0,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,0,0,0,0,0},
    {1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
    {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
    {1,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,1},
    {1,1,1,2,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,2,1,1,1},
    {1,1,1,2,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,2,1,1,1},
    {1,2,2,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,1,1,2,1},
    {1,2,1,1,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};

SDL_Texture* load_texture(SDL_Renderer* renderer , const char* path) {
    SDL_Surface* Surface = SDL_LoadBMP(path);
    if (!Surface) {
        printf("Erreur chargement image %s : %s\n", path, SDL_GetError());
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, Surface);
    SDL_FreeSurface(Surface);
    return texture;
}

void init_map(SDL_Renderer* renderer) {
    textures[1] = load_texture(renderer , "assets/Walls.bmp");
    textures[2] = load_texture(renderer , "assets/point.bmp");
    // textures[3] = load_texture(renderer , "assets/point.bmp");
}

void clean_map(void) {
    for (int i = 0 ; i < 6 ; i++) {
        if (textures[i] != NULL) {
            SDL_DestroyTexture(textures[i]);
            textures[i] = NULL;
        }
    }
}

int is_wall(int x , int y) {
    if (x < 0 || x >= MAP1_WIDTH || y < 0 || y >= MAP1_HEIGHT) return 0;
    return map1[y][x] == 1;
}

int get_wall_sprite(int x , int y) {

    int n = is_wall(x , y - 1); // nord
    int s = is_wall(x , y + 1); // sud
    int e = is_wall(x + 1 , y); // est
    int w = is_wall(x - 1 , y); // ouest
    

    int ne = is_wall(x + 1 , y - 1); // nord est
    int nw = is_wall(x - 1 , y - 1); // nord ouest
    int se = is_wall(x + 1 , y + 1); // sud est
    int sw = is_wall(x - 1 , y + 1); // sud ouest

    if (!n && !s && !e && !w) return 0;
    if (!n && !s && e && w) return 1;
    if (n && s && !e && !w) return 2;

    if (n && s && e && w) {
        if (ne && nw && se && sw) return 46;
        else if (ne && !nw && se && sw) return 40;
        else if (ne && nw && se && !sw) return 41;
        else if (ne && nw && !se && sw) return 42;
        else if (!ne && nw && se && sw) return 43;
        else return 3;
    }

    if (!n && s && !e && !w) return 4;
    if (!n && !s && e && !w) return 5;
    if (n && !s && !e && !w) return 6;
    if (!n && !s && !e && w) return 7;

    if (!n && s && e && !w) {
        if (se) return 16;
        else return 8;
    }
    if (n && !s && e && !w) {
        if (ne) return 17;
        return 9;
    }
    if (n && !s && !e && w) {
        if (nw) return 18;
        return 10;
    }
    if (!n && s && !e && w) {
        if (sw) return 19;
        return 11;
    }

    if (!n && s && e && w) {
        if (sw && se) return 20;
        else if (!sw && se) return 24;
        else if (sw && !se) return 28;
        else return 12;
    }
    if (n && s && e && !w) {
        if (ne && se) return 21;
        else if (ne && !se) return 25;
        else if (!ne && se) return 29;
        else return 13;
    }
    if (n && !s && e && w) {
        if (ne && nw) return 22;
        else if (!ne && nw) return 26;
        else if (ne && !nw) return 30;
        else return 14;
    }
    if (n && s && !e && w) {
        if (nw && sw) return 23;
        else if (!nw && sw) return 27;
        else if (nw && !sw) return 31;
        else return 15;
    }

    return 0;
}

void draw_map(SDL_Renderer* renderer) {
    int x , y;
    SDL_Rect destRect;
    SDL_Rect srcRect;

    destRect.w = TILE_SIZE;
    destRect.h = TILE_SIZE;

    srcRect.w = TILE_SIZE;
    srcRect.h = TILE_SIZE;


    for (y = 0 ; y < MAP1_HEIGHT ; y++) {
        for (x = 0 ; x < MAP1_WIDTH ; x++) {

            destRect.x = x * TILE_SIZE;
            destRect.y = y * TILE_SIZE;

            int type = map1[y][x];

            if (type == 1 && textures[1] != NULL) {
                int spriteIndex = get_wall_sprite(x , y);
                srcRect.x = (spriteIndex / SPRITE_SHEET_ROWS) * TILE_SIZE;
                srcRect.y = (spriteIndex % SPRITE_SHEET_ROWS) * TILE_SIZE;
                SDL_RenderCopy(renderer , textures[1] , &srcRect , &destRect);
            } else if (type >= 0 && type < 6 && textures[type] != NULL) {
                SDL_RenderCopy(renderer, textures[type], NULL, &destRect);
            } else if (type == 1) { 
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                SDL_RenderFillRect(renderer, &destRect);
            } else if (type == 2) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                SDL_Rect dot = {destRect.x + 10, destRect.y + 10, 4, 4};
                SDL_RenderFillRect(renderer, &dot);
            } else {}
        }
    }
}