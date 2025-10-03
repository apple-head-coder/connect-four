#include "connect-four.h"

// #define DEBUG

#ifdef DEBUG
long long calls = 0;
#endif

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

// Get the best move in the current position
int grid_best_move(grid_t *grid, player_t player) {
    int best_move;
    // Pass in unreachable alpha and beta
    minimax(grid, player, EVAL_MIN - 1, EVAL_MAX + 1, &best_move);
    return best_move;
}

// Minimax recursive algorithm to find the best move
int minimax(grid_t *grid, player_t player, int alpha, int beta, int *best_move_out) {
    /**
     * Minimax algorithm:
     * There is a maximizer (red) trying to maximize the evaluation
     * There is a minimizer (yellow) trying to minimize the evaluation
     * A red win is an evaluation of EVAL_MAX, a yellow win is EVAL_MIN, and a draw is 0
     * 
     * We try all moves and recursively evaluate each of the resulting positions
     * We pick the best one for the current player (max/min)
     * 
     * alpha is the maximum evaluation the maximizer has reached so far
     * beta is the minimum evaluation the minimizer has reached so far
     * If it is not possible to improve upon the alpha/beta, we can return
     * If alpha >= beta, we can return (this is known as alpha beta pruning)
     * 
     * This algorithm is modified such that the longer it takes to win, the closer to 0 the evaluation will be
     * This encourages faster winning and slower losing
     */

    #ifdef DEBUG
    if (++calls % 100000000ll == 0ll) { printf("minimax calls: %lld\n", calls); print_grid(grid); }
    #endif

    // Check for game over
    switch (grid_get_winner(grid)) {
        case RED: return EVAL_MAX;
        case YELLOW: return EVAL_MIN;
        case DRAW: return 0;
    }

    int best_move;

    if (player == RED) {
        for (int move = 0; move < 7; move++) {
            if (alpha >= EVAL_MAX - 1) { break; }  // stop if cannot improve alpha

            if (grid->tops[move] >= 6) { continue; }  // ignore invalid mvoes

            // Try the move & evaluate
            grid_play(grid, player, move);
            int eval = minimax(grid, YELLOW, inc_mag(alpha), inc_mag(beta), NULL);
            eval = dec_mag(eval);  // encourages faster wins and slower losses
            grid_unplay(grid, move);
            
            if (eval >= beta) { return beta; }  // prune if alpha >= beta before updating alpha
            if (eval > alpha) {
                // Take note if best one so far
                alpha = eval;
                best_move = move;
            }
        }

        if (best_move_out) { *best_move_out = best_move; }
        return alpha;
    } else {
        // Same thing as yellow but reversed
        for (int move = 0; move < 7; move++) {
            if (beta <= EVAL_MIN + 1) { break; }
            if (grid->tops[move] >= 6) { continue; }
            
            grid_play(grid, player, move);
            int eval = minimax(grid, RED, inc_mag(alpha), inc_mag(beta), NULL);
            eval = dec_mag(eval);
            grid_unplay(grid, move);
            
            if (eval <= alpha) { return alpha; }
            if (eval < beta) {
                beta = eval;
                best_move = move;
            }
        }
        
        if (best_move_out) { *best_move_out = best_move; }
        return beta;
    }
}

// Get the current winner, `DRAW`, or `NONE`
player_t grid_get_winner(grid_t *grid) {
    // Check for wins
    for (int i = 0; i < NUM_WINS; i++) {
        if ((grid->red_grid & wins[i]) == wins[i]) { return RED; }
        if ((grid->yellow_grid & wins[i]) == wins[i]) { return YELLOW; }
    }

    // Check for draw (grid full)
    if ((grid->red_grid | grid->yellow_grid) == BITGRID_FULL) { return DRAW; }

    return NONE;
}

// Play a move given a column (right-to-left index)
void grid_play(grid_t *grid, player_t player, int column) {
    bitgrid_t bit = row_col_to_bit(grid->tops[column]++, column);
    if (player == RED) { grid->red_grid |= bit; }
    else { grid->yellow_grid |= bit; }
}

// Unplay a move given a column (right-to-left index)
void grid_unplay(grid_t *grid, int column) {
    bitgrid_t inverted_bit = ~row_col_to_bit(--grid->tops[column], column);
    grid->red_grid &= inverted_bit;
    grid->yellow_grid &= inverted_bit;
}

// Generate all the ways to win, stored in global `wins` variable
void generate_wins() {
    int index = 0;

    // Horizontal
    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 4; col++) {
            wins[index++] = row_col_to_bit(row, col + 0) |
                            row_col_to_bit(row, col + 1) |
                            row_col_to_bit(row, col + 2) |
                            row_col_to_bit(row, col + 3) ;
        }
    }

    // Vertical
    for (int col = 0; col < 7; col++) {
        for (int row = 0; row < 3; row++) {
            wins[index++] = row_col_to_bit(row + 0, col) |
                            row_col_to_bit(row + 1, col) |
                            row_col_to_bit(row + 2, col) |
                            row_col_to_bit(row + 3, col) ;
        }
    }

    // Generate diagonal bitmasks
    bitgrid_t diag_up_left = 0;
    bitgrid_t diag_up_right = 0;
    for (int i = 0; i < 4; i++) {
        diag_up_left |= 1ull << i * 8;
        diag_up_right |= 1ull << i * 6;
    }

    // Diagonal
    // Each direction can start anywhere in a certain 3x4 box
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 4; col++) {
            wins[index++] = diag_up_left << row * 7 + col;
            wins[index++] = diag_up_right << row * 7 + 6 - col;
        }
    }
}

// Clear the terminal and print the game grid with colors
void print_grid(grid_t *grid) {
    #ifndef DEBUG
    printf("\x1b[H\x1b[J");  // clear terminal
    #endif

    printf("\x1b[34m+---+---+---+---+---+---+---+\n");

    // Reverse both for loops as we need to print from top-left
    // but bits start from bottom-right
    for (int row = 5; row >= 0; row--) {
        for (int col = 6; col >= 0; col--) {
            printf("| ");

            bitgrid_t bit = row_col_to_bit(row, col);

            // Print the chip
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

// Returns a bitmask for a given row and col (0, 0 = bottom-right)
bitgrid_t row_col_to_bit(int row, int col) {
    return 1ull << row * 7 + col;
}

// Increment the magnitude of a number
int inc_mag(int n) {
    return n ? n > 0 ? n + 1 : n - 1 : n;
}

// Decrement the magnitude of a number
int dec_mag(int n) {
    return n ? n > 0 ? n - 1 : n + 1 : n;
}
