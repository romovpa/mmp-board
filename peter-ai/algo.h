#ifndef __ALGO_H
#define __ALGO_H

#define STATISTICS

#define DEBUG(S) { printf(S); printf("\n"); fflush(stdout); }

#include "board.h"

struct Step find_step();
void show_diagnostics();

#endif // __ALGO_H
