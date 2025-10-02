#include "connect-four.h"
#include <stdlib.h>

// long long calls = 0;

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
    int best_move;
    minimax(grid, player, EVAL_MIN - 1, EVAL_MAX + 1, &best_move);
    return best_move;
}

int minimax(grid_t *grid, player_t player, int alpha, int beta, int *best_move_out) {
    // if (++calls % 100000000ll == 0ll) { printf("minimax calls: %lld\n", calls); print_grid(grid); }
    switch (grid_get_winner(grid)) {
        case RED: return EVAL_MAX;
        case YELLOW: return EVAL_MIN;
        case DRAW: return 0;
    }

    int best_move;

    if (player == RED) {
        for (int move = 0; move < 7; move++) {
            if (alpha >= EVAL_MAX - 1) { break; }
            if (grid->tops[move] >= 6) { continue; }
            
            grid_play(grid, player, move);
            int eval = minimax(grid, YELLOW, inc_mag(alpha), inc_mag(beta), NULL);
            eval = dec_mag(eval);
            grid_unplay(grid, move);
            
            if (eval >= beta) { return beta; }
            if (eval > alpha) { alpha = eval; best_move = move; }
        }

        if (best_move_out) { *best_move_out = best_move; }
        return alpha;
    } else {
        for (int move = 0; move < 7; move++) {
            if (beta <= EVAL_MIN + 1) { break; }
            if (grid->tops[move] >= 6) { continue; }
            
            grid_play(grid, player, move);
            int eval = minimax(grid, RED, inc_mag(alpha), inc_mag(beta), NULL);
            eval = dec_mag(eval);
            grid_unplay(grid, move);
            
            if (eval <= alpha) { return alpha; }
            if (eval < beta) { beta = eval; best_move = move; }
        }
        
        if (best_move_out) { *best_move_out = best_move; }
        return beta;
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

void grid_unplay(grid_t *grid, int column) {
    bitgrid_t inv_bit = ~row_col_to_bit(--grid->tops[column], column);
    grid->red_grid &= inv_bit;
    grid->yellow_grid &= inv_bit;
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
    printf("\x1b[H\x1b[J");
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

bitgrid_t row_col_to_bit(int row, int col) {
    return 1ull << row * 7 + col;
}

int inc_mag(int n) {
    return n ? n > 0 ? n + 1 : n - 1 : n;
}

int dec_mag(int n) {
    return n ? n > 0 ? n - 1 : n + 1 : n;
}
