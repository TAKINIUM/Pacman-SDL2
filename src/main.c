#include <stdio.h>
#include <SDL.h>
#include "map.h"
#include "pacman.h"
#include "ghost.h"
#include "audio.h" 
#include "menu.h"
#include "ui.h"
#include "scoreboard.h"

#define MAP_HEIGHT_PX (MAP1_HEIGHT * TILE_SIZE)
#define HUD_HEIGHT 40
#define SCREEN_WIDTH MAP1_WIDTH * TILE_SIZE
#define SCREEN_HEIGHT (MAP_HEIGHT_PX + HUD_HEIGHT)

typedef enum {
    STATE_MENU,
    STATE_GAME,
    STATE_GAMEOVER,
    STATE_SCOREBOARD
} GameState;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int init_sdl(void) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
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
        return 1; 
    }

    init_map(renderer);
    init_pacman(renderer);
    init_ghosts(renderer);
    init_audio(); 
    init_menu(renderer);
    init_ui(renderer);
    init_scoreboard();

    GameState state = STATE_MENU;
    char input_name[12] = "";

    // boucle du jeu
    int is_running = 1;
    SDL_Event event;

    while (is_running) {
        // event souris / clavier
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                is_running = 0;
            }
            
            if (state == STATE_MENU) {
                MenuOption choice = handle_menu_input(&event);
                if (choice != (MenuOption)-1) {
                    switch (choice) {
                        case MENU_OPTION_PLAY:
                            state = STATE_GAME;
                            // Reset full game (lives, score, positions)
                            init_pacman(renderer); 
                            reset_ghosts();
                            break;
                        case MENU_OPTION_SCOREBOARD:
                            state = STATE_SCOREBOARD;
                            break;
                        case MENU_OPTION_QUIT:
                            is_running = 0;
                            break;
                        default:
                            break;
                    }
                }
            } else if (state == STATE_SCOREBOARD) {
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                    state = STATE_MENU;
                }
            } else if (state == STATE_GAMEOVER) {
                if (event.type == SDL_TEXTINPUT) {
                     if (strlen(input_name) < 10) {
                         strcat(input_name, event.text.text);
                     }
                } else if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(input_name) > 0) {
                         input_name[strlen(input_name) - 1] = '\0';
                    } else if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER) {
                         // Default name if empty
                         if (strlen(input_name) == 0) strcpy(input_name, "PLAYER"); 
                         add_score(input_name, get_pacman()->score);
                         SDL_StopTextInput();
                         state = STATE_MENU;
                    }
                }
            } else {
                if (get_pacman()->lives <= 0) {
                     state = STATE_GAMEOVER;
                     input_name[0] = '\0';
                     SDL_StartTextInput();
                }
                
                if (event.type == SDL_KEYDOWN) {
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
                        case SDLK_p: // pathfinding
                            toggle_ghost_pathfinding();
                            break;
                        case SDLK_ESCAPE: 
                            state = STATE_MENU; 
                            break;
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        if (state == STATE_MENU) {
            update_menu();
            draw_menu(renderer);
        } else if (state == STATE_GAMEOVER) {
             const char* game_over_txt = "GAME OVER";
             float go_scale = 2.0f;
             int go_w = get_text_width(game_over_txt, go_scale);
             draw_text(renderer, game_over_txt, (SCREEN_WIDTH - go_w)/2, SCREEN_HEIGHT/2 - 50, go_scale);
             
             char score_str[32];
             sprintf(score_str, "SCORE %d", get_pacman()->score);
             float sc_scale = 1.0f;
             int sc_w = get_text_width(score_str, sc_scale);
             draw_text(renderer, score_str, (SCREEN_WIDTH - sc_w)/2, SCREEN_HEIGHT/2 + 20, sc_scale);

             char name_buf[64];
             sprintf(name_buf, "NAME: %s_", input_name);
             int nm_w = get_text_width(name_buf, 1.0f);
             draw_text(renderer, name_buf, (SCREEN_WIDTH - nm_w)/2, SCREEN_HEIGHT/2 + 60, 1.0f);
             
        } else if (state == STATE_SCOREBOARD) {
            draw_scoreboard(renderer);
        } else {
            update_pacman();
            update_ghosts(get_pacman());
            update_audio();
            
            draw_map(renderer);
            draw_pacman(renderer);
            draw_ghosts(renderer);

            draw_hud(renderer, get_pacman()->score, get_pacman()->lives, MAP_HEIGHT_PX);
        }
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    
    clean_map();
    clean_pacman();
    clean_ghosts();
    clean_audio();
    clean_menu();
    clean_ui();
    close_sdl();
    return 0;
}