#ifndef __BOARD_H
#define __BOARD_H

#define MAX_FULLSTEPS 30
#define MAX_PLAYERS 2
#define MAX_PIECES 8
#define SIDE_LEN 8
#define MOVE_VAR 8

#define ch2player(c) ((c)-'A')
#define player2ch(p) ((p)+'A')
#define op(p) (1-(p))

#define bool char

// Board Structure
extern int halfstep;
extern int fullstep;
extern bool over;
extern char player;
extern char matrix[SIDE_LEN][SIDE_LEN];
extern int  score[MAX_PLAYERS];
extern unsigned char team[MAX_PLAYERS];
extern unsigned char h_count[MAX_PLAYERS][SIDE_LEN];
extern unsigned char v_count[MAX_PLAYERS][SIDE_LEN];
extern char row_pos[MAX_PLAYERS][MAX_PIECES];
extern char col_pos[MAX_PLAYERS][MAX_PIECES];
extern bool parade[MAX_PLAYERS];

// Movements
struct Step {
	char row0, col0;
	char row1, col1;
	signed char cost;
};
extern const int MOVE_ROW[];
extern const int MOVE_COL[];

#include <stdio.h>

char move(struct Step *st);
void unmove(struct Step *st, char old);

void read_config(FILE *f);
void write_config(FILE *f);

void start_clock();
void stop_clock();

#endif // __BOARD_H
