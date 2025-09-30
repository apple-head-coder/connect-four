#include "connect-four.h"

int main() {
    grid_t grid = {0};
    player_t winner;

    generate_wins();

    while (1) {
        print_grid(&grid);
        
        int player_move;
        printf("Enter column number: ");
        scanf("%d", &player_move);
        grid_play(&grid, RED, 7 - player_move);
        winner = grid_get_winner(&grid);
        if (winner == RED) {
            print_grid(&grid);
            printf("YOU WIN\n");
            break;
        }

        int computer_move = grid_best_move(&grid, YELLOW);
        grid_play(&grid, YELLOW, computer_move);
        winner = grid_get_winner(&grid);
        if (winner == YELLOW) {
            print_grid(&grid);
            printf("YOU LOSE\n");
            break;
        } else if (winner == DRAW) {
            print_grid(&grid);
            printf("DRAW\n");
            break;
        }
    }

    return 0;
}

int grid_best_move(grid_t *grid, player_t player) {
    for (int i = 0; i < 7; i++) {
        if (grid->tops[i] < 6) { return i; }
    }
}

player_t grid_get_winner(grid_t *grid) {
    for (int i = 0; i < NUM_WINS; i++) {
        if ((grid->red_grid & wins[i]) == wins[i]) { return RED; }
        if ((grid->yellow_grid & wins[i]) == wins[i]) { return YELLOW; }
    }

    if ((grid->red_grid | grid->yellow_grid) == BITGRID_FULL) { return DRAW; }

    return NONE;
}

void grid_play(grid_t *grid, player_t player, int column) {
    bitgrid_t bit = row_col_to_bit(grid->tops[column]++, column);
    if (player == RED) { grid->red_grid |= bit; }
    else { grid->yellow_grid |= bit; }
}

void generate_wins() {
    int index = 0;

    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 4; col++) {
            wins[index++] = row_col_to_bit(row, col + 0) |
                            row_col_to_bit(row, col + 1) |
                            row_col_to_bit(row, col + 2) |
                            row_col_to_bit(row, col + 3) ;
        }
    }

    for (int col = 0; col < 7; col++) {
        for (int row = 0; row < 3; row++) {
            wins[index++] = row_col_to_bit(row + 0, col) |
                            row_col_to_bit(row + 1, col) |
                            row_col_to_bit(row + 2, col) |
                            row_col_to_bit(row + 3, col) ;
        }
    }

    bitgrid_t diag_up_left = 0;
    bitgrid_t diag_up_right = 0;
    for (int i = 0; i < 4; i++) {
        diag_up_left |= 1ull << i * 8;
        diag_up_right |= 1ull << i * 6;
    }

    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 4; col++) {
            wins[index++] = diag_up_left << row * 7 + col;
            wins[index++] = diag_up_right << row * 7 + 6 - col;
        }
    }
}

void print_grid(grid_t *grid) {
    printf("\x1b[H\x1b[J\x1b[34m+---+---+---+---+---+---+---+\n");

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

bitgrid_t row_col_to_bit(int row, int col) {
    return 1ull << row * 7 + col;
}
