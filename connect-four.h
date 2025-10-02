#include <stdio.h>

#define NUM_WINS 69
#define BITGRID_FULL 0x3ffffffffffull
#define EVAL_MAX 43
#define EVAL_MIN -43

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

int minimax(grid_t *grid, player_t player, int alpha, int beta, int *best_move_out);

player_t grid_get_winner(grid_t *grid);

void grid_play(grid_t *grid, player_t player, int column);

void grid_unplay(grid_t *grid, int column);

void generate_wins();

void print_grid(grid_t *grid);

bitgrid_t row_col_to_bit(int row, int col);

int inc_mag(int n);

int dec_mag(int n);
