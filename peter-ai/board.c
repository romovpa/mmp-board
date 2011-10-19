#include "board.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// Time

int timerem[MAX_PLAYERS];
int timestamp;

// Board Structure

int halfstep;
int fullstep;
char player;
char matrix[SIDE_LEN][SIDE_LEN];
int score[MAX_PLAYERS];

unsigned char h_count[MAX_PLAYERS][SIDE_LEN];
unsigned char v_count[MAX_PLAYERS][SIDE_LEN];
unsigned char team[MAX_PLAYERS];
char row_pos[MAX_PLAYERS][MAX_PIECES];
char col_pos[MAX_PLAYERS][MAX_PIECES];

bool over;
bool parade[MAX_PLAYERS];

// Movements

const int MOVE_ROW[MOVE_VAR] = {2, 2, 1, 1, -1, -1, -2, -2};
const int MOVE_COL[MOVE_VAR] = {1, -1, 2, -2, 2, -2, 1, -1};

// Functions

void print_all()
{
	printf("halfstep: %d\n", halfstep);
	printf("over: %d\n", over);
	printf("player: %d\n", player);
	printf("score: %d %d\n", score[0], score[1]);
	printf("team: %d %d\n", team[0], team[1]);
	printf("parade: %d %d\n", parade[0], parade[1]);
	printf("~~~~~~~~~~~~~~~~\n");
	int r, c, pl;
	for (r = 0; r < SIDE_LEN; ++r) {
		for (c = 0; c < SIDE_LEN; ++c)
			printf("%c", matrix[r][c]);
		printf("\n");
	}
	for (pl = 0; pl < MAX_PLAYERS; ++pl) {
		printf("~~~~~~~~~~~~~~~~\n");
		printf("Player %d\n", pl);
		printf("score: %d\nteam: %d\nparade: %d\n", 
			score[pl], team[pl], parade[pl]);

		printf("h_count: ");
		for (r = 0; r < SIDE_LEN; ++r)
			printf("%d ", (int)h_count[pl][r]);
		printf("\n");

		printf("v_count: ");
		for (r = 0; r < SIDE_LEN; ++r)
			printf("%d ", (int)v_count[pl][r]);
		printf("\n");

		printf("row_pos: ");
		for (r = 0; r < team[pl]; ++r)
			printf("%d ", (int)row_pos[pl][r]);
		printf("\n");

		printf("col_pos: ");
		for (r = 0; r < team[pl]; ++r)
			printf("%d ", (int)col_pos[pl][r]);
		printf("\n");
	}
	printf("\n\n");
}

void add_piece(char player, char row, char col)
{
	h_count[player][row]++;
	v_count[player][col]++;

	char last = team[player]++;
	row_pos[player][last] = row;
	col_pos[player][last] = col;
}

void remove_piece(char player, char row, char col)
{
	h_count[player][row]--;
	v_count[player][col]--;

	char i, last = --team[player];
	for (i = 0; i <= team[player]; ++i)
		if (row_pos[player][i] == row && col_pos[player][i] == col) {
			row_pos[player][i] = row_pos[player][last];
			col_pos[player][i] = col_pos[player][last];
			return;
		}
	fprintf(stderr, "ACHTUNG remove");
	exit(3);
}

void move_piece(char player, char row, char col, char d_row, char d_col)
{
	h_count[player][row]--;
	v_count[player][col]--;
	h_count[player][d_row]++;
	v_count[player][d_col]++;

	char i;
	for (i = 0; i < team[player]; ++i)
		if (row_pos[player][i] == row && col_pos[player][i] == col) {
			row_pos[player][i] = d_row;
			col_pos[player][i] = d_col;
			return;
		}
	fprintf(stderr, "ACHTUNG move");
	exit(3);
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
			int pl = ch2player(matrix[r][c]);
			add_piece(pl, r, c);
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
	if (parade[0]) score[0] += 3;
	if (parade[1]) score[1] += 3;
	over = parade[0] || parade[1];

	// turning halfstep
	++halfstep;
	player = op(player);
	if (!player)
		++fullstep;

	return target;
}

void unmove(struct Step *st, char old)
{
	--halfstep;
	if (!player)
		--fullstep;
	player = op(player);

	// move piece back
	matrix[st->row0][st->col0] = player2ch(player);
	matrix[st->row1][st->col1] = old;
	move_piece(player, st->row1, st->col1, st->row0, st->col0);

	// parade of pieces
	if (parade[0]) score[0] -= 3;
	if (parade[1]) score[1] -= 3;
	parade[0] = parade[1] = 0;
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
	fullstep = (halfstep+1)/2;
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
	// TODO: improve robustness
	int r, c;
	char line[12];
	for (r = 0; r < SIDE_LEN; ++r) {
		fgets(line, 12*sizeof(char), f);
		for (c = 0; c < SIDE_LEN; ++c)
			matrix[r][c] = line[c];
	}

	// initialization
	init_board();
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
	else if (fullstep > MAX_FULLSTEPS) {
		// draw
		winner = 'D';
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

float remains_sec()
{
	return (float)timerem[player]/CLOCKS_PER_SEC;
}

