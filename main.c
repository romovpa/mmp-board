#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define MATRIX_FILE "matrix.txt"

#define DISABLE_CHECK

#define MAX_DEPTH 20
#define MAX_STEPS 130
#define MAX_PLAYERS 2
#define SIDE_LEN 8

#define ch2player(c) ((c)-'A')
#define player2ch(p) ((p)+'A')
#define op(p) (1-(p))

#define bool char


// Time
int timerem[MAX_PLAYERS];

// Board Structure
int halfstep;
bool over;
char player;
char matrix[SIDE_LEN][SIDE_LEN];
int score[MAX_PLAYERS];
int team[MAX_PLAYERS];
int h_count[MAX_PLAYERS][SIDE_LEN];
int v_count[MAX_PLAYERS][SIDE_LEN];
bool parade[MAX_PLAYERS];

struct Step {
	char row0, col0;
	char row1, col1;
	signed char cost;
} steps[MAX_DEPTH][MAX_STEPS];

char move(struct Step *st)
{
	char target = matrix[st->row1][st->col1];

	if (isdigit(target)) 
		// catching the number
		score[player] += (target-'0');
	if (isalpha(target)) {
		// eating
		score[player] += 1;
		team[op(player)]--;
		h_count[op(player)][st->row1]--;
		v_count[op(player)][st->col1]--;
	}

	// move piece
	matrix[st->row0][st->col0] = '-';
	h_count[player][st->row0]--;
	v_count[player][st->col0]--;
	matrix[st->row1][st->col1] = player2ch(player);
	h_count[player][st->row1]++;
	v_count[player][st->col1]++;

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
	h_count[player][st->row1]--;
	v_count[player][st->col1]--;
	h_count[player][st->row0]++;
	v_count[player][st->col0]++;
	matrix[st->row1][st->col1] = old;

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
		team[op(player)]++;
		h_count[op(player)][st->row1]++;
		v_count[op(player)][st->col1]++;
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
		perror("game is already over");
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

int main(int argc, char *argv[])
{
	FILE *f;

	// read configuration 
	f = fopen(MATRIX_FILE, "rt");
	read_config(f);
	fclose(f);

	// checking configuration
	#ifndef DISABLE_CHECK
	check_config();
	#endif

	// find optimal step
	//int s = find_step();
	//move(&steps[0][s]);

	// write configuration
	f = fopen(MATRIX_FILE, "wt");
	write_config(f);
	fclose(f);

	return 0;
}
