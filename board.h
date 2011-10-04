#ifndef __BOARD_H
#define __BOARD_H

#define MAX_PLAYERS 2
#define SIDE_LEN 8

#define ch2player(c) ((c)-'A')
#define player2ch(p) ((p)+'A')
#define op(p) (1-(p))

#define bool char

// Board Structure
extern int halfstep;
extern bool over;
extern char player;
extern char matrix[SIDE_LEN][SIDE_LEN];
extern int  score[MAX_PLAYERS];
extern int  team[MAX_PLAYERS];
extern int  h_count[MAX_PLAYERS][SIDE_LEN];
extern int  v_count[MAX_PLAYERS][SIDE_LEN];
extern bool parade[MAX_PLAYERS];

struct Step {
	char row0, col0;
	char row1, col1;
	signed char cost;
};

#include <stdio.h>

char move(struct Step *st);
void unmove(struct Step *st, char old);

void read_config(FILE *f);
void write_config(FILE *f);

void start_clock();
void stop_clock();

#endif // __BOARD_H
