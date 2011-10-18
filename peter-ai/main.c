#include <stdio.h>

#include "board.h"
#include "algo.h"

#define MATRIX_FILE "matrix.txt"
#define DISABLE_CHECK

int main(int argc, char *argv[])
{
	FILE *f;
	struct Step st;

	// read configuration 
	f = fopen(MATRIX_FILE, "rt");
	read_config(f);
	fclose(f);

	// checking configuration
	#ifndef DISABLE_CHECK
	check_config();
	#endif

	// find optimal step
	start_clock();
	st = find_step();
	stop_clock();

	// make move
	move(&st);

	// write configuration
	f = fopen(MATRIX_FILE, "wt");
	write_config(f);
	fclose(f);

	// print statistics
	#ifdef STATISTICS
	print_statistics();
	#endif

	return 0;
}
