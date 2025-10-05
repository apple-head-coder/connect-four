#include "connect-four.h"

// #define DEBUG

#ifdef DEBUG
long long calls = 0;
#endif

int main() {
    grid_t grid = {0};
    player_t winner;

    while (1) {
        print_grid(&grid);
        
        int player_move;
        printf("Enter column number: ");
        scanf("%d", &player_move);
        grid_play(&grid, RED, 7 - player_move);
        winner = grid_get_winner(&grid, YELLOW);
        if (winner == RED) {
            print_grid(&grid);
            printf("YOU WIN\n");
            break;
        }

        int computer_move = grid_best_move(&grid, YELLOW);
        grid_play(&grid, YELLOW, computer_move);
        winner = grid_get_winner(&grid, RED);
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
    switch (grid_get_winner(grid, player)) {
        case RED: return EVAL_MAX;
        case YELLOW: return EVAL_MIN;
        case DRAW: return 0;
    }

    int best_move;
    
    // Pre-increment alpha and beta to work with incoming evals directly
    alpha = inc_mag(alpha);
    beta = inc_mag(beta);

    if (player == RED) {
        for (int move = 0; move < 7; move++) {
            if (alpha >= EVAL_MAX) { break; }  // stop if cannot improve alpha

            if (grid->tops[move] >= 6) { continue; }  // ignore invalid mvoes

            // Try the move & evaluate
            grid_play(grid, player, move);
            int eval = minimax(grid, YELLOW, alpha, beta, NULL);
            grid_unplay(grid, move);

            if (eval >= beta) { return dec_mag(beta); }  // prune if alpha >= beta before updating alpha
            if (eval > alpha) {
                // Take note if best one so far
                alpha = eval;
                best_move = move;
            }
        }

        if (best_move_out) { *best_move_out = best_move; }
        return dec_mag(alpha);  // encourage faster wins and slower losses
    } else {
        // Same thing as yellow but reversed
        for (int move = 0; move < 7; move++) {
            if (beta <= EVAL_MIN) { break; }

            if (grid->tops[move] >= 6) { continue; }

            grid_play(grid, player, move);
            int eval = minimax(grid, RED, alpha, beta, NULL);
            grid_unplay(grid, move);
            
            if (eval <= alpha) { return dec_mag(alpha); }
            if (eval < beta) {
                beta = eval;
                best_move = move;
            }
        }
        
        if (best_move_out) { *best_move_out = best_move; }
        return dec_mag(beta);
    }
}

// Get the current winner, `DRAW`, or `NONE`
player_t grid_get_winner(grid_t *grid, player_t next_player) {
    if (next_player == RED) {
        // Only yellow could have won
        if (has_won(grid->yellow_grid)) { return YELLOW; }

        if ((grid->red_grid | grid->yellow_grid) == BITGRID_FULL) { return DRAW; }

        return NONE;
    } else {
        // Only red could have won
        if (has_won(grid->red_grid)) { return RED; }

        return NONE;
    }
}

// Given a player's bitgrid, determines if the player won
static inline int has_won(bitgrid_t player_grid) {
	bitgrid_t bitmask;

    // Use bit shift & bitwise and to check for 2-in-a-row's and then 4-in-a-row's
	// The extra 0 bit at the end of each row guards against wrap-arounds

	// Horizontal (offset 1)
	bitmask = player_grid & player_grid << 1;
	if (bitmask & bitmask << 2) return 1;
	
	// Vertical (offset 8)
	bitmask = player_grid & player_grid << 8;
	if (bitmask & bitmask << 16) return 1;

	// Diagonal UR (offset 7)
	bitmask = player_grid & player_grid << 7;
	if (bitmask & bitmask << 14) return 1;

	// Diagonal UL (offset 9)
	bitmask = player_grid & player_grid << 9;
	if (bitmask & bitmask << 18) return 1;

	return 0;
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
static inline bitgrid_t row_col_to_bit(int row, int col) {
    return 1ull << row * 8 + col;
}

// Increment the magnitude of a number
static inline int inc_mag(int n) {
    return n ? n > 0 ? n + 1 : n - 1 : n;
}

// Decrement the magnitude of a number
static inline int dec_mag(int n) {
    return n ? n > 0 ? n - 1 : n + 1 : n;
}
