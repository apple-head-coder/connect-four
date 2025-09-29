#include "connect-four.h"

int main() {
    grid_t grid = {0};

    while (1) {
        printf("\x1b[H\x1b[J");
        print_grid(&grid);
        
        int player_move;
        printf("Enter column number: ");
        scanf("%d", &player_move);
        grid_play(&grid, RED, 7 - player_move);
    }

    return 0;
}

void print_grid(grid_t *grid) {
    printf("\x1b[34m+---+---+---+---+---+---+---+\n");

    for (int row = 5; row >= 0; row--) {
        for (int col = 6; col >= 0; col--) {
            printf("| ");

            bitgrid_t bit = row_col_to_bit(row, col);

            if (grid->red_grid & bit) {
                printf("\x1b[31mO");
            } else if (grid->yellow_grid & bit) {
                printf("\x1b[33mO");
            } else {
                printf(" ");
            }

            printf("\x1b[34m ");
        }
        printf("|\n+---+---+---+---+---+---+---+\n");
    }
    printf("\x1b[39m  1   2   3   4   5   6   7\n");
}

void grid_play(grid_t *grid, player_t player, int column) {
    bitgrid_t bit = row_col_to_bit(grid->tops[column]++, column);
    if (player == RED) { grid->red_grid |= bit; }
    else { grid->yellow_grid |= bit; }
}

bitgrid_t row_col_to_bit(int row, int col) {
    return 1ull << row * 7 + col;
}
