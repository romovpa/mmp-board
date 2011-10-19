#ifndef __OPTIONS_H
#define __OPTIONS_H

#define INF INT_MAX

// do not write result matrix (used for experiments)
//#define FAKE

// enable computation of various counters
//#define STATISTICS

// maximal number of steps from one position
#define MAX_STEPS 130

#define MAX_SCORE 200
#define WIN_VALUE (INF-MAX_SCORE)

/*** Algorithm-specific options ***/

// maximal searching depth
#define MAX_DEPTH 12

// setting search depth according to number of pieces, step number, etc.
#define DEPTH_HEURISTIC

// time control settings
#define ZEITNOT
#define ZEITNOT_1 8
#define ZEITNOT_2 4

// enable debut mode: increase search depth on the first step
#define DEBUT
#define DEBUT_FULLSTEPS 2

// enable null-window heuristic
#define SEARCH_NULLWIN

// enable step ordering (does search)
#define STEPS_ORDER
#define STEPS_MAX_COST 2

// checking for opponent's pieces parade (requires additional loop)
#define STEPS_ORDER_OP_PARADE

// evaluate: score difference heuristic
#define EVALUATE_SCORE
#define EVALUATE_SCORE_W 1

// evaluate: straightness heuristic
#define EVALUATE_STRAIGHT
#define EVALUATE_STRAIGHT_W 3

#endif // __OPTIONS_H
