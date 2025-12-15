#include <stdio.h>
#include <SDL.h>
#include "map.h"

#define MAP1_WIDTH 28
#define MAP1_HEIGHT 31

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
    textures[1] = load_texture(renderer , "assets/mur.bmp");
    textures[2] = load_texture(renderer , "assets/point.bmp");
    textures[3] = load_texture(renderer , "assets/point.bmp");
}

void clean_map(void) {
    for (int i = 0 ; i < 6 ; i++) {
        if (textures[i] != NULL) {
            SDL_DestroyTexture(textures[i]);
            textures[i] = NULL;
        }
    }
}

void draw_map(SDL_Renderer* renderer) {
    int x , y;
    SDL_Rect destRect;

    destRect.w = TILE_SIZE;
    destRect.h = TILE_SIZE;

    for (y = 0 ; y < MAP1_HEIGHT ; y++) {
        for (x = 0 ; x < MAP1_WIDTH ; x++) {

            destRect.x = x * TILE_SIZE;
            destRect.y = y * TILE_SIZE;

            int type = map1[y][x];

            if (type >= 0 && type < 6 && textures[type] != NULL) {
                SDL_RenderCopy(renderer, textures[type], NULL, &destRect);
            } else if (type == 1) { 
                // Si l'image du mur n'est pas chargée, on met du bleu
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                SDL_RenderFillRect(renderer, &destRect);
            } else if (type == 2) {
                // Si l'image du point n'est pas chargée, on met du jaune
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                SDL_Rect dot = {destRect.x + 10, destRect.y + 10, 4, 4};
                SDL_RenderFillRect(renderer, &dot);
            } else {}
            
            // images = SDL_RenderCopy
        }
    }
}