#include <stdio.h>

#define NUM_WINS 69
#define BITGRID_FULL 0x3ffffffffffull

typedef unsigned long long bitgrid_t;

typedef enum {
    RED,
    YELLOW,
    DRAW,
    NONE,
} player_t;

typedef struct {
    bitgrid_t red_grid;
    bitgrid_t yellow_grid;
    int tops[7];
} grid_t;

bitgrid_t wins[NUM_WINS];

int grid_best_move(grid_t *grid, player_t player);

player_t grid_get_winner(grid_t *grid);

void grid_play(grid_t *grid, player_t player, int column);

void generate_wins();

void print_grid(grid_t *grid);

bitgrid_t row_col_to_bit(int row, int col);
