#include <stdio.h>
#include <SDL.h>
#include "map.h"

#define SCREEN_WIDTH MAP1_WIDTH * TILE_SIZE
#define SCREEN_HEIGHT MAP1_HEIGHT * TILE_SIZE

SDL_Window* window = NULL; // type de fenetre
SDL_Renderer* renderer = NULL; // surface fenetre

int init_sdl(void) {

    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur d'initialisation de SDL : %s\n", SDL_GetError());
        return 0; // Échec
    }

    // creer fenêtre
    window = SDL_CreateWindow (
        "TOMMAN",
        SDL_WINDOWPOS_CENTERED,        
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    // pas de fenêtre
    if (window == NULL) {
        printf("Erreur de création de la fenêtre : %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }

    renderer = SDL_CreateRenderer (
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    if (renderer == NULL) { // pas de renderer
        printf("Erreur de création du rendu : %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0; // Échec
    }

    return 1; // Succès
}

void close_sdl(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main( int argc, char* args[] ) {

    if (!init_sdl()) {
        return 1; // echec
    }

    init_map(renderer);

    // boucle du jeu
    int is_running = 1;
    SDL_Event event;

    while (is_running) {
        // event souris / clavier
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                is_running = 0;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        draw_map(renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    
    clean_map();
    close_sdl();
    return 0;
}