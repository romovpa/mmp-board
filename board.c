#include "board.h"

#include <stdlib.h>
#include <time.h>

// Time

int timerem[MAX_PLAYERS];
int timestamp;

// Board Structure

int halfstep;
bool over;
char player;
char matrix[SIDE_LEN][SIDE_LEN];
int score[MAX_PLAYERS];
unsigned char team[MAX_PLAYERS];
unsigned char h_count[MAX_PLAYERS][SIDE_LEN];
unsigned char v_count[MAX_PLAYERS][SIDE_LEN];
char row_pos[MAX_PLAYERS][MAX_PIECES];
char col_pos[MAX_PLAYERS][MAX_PIECES];
bool parade[MAX_PLAYERS];

// Movements

const int MOVE_ROW[MOVE_VAR] = {2, 2, 1, 1, -1, -1, -2, -2};
const int MOVE_COL[MOVE_VAR] = {1, -1, 2, -2, 2, -2, 1, -1};

// Functions

void remove_piece(char player, char row, char col)
{
	h_count[player][row]--;
	v_count[player][col]--;

	char i, last = --team[player];
	for (i = 0; i <= team[player]; ++i)
		if (row_pos[player][i] == row && col_pos[player][i]) {
			row_pos[player][i] = row_pos[player][last];
			col_pos[player][i] = col_pos[player][last];
			break;
		}
}

void add_piece(char player, char row, char col)
{
	h_count[player][row]++;
	v_count[player][col]++;

	char last = team[player]++;
	row_pos[player][last] = row;
	col_pos[player][last] = col;
}

void move_piece(char player, char row, char col, char d_row, char d_col)
{
	h_count[player][row]--;
	v_count[player][col]--;
	h_count[player][d_row]++;
	v_count[player][d_col]++;

	char i;
	for (i = 0; i < team[player]; ++i)
		if (row_pos[player][i] == row && col_pos[player][i]) {
			row_pos[player][i] = d_row;
			col_pos[player][i] = d_col;
			break;
		}
}

void init_board()
{
	char r, c;

	// board initialization
	over = 0;
	for (r = 0; r < SIDE_LEN; ++r) {
		h_count[0][r] = 0;
		v_count[0][r] = 0;
		h_count[1][r] = 0;
		v_count[1][r] = 0;
	}
	team[0] = team[1] = 0;
	for (r = 0; r < SIDE_LEN; ++r)
	for (c = 0; c < SIDE_LEN; ++c)
		if (isalpha(matrix[r][c])) {
			int p = ch2player(matrix[r][c]);
			add_piece(p, r, c);
		}
	parade[0] = parade[1] = 0;
}

char move(struct Step *st)
{
	char target = matrix[st->row1][st->col1];

	if (isdigit(target))
		// catching the number
		score[player] += (target-'0');
	if (isalpha(target)) {
		// eating
		score[player] += 1;
		remove_piece(op(player), st->row1, st->col1);
	}

	// move piece
	matrix[st->row0][st->col0] = '-';
	matrix[st->row1][st->col1] = player2ch(player);
	move_piece(player, st->row0, st->col0, st->row1, st->col1);

	// checking for parade of pieces
	int i;
	for (i = 0; i < SIDE_LEN; ++i) {
		if (h_count[0][i] == team[0] || v_count[0][i] == team[0])
			parade[0] = 1;
		if (h_count[1][i] == team[1] || v_count[1][i] == team[1])
			parade[1] = 1;
	}
	if (parade[0]) score[1] += 3;
	if (parade[1]) score[1] += 3;
	over = parade[0] || parade[1];

	return target;
}

void unmove(struct Step *st, char old)
{
	// move piece back
	matrix[st->row0][st->col0] = player2ch(player);
	matrix[st->row1][st->col1] = old;
	move_piece(player, st->row1, st->col1, st->row0, st->col0);

	// parade of pieces
	if (parade[0]) score[0] -= 3;
	if (parade[1]) score[1] -= 3;
	over = 0;

	if (isdigit(old))
		// return the cathed number
		score[player] -= (old-'0');
	if (isalpha(old)) {
		// return the eated piece
		score[player] -= 1;
		add_piece(op(player), st->row1, st->col1);
	}
}

void read_config(FILE *f)
{
	// reading game status
	char winner, me;
	fscanf(f, "%c %d %c\n", &me, &halfstep, &winner);
	player = ch2player(me);

	// checking for game over
	if (winner != 'U') {
		fprintf(stderr, "game is already over\n");
		exit(1);
	}

	// reading time & score
	double t;
	fscanf(f, "%d %lf\n", &score[0], &t);
	timerem[0] = (int) CLOCKS_PER_SEC * t;
	fscanf(f, "%d %lf\n", &score[1], &t);
	timerem[1] = (int) CLOCKS_PER_SEC * t;

	// reading matrix
	int r, c;
	for (r = 0; r < SIDE_LEN; ++r) {
		for (c = 0; c < SIDE_LEN; ++c)
			matrix[r][c] = (char)fgetc(f);
		do { c = fgetc(f); }
		while (c != '\n');
	}
}

void write_config(FILE *f)
{
	// writing game status
	char winner = 'U';
	if (over) {
		if (score[0] == score[1])
			// draw
			winner = 'D';
		else
			// win
			winner = (score[0] > score[1] ? player2ch(0) : player2ch(1));
	}
	fprintf(f, "%c %d %c\n", player2ch(player), halfstep, winner);

	// writing time & score
	fprintf(f, "%d %lf\n", score[0], (double)timerem[0]/CLOCKS_PER_SEC);
	fprintf(f, "%d %lf\n", score[1], (double)timerem[1]/CLOCKS_PER_SEC);

	// writing matrix
	int r, c;
	for (r = 0; r < SIDE_LEN; ++r) {
		for (c = 0; c < SIDE_LEN; ++c)
			fputc(matrix[r][c], f);
		fputc('\n', f);
	}
}

void start_clock()
{
	timestamp = clock();
}

void stop_clock()
{
	int delta = clock() - timestamp;
	timerem[player] -= delta;
}

