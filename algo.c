#include "algo.h"
#include "board.h"

#include <limits.h>
#include <stdlib.h>

#define INF INT_MAX

// Search Structure
struct Step steps[MAX_DEPTH][MAX_STEPS];
int steps_num[MAX_DEPTH];

int step_value[MAX_STEPS];

int depth, max_depth;

// Functions

int ab_call_num = 0;
void show_diagnostics()
{
	ab_call_num++;
	fprintf(stderr, "Call %d, depth %d, player %c", ab_call_num, depth, player2ch(player));
	if (over) fprintf(stderr, " [over]");
	fprintf(stderr, "\n");
	fprintf(stderr, "Scores: %d %d\n", score[0], score[1]);
	if (score[0] < 0 || score[1] < 0)
		printf("Negative score: %d\n", ab_call_num);

	int r, c;
	for (r = 0; r < SIDE_LEN; ++r) {
		for (c = 0; c < SIDE_LEN; ++c)
			fprintf(stderr, "%c", matrix[r][c]);
		fprintf(stderr, "\n");
	}

	fprintf(stderr, "\n");
}

void generate_steps()
{
	int i, t, num = 0;
	struct Step *st_p = &steps[depth][0];
	for (i = 0; i < team[player]; ++i) {
		int r = row_pos[player][i];
		int c = col_pos[player][i];
		for (t = 0; t < MOVE_VAR; ++t) {
			int rn = r + MOVE_ROW[t];
			int cn = c + MOVE_COL[t];
			if (rn < 0 || rn >= SIDE_LEN) continue;
			if (cn < 0 || cn >= SIDE_LEN) continue;
			if (matrix[rn][cn] == player2ch(player)) continue;

			// adding potential step
			st_p->row0 = r;
			st_p->col0 = c;
			st_p->row1 = rn;
			st_p->col1 = cn;
			st_p++;
			num++;

			// TODO: THINK: ok, what about ordering?
		}
	}
	steps_num[depth] = num;
}

// TODO: THINK: ok, what about complicated evaluations?
int evaluate()
{
	int res = 0;

	res += score[player] - score[op(player)];

	int i;
	unsigned char v_maxc = 0, h_maxc = 0;
	for (i = 0; i < SIDE_LEN; i++) {
		if (v_maxc < v_count[player][i])
			v_maxc = v_count[player][i];
		if (h_maxc < h_count[player][i])
			h_maxc = h_count[player][i];
	}
	//res += h_maxc + v_maxc;

	return res;
}

int alphabeta(int alpha, int beta)
{
	if (over)
		return (INF - depth);
	if (depth >= max_depth)
		return evaluate();

	int i;
	struct Step *st_p = &steps[depth][0];
	generate_steps();
	for (i = 0; i < steps_num[depth]; ++i, ++st_p) {
		int mem_score = score[1];
		struct Step mem_step = *st_p;

		char old = move(st_p);

		depth++;
		int tmp = - alphabeta(-beta, -alpha);
		depth--;

		unmove(st_p, old);

		if (tmp > alpha)
			alpha = tmp;

		if (alpha >= beta)
			break;
	}

	return alpha;
}

struct Step find_step()
{
	// initialization
	depth = 0;
	max_depth = MAX_DEPTH;

	// run search
	int chance = alphabeta(-INF, INF);

	// determining optimal step
	int i, opt = 0;
	for (i = 0; i < steps_num[0]; ++i)
		if (step_value[i] > step_value[opt])
			opt = i;

	return steps[0][opt];
}

