#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <SDL.h>

#define MAX_HIGH_SCORES 10

typedef struct {
    char name[12];
    int score;
} ScoreEntry;

void init_scoreboard();
void load_scores();
void save_scores();
void add_score(const char* name, int score);
void draw_scoreboard(SDL_Renderer* renderer);

#endif
