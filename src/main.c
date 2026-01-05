#include <stdio.h>
#include <SDL.h>
#include "map.h"
#include "pacman.h"

#define SCREEN_WIDTH MAP1_WIDTH * TILE_SIZE
#define SCREEN_HEIGHT MAP1_HEIGHT * TILE_SIZE

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int init_sdl(void) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur d'initialisation de SDL : %s\n", SDL_GetError());
        return 0;
    }

    window = SDL_CreateWindow (
        "TOMMAN",
        SDL_WINDOWPOS_CENTERED,        
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

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

    if (renderer == NULL) {
        printf("Erreur de création du rendu : %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

    return 1;
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
    init_pacman(renderer);

    // boucle du jeu
    int is_running = 1;
    SDL_Event event;

    while (is_running) {
        // event souris / clavier
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                is_running = 0;
            }
            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP: 
                        set_pacman_direction(0, -1); 
                        break;
                    case SDLK_DOWN:  
                        set_pacman_direction(0, 1); 
                        break;
                    case SDLK_LEFT:  
                        set_pacman_direction(-1, 0); 
                        break;
                    case SDLK_RIGHT: 
                        set_pacman_direction(1, 0); 
                        break;
                    case SDLK_ESCAPE: 
                        is_running = 0; 
                        break;
                }
            }
        }

        update_pacman();

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        draw_map(renderer);
        draw_pacman(renderer);
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
        
    }
    
    clean_map();
    clean_pacman();
    close_sdl();
    return 0;
}