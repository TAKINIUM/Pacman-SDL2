#include "menu.h"
#include <SDL.h>
#include <stdio.h>
#include "map.h"
#include <ctype.h>
#include "audio.h"

static SDL_Texture* alphabet_texture = NULL;
static SDL_Texture* numbers_texture = NULL;
static int numbers_w = 0;
static int numbers_h = 0;
static int current_selection = 0;

static int max_lives = 3;
static int volume = 64;
static int in_options = 0;

int get_volume() { return volume; }
int get_max_lives() { return max_lives; }

#define CHAR_WIDTH 24
#define CHAR_HEIGHT 24
#define CHAR_COLS 6

void init_menu(SDL_Renderer* renderer) {
    SDL_Surface* surface = SDL_LoadBMP("assets/Alphabet.bmp");
    if (!surface) {
        printf("Erreur chargement assets/Alphabet.bmp : %s\n", SDL_GetError());
        return;
    }
    SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0, 0, 0));
    alphabet_texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    SDL_Surface* surf_num = SDL_LoadBMP("assets/Chiffres.bmp");
    if (surf_num) {
         SDL_SetColorKey(surf_num, SDL_TRUE, SDL_MapRGB(surf_num->format, 0, 0, 0));
         numbers_texture = SDL_CreateTextureFromSurface(renderer, surf_num);
         if (numbers_texture) {
             SDL_QueryTexture(numbers_texture, NULL, NULL, &numbers_w, &numbers_h);
         }
         SDL_FreeSurface(surf_num);
    } else {
        printf("Erreur loading assets/Chiffres.bmp\n");
    }

    current_selection = 0;
}

void clean_menu() {
    if (alphabet_texture) {
        SDL_DestroyTexture(alphabet_texture);
        alphabet_texture = NULL;
    }
    if (numbers_texture) {
        SDL_DestroyTexture(numbers_texture);
        numbers_texture = NULL;
    }
}

void draw_char(SDL_Renderer* renderer, char c, int x, int y, float scale) {
    c = toupper(c);
    
    SDL_Rect src; 
    SDL_Rect dest = { x, y, (int)(CHAR_WIDTH * scale), (int)(CHAR_HEIGHT * scale) };
    
    if (c >= 'A' && c <= 'Z') {
        if (!alphabet_texture) return;
        int index = c - 'A';
        int col = index % CHAR_COLS;
        int row = index / CHAR_COLS;
        src.x = col * CHAR_WIDTH; src.y = row * CHAR_HEIGHT; 
        src.w = CHAR_WIDTH; src.h = CHAR_HEIGHT;
        SDL_RenderCopy(renderer, alphabet_texture, &src, &dest);
    } else if (c >= '0' && c <= '9') {
        if (!numbers_texture) return;
        int val = c - '0';
        
        // 2 Columns, 5 Rows layout confirmed by user
        // Col 0: 0-4
        // Col 1: 5-9
        int col = (val >= 5) ? 1 : 0;
        int row = val % 5;
        
        src.x = col * CHAR_WIDTH; 
        src.y = row * CHAR_HEIGHT;
        
        src.w = CHAR_WIDTH; src.h = CHAR_HEIGHT;
        SDL_RenderCopy(renderer, numbers_texture, &src, &dest);
    }
}

void draw_text(SDL_Renderer* renderer, const char* text, int x, int y, float scale) {
    int cur_x = x;
    while (*text) {
        if (*text == ' ') {
            cur_x += (int)(CHAR_WIDTH * scale);
        } else {
            draw_char(renderer, *text, cur_x, y, scale);
            cur_x += (int)(CHAR_WIDTH * scale);
        }
        text++;
    }
}

int get_text_width(const char* text, float scale) {
    if (!text) return 0;
    int width = 0;
    while (*text) {
        width += (int)(CHAR_WIDTH * scale);
        text++;
    }
    return width;
}

void draw_menu(SDL_Renderer* renderer) {
    if (!alphabet_texture) return;

    // Title centered
    const char* title = "PACMAN";
    int title_len = 6;
    float title_scale = 3.0f;
    int title_width = title_len * (int)(CHAR_WIDTH * title_scale);
    int center_x = (MAP1_WIDTH * TILE_SIZE) / 2;
    
    draw_text(renderer, title, center_x - title_width / 2, 100, title_scale); 

    // Options
    const char* options[] = { "JOUER", "OPTION", "SCOREBOARD", "QUITTER" };
    int start_y = 300;
    int spacing = 50;

    if (in_options) {
        // Draw Options Menu
        draw_text(renderer, "OPTIONS", center_x - get_text_width("OPTIONS", 1.5f) / 2, 80, 1.5f);

        // VOLUME
        char vol_txt[] = "VOLUME";
        float scale = (current_selection == 0) ? 1.5f : 1.0f;
        int t_w = get_text_width(vol_txt, scale);
        draw_text(renderer, vol_txt, center_x - t_w / 2, 300, scale);
        
        // Draw bar
        int bar_w = 200;
        int bar_h = 20;
        int bar_x = center_x - bar_w / 2;
        int bar_y = 330;
        
        SDL_Rect outline = { bar_x, bar_y, bar_w, bar_h };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &outline);
        
        int fill_w = (int)((float)volume / 128.0f * bar_w);
        SDL_Rect fill = { bar_x + 2, bar_y + 2, fill_w - 4, bar_h - 4 };
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow
        if (fill_w > 4) SDL_RenderFillRect(renderer, &fill);

        
        // VIES
        char life_buf[32];
        sprintf(life_buf, "VIES %d", max_lives);
        scale = (current_selection == 1) ? 1.5f : 1.0f;
        int l_w = get_text_width(life_buf, scale);
        draw_text(renderer, life_buf, center_x - l_w / 2, 380, scale);
        
        // RETOUR
        scale = (current_selection == 2) ? 1.5f : 1.0f;
        const char* back = "RETOUR";
        int b_w = get_text_width(back, scale);
        draw_text(renderer, back, center_x - b_w / 2, 420, scale);

    } else {
        // Main Menu
        draw_text(renderer, title, center_x - title_width / 2, 100, title_scale); 

        for (int i = 0; i < MENU_OPTION_COUNT; i++) {
            float scale = (i == current_selection) ? 1.5f : 1.0f;
            
            // Centering the option text
            int text_width = 0;
            const char *p = options[i];
            while(*p++) text_width += (int)(CHAR_WIDTH * scale);

            draw_text(renderer, options[i], center_x - text_width / 2, start_y + i * spacing, scale);
        }
    }
}

MenuOption handle_menu_input(SDL_Event* event) {
    if (event->type == SDL_KEYDOWN) {
        if (in_options) {
            switch (event->key.keysym.sym) {
                case SDLK_UP:
                    current_selection--;
                    if (current_selection < 0) current_selection = 2;
                    break;
                case SDLK_DOWN:
                    current_selection++;
                    if (current_selection > 2) current_selection = 0;
                    break;
                case SDLK_LEFT:
                    if (current_selection == 0) { // Volume
                         volume -= 8;
                         if (volume < 0) volume = 0;
                         set_audio_volume(volume);
                    } else if (current_selection == 1) { // Lives
                         max_lives--;
                         if (max_lives < 1) max_lives = 1;
                    }
                    break;
                case SDLK_RIGHT:
                    if (current_selection == 0) { // Volume
                         volume += 8;
                         if (volume > 128) volume = 128;
                         set_audio_volume(volume);
                    } else if (current_selection == 1) { // Lives
                         max_lives++;
                         if (max_lives > 9) max_lives = 9;
                    }
                    break;
                case SDLK_RETURN:
                case SDLK_KP_ENTER:
                case SDLK_ESCAPE:
                    if (current_selection == 2 || event->key.keysym.sym == SDLK_ESCAPE) { // RETOUR
                         in_options = 0;
                         current_selection = MENU_OPTION_OPTIONS;
                    }
                    break;
            }
        } else {
            switch (event->key.keysym.sym) {
                case SDLK_UP:
                    current_selection--;
                    if (current_selection < 0) current_selection = MENU_OPTION_COUNT - 1;
                    break;
                case SDLK_DOWN:
                    current_selection++;
                    if (current_selection >= MENU_OPTION_COUNT) current_selection = 0;
                    break;
                case SDLK_RETURN:
                case SDLK_KP_ENTER:
                    if (current_selection == MENU_OPTION_OPTIONS) {
                        in_options = 1;
                        current_selection = 0;
                        return (MenuOption)-1; // Don't return OPTION action, handle internally
                    }
                    return (MenuOption)current_selection;
            }
        }
    }
    return (MenuOption)-1;
}

void update_menu() {
    
}
