#include "scoreboard.h"
#include "menu.h"
#include "map.h"
#include <stdio.h>
#include <string.h>

static ScoreEntry high_scores[MAX_HIGH_SCORES];

void init_scoreboard() {
    for (int i = 0; i < MAX_HIGH_SCORES; i++) {
        sprintf(high_scores[i].name, "---");
        high_scores[i].score = 0;
    }
    load_scores();
}

void load_scores() {
    FILE* f = fopen("scores.txt", "r");
    if (!f) return;
    for (int i = 0; i < MAX_HIGH_SCORES; i++) {
        if (fscanf(f, "%11s %d", high_scores[i].name, &high_scores[i].score) != 2) {
            break;
        }
    }
    fclose(f);
}

void save_scores() {
    FILE* f = fopen("scores.txt", "w");
    if (!f) return;
    for (int i = 0; i < MAX_HIGH_SCORES; i++) {
        fprintf(f, "%s %d\n", high_scores[i].name, high_scores[i].score);
    }
    fclose(f);
}

void add_score(const char* name, int score) {
    int pos = -1;
    for (int i = 0; i < MAX_HIGH_SCORES; i++) {
        if (score > high_scores[i].score) {
            pos = i;
            break;
        }
    }

    if (pos != -1) {
        for (int i = MAX_HIGH_SCORES - 1; i > pos; i--) {
            high_scores[i] = high_scores[i - 1];
        }
        strncpy(high_scores[pos].name, name, 11);
        high_scores[pos].name[11] = '\0';
        high_scores[pos].score = score;
        save_scores();
    }
}

void draw_scoreboard(SDL_Renderer* renderer) {
    const char* title = "BEST SCORES";
    int center_x = (MAP1_WIDTH * TILE_SIZE) / 2;
    draw_text(renderer, title, center_x - get_text_width(title, 2.0f)/2, 50, 2.0f);

    int start_y = 150;
    int spacing = 30;

    for (int i = 0; i < MAX_HIGH_SCORES; i++) {
        char buffer[64];
        sprintf(buffer, "%d %-10s %d", i+1, high_scores[i].name, high_scores[i].score);
        
        int text_w = get_text_width(buffer, 1.0f);
        draw_text(renderer, buffer, center_x - text_w/2, start_y + i * spacing, 1.0f);
    }
    
    const char* back = "PRESS ESCAPE";
    draw_text(renderer, back, center_x - get_text_width(back, 1.0f)/2, start_y + MAX_HIGH_SCORES * spacing + 40, 1.0f);
}
