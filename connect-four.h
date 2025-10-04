#include <stdio.h>

#define BITGRID_FULL 0x7f7f7f7f7f7full
#define EVAL_MAX 43
#define EVAL_MIN -43

// Represents positions in the game grid with bits
typedef unsigned long long bitgrid_t;

typedef enum {
    RED,
    YELLOW,
    DRAW,
    NONE,
} player_t;

typedef struct {
    bitgrid_t red_grid;     // Red piece positions
    bitgrid_t yellow_grid;  // Yellow piece positions
    int tops[7];            // Bottom-up index of the top of each column
} grid_t;

int grid_best_move(grid_t *grid, player_t player);

int minimax(grid_t *grid, player_t player, int alpha, int beta, int *best_move_out);

player_t grid_get_winner(grid_t *grid);

int has_won(bitgrid_t player_grid);

void grid_play(grid_t *grid, player_t player, int column);

void grid_unplay(grid_t *grid, int column);

void print_grid(grid_t *grid);

static inline bitgrid_t row_col_to_bit(int row, int col);

static inline int inc_mag(int n);

static inline int dec_mag(int n);
