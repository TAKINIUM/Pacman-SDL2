#include "ghost.h"
#include "map.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define GHOST_COUNT 4
#define MAX_PATH_LEN 1024

typedef struct {
    int x, y;
} Point;

typedef struct {
    float x, y;
    int dir_x, dir_y;
    float speed;
    int type;
    SDL_Color color;
    int frame;
    Point path[MAX_PATH_LEN];
    int path_len;
    Point random_target; 
} GhostData;

static GhostData ghosts[GHOST_COUNT];
static SDL_Texture* ghost_textures[4]; 
static int show_path = 0;

static SDL_Texture* load_ghost_texture(SDL_Renderer* renderer, const char* path) {
    SDL_Surface* surface = SDL_LoadBMP(path);
    if (!surface) {
        printf("Erreur chargement ghost %s : %s\n", path, SDL_GetError());
        return NULL;
    }
    SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0, 0, 0));
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return tex;
}

void reset_ghosts() {
    SDL_Color colors[4] = {
        {255, 0, 0, 255},     // Rouge (Blinky)
        {255, 182, 255, 255}, // Rose (Pinky)
        {0, 255, 255, 255},   // Bleu (Inky)
        {255, 182, 85, 255}   // Jaune (Clyde)
    };

    for(int i=0; i<GHOST_COUNT; i++) {
        ghosts[i].x = (12 + i) * TILE_SIZE; 
        ghosts[i].y = 14 * TILE_SIZE;
        ghosts[i].dir_x = 0;
        ghosts[i].dir_y = 0;
        ghosts[i].speed = 2.0f;
        ghosts[i].type = i;
        ghosts[i].color = colors[i];
        ghosts[i].frame = 0;
        ghosts[i].path_len = 0;
        ghosts[i].random_target.x = -1;
        ghosts[i].random_target.y = -1;
    }
}

void init_ghosts(SDL_Renderer* renderer) {
    ghost_textures[DIR_RIGHT] = load_ghost_texture(renderer, "assets/RightPhantom.bmp");
    ghost_textures[DIR_LEFT]  = load_ghost_texture(renderer, "assets/LeftPhantom.bmp");
    ghost_textures[DIR_UP]    = load_ghost_texture(renderer, "assets/TopPhantom.bmp");
    ghost_textures[DIR_DOWN]  = load_ghost_texture(renderer, "assets/BotPhantom.bmp");
    reset_ghosts();
}

typedef struct {
    Point nodes[MAP1_WIDTH * MAP1_HEIGHT];
    int head, tail;
} Queue;

void q_push(Queue* q, Point p) {
    q->nodes[q->tail++] = p;
}
Point q_pop(Queue* q) {
    return q->nodes[q->head++];
}
int q_empty(Queue* q) {
    return q->head == q->tail;
}

void clamp_point(Point* p) {
    if (p->x < 1) p->x = 1;
    if (p->x >= MAP1_WIDTH - 1) p->x = MAP1_WIDTH - 2;
    if (p->y < 1) p->y = 1;
    if (p->y >= MAP1_HEIGHT - 1) p->y = MAP1_HEIGHT - 2;
}

Point find_nearest_valid(Point target) {
    if (!is_wall(target.x, target.y)) return target;
    // Recherche spirale pour trouver une case libre proche
    for (int r = 1; r < 20; r++) { // Rayon augmenté
        for (int dx = -r; dx <= r; dx++) {
            for (int dy = -r; dy <= r; dy++) {
                int nx = target.x + dx;
                int ny = target.y + dy;
                if (nx >= 1 && nx < MAP1_WIDTH-1 && ny >= 1 && ny < MAP1_HEIGHT-1) {
                    if (!is_wall(nx, ny)) return (Point){nx, ny};
                }
            }
        }
    }
    return target;
}

int bfs(Point start, Point end, Point* out_path, int avoid_x, int avoid_y) {
    static int visited[MAP1_HEIGHT][MAP1_WIDTH];
    static Point parent[MAP1_HEIGHT][MAP1_WIDTH];
    
    if (start.x < 0 || start.x >= MAP1_WIDTH || start.y < 0 || start.y >= MAP1_HEIGHT) return 0;
    if (is_wall(end.x, end.y)) return 0;

    memset(visited, 0, sizeof(visited));
    
    Queue q = { .head = 0, .tail = 0 };
    q_push(&q, start);
    visited[start.y][start.x] = 1;
    parent[start.y][start.x] = (Point){-1, -1};

    int found = 0;
    int dirs[4][2] = {{0,-1}, {0,1}, {-1,0}, {1,0}};

    while (!q_empty(&q)) {
        Point curr = q_pop(&q);
        if (curr.x == end.x && curr.y == end.y) {
            found = 1;
            break;
        }

        for (int i=0; i<4; i++) {
            int nx = curr.x + dirs[i][0];
            int ny = curr.y + dirs[i][1];

            if (nx >= 0 && nx < MAP1_WIDTH && ny >= 0 && ny < MAP1_HEIGHT) {
                int blocked = (nx == avoid_x && ny == avoid_y);
                if (!visited[ny][nx] && !is_wall(nx, ny) && !blocked) {
                    visited[ny][nx] = 1;
                    parent[ny][nx] = curr;
                    q_push(&q, (Point){nx, ny});
                }
            }
        }
    }

    if (found) {
        int len = 0;
        Point curr = end;
        Point temp_path[MAX_PATH_LEN];
        while (curr.x != -1) {
            temp_path[len++] = curr;
            curr = parent[curr.y][curr.x];
        }
        for (int i=0; i<len; i++) {
            out_path[i] = temp_path[len - 1 - i];
        }
        return len;
    }
    return 0;
}

void update_ghosts(Pacman* p) {
    Point pac_tile = {(int)(p->x / TILE_SIZE), (int)(p->y / TILE_SIZE)};
    clamp_point(&pac_tile);

    for(int i=0; i<GHOST_COUNT; i++) {
        GhostData* g = &ghosts[i];
        g->frame = (SDL_GetTicks() / 150) % 2; 

        Point ghost_tile = {(int)((g->x + TILE_SIZE/2) / TILE_SIZE), (int)((g->y + TILE_SIZE/2) / TILE_SIZE)};
        
        // Mettre à jour collision (simple reset)
        float dist_sq = (g->x - p->x)*(g->x - p->x) + (g->y - p->y)*(g->y - p->y);
        if (dist_sq < (TILE_SIZE / 2.0f) * (TILE_SIZE / 2.0f)) {
            reset_pacman();
            reset_ghosts();
            return;
        }

        Point target = pac_tile;
        
        if (g->type == 1) { // --- ROSE (Pinky) ORIGINAL PATTERN ---
             // Vise 4 cases devant Pacman
             int offset = 4;
             target.x = pac_tile.x + p->dir_x * offset;
             target.y = pac_tile.y + p->dir_y * offset;
             
             // Reproduction du bug original d'arcade :
             // Si Pacman regarde vers le HAUT, on décale aussi vers la GAUCHE
             if (p->dir_y == -1) { // UP
                 target.x -= offset;
             }
             
             clamp_point(&target);
             target = find_nearest_valid(target);
        } 
        else if (g->type == 3) { // JAUNE - Aléatoire pur
            int reached = (ghost_tile.x == g->random_target.x && ghost_tile.y == g->random_target.y);
            if (g->random_target.x == -1 || reached || is_wall(g->random_target.x, g->random_target.y)) {
                int rx, ry;
                do {
                    rx = rand() % (MAP1_WIDTH - 2) + 1;
                    ry = rand() % (MAP1_HEIGHT - 2) + 1;
                } while(is_wall(rx, ry));
                g->random_target.x = rx;
                g->random_target.y = ry;
            }
            target = g->random_target;
        }

        // --- Déplacement ---
        float cx = ghost_tile.x * TILE_SIZE;
        float cy = ghost_tile.y * TILE_SIZE;
        float dx = fabs(g->x - cx);
        float dy = fabs(g->y - cy);

        // Au centre de la tuile
        if (dx < g->speed && dy < g->speed) {
            g->x = cx; 
            g->y = cy;
            
            if (g->type == 2) { // Bleu : Chemin alternatif
                Point p1[MAX_PATH_LEN];
                int len1 = bfs(ghost_tile, pac_tile, p1, -1, -1);
                int used_alt = 0;
                if (len1 > 1) {
                    Point p2[MAX_PATH_LEN];
                    int len2 = bfs(ghost_tile, pac_tile, p2, p1[1].x, p1[1].y);
                    if (len2 > 1) {
                        Point next2 = p2[1];
                        int ndx = next2.x - ghost_tile.x;
                        int ndy = next2.y - ghost_tile.y;
                        // Avoid direct U-turn check
                         if (!(g->dir_x != 0 && ndx == -g->dir_x) && !(g->dir_y != 0 && ndy == -g->dir_y)) {
                            memcpy(g->path, p2, len2 * sizeof(Point));
                            g->path_len = len2;
                            used_alt = 1;
                         }
                    }
                }
                if (!used_alt) {
                    if (len1 > 0) { memcpy(g->path, p1, len1 * sizeof(Point)); g->path_len = len1; }
                    else g->path_len = 0;
                }
            } else {
                g->path_len = bfs(ghost_tile, target, g->path, -1, -1);
            }
            
            if (g->path_len > 1) {
                Point next = g->path[1];
                g->dir_x = next.x - ghost_tile.x;
                g->dir_y = next.y - ghost_tile.y;
            } else {
                // Random move if stuck
                int blocked = is_wall(ghost_tile.x + g->dir_x, ghost_tile.y + g->dir_y);
                if (blocked || (g->dir_x == 0 && g->dir_y == 0)) {
                    int dirs[4][2] = {{0,-1},{0,1},{1,0},{-1,0}};
                    int start_k = rand() % 4;
                    for(int i=0; i<4; i++) {
                        int k = (start_k + i) % 4;
                        if (!is_wall(ghost_tile.x + dirs[k][0], ghost_tile.y + dirs[k][1])) {
                             g->dir_x = dirs[k][0];
                             g->dir_y = dirs[k][1];
                             break;
                        }
                    }
                }
            }
        }
        
        g->x += g->dir_x * g->speed;
        g->y += g->dir_y * g->speed;
        
        if (g->x < -TILE_SIZE) g->x = MAP1_WIDTH * TILE_SIZE;
        if (g->x > MAP1_WIDTH * TILE_SIZE) g->x = -TILE_SIZE;
    }
}

void draw_ghosts(SDL_Renderer* renderer) {
    for(int i=0; i<GHOST_COUNT; i++) {
        GhostData* g = &ghosts[i];
        
        if (show_path && g->path_len > 1) {
             SDL_SetRenderDrawColor(renderer, g->color.r, g->color.g, g->color.b, 255);
             for (int k=0; k < g->path_len - 1; k++) {
                 SDL_RenderDrawLine(renderer, 
                    g->path[k].x * TILE_SIZE + TILE_SIZE/2, g->path[k].y * TILE_SIZE + TILE_SIZE/2,
                    g->path[k+1].x * TILE_SIZE + TILE_SIZE/2, g->path[k+1].y * TILE_SIZE + TILE_SIZE/2);
             }
             SDL_Rect r = {g->path[g->path_len-1].x * TILE_SIZE + 10, g->path[g->path_len-1].y * TILE_SIZE + 10, 4, 4};
             SDL_RenderFillRect(renderer, &r);
        }

        int tex_idx = DIR_RIGHT;
        if (g->dir_x == -1) tex_idx = DIR_LEFT;
        else if (g->dir_y == -1) tex_idx = DIR_UP;
        else if (g->dir_y == 1) tex_idx = DIR_DOWN;
        
        SDL_Texture* tex = ghost_textures[tex_idx];
        SDL_Rect dest = {(int)g->x, (int)g->y, TILE_SIZE, TILE_SIZE};
        SDL_Rect src = { g->frame * TILE_SIZE, g->type * TILE_SIZE, TILE_SIZE, TILE_SIZE };
        
        if (tex) {
            SDL_RenderCopy(renderer, tex, &src, &dest);
        } else {
            SDL_SetRenderDrawColor(renderer, g->color.r, g->color.g, g->color.b, 255);
            SDL_RenderFillRect(renderer, &dest);
        }
    }
}

void toggle_ghost_pathfinding() {
    show_path = !show_path;
}

void clean_ghosts() {
    for(int i=0; i<4; i++) {
        if (ghost_textures[i]) SDL_DestroyTexture(ghost_textures[i]);
    }
}