#include <stdio.h>

typedef unsigned long long bitgrid_t;

typedef enum {
    RED,
    YELLOW,
} player_t;

typedef struct {
    bitgrid_t red_grid;
    bitgrid_t yellow_grid;
    int tops[7];
} grid_t;

void print_grid(grid_t *grid);

void grid_play(grid_t *grid, player_t player, int column);

bitgrid_t row_col_to_bit(int row, int col);
