#ifndef MENU_H
#define MENU_H

#include <SDL.h>

typedef enum {
    MENU_OPTION_PLAY,
    MENU_OPTION_OPTIONS,
    MENU_OPTION_SCOREBOARD,
    MENU_OPTION_QUIT,
    MENU_OPTION_COUNT
} MenuOption;

void init_menu(SDL_Renderer* renderer);
void update_menu();
void draw_menu(SDL_Renderer* renderer);
void clean_menu();
MenuOption handle_menu_input(SDL_Event* event);

// Exported for Game Over screen
void draw_text(SDL_Renderer* renderer, const char* text, int x, int y, float scale);
int get_text_width(const char* text, float scale);

int get_volume();
int get_max_lives();

#endif
