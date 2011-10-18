#include "algo.h"
#include "board.h"

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#define INF INT_MAX

/* Options */
#define MAX_DEPTH 3
#define MAX_STEPS 130

#define MAX_SCORE 200
#define WIN_VALUE (INF-MAX_SCORE)

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
#define EVALUATE_STRAIGHT_W 1

/* Search Structure */
struct Step steps[MAX_DEPTH][MAX_STEPS];
int steps_num[MAX_DEPTH];
int step_value[MAX_STEPS];
int depth, max_depth;

/* Statistics */
#ifdef STATISTICS
int count_evaluate = 0;      // number of evaluate() calls
int count_alphabeta = 0;     // number of alphabeta() calls
int count_prunings = 0;      // number of prunings
int count_over = 0;          // number of game overs found
int count_steps = 0;         // number of steps generated
#ifdef SEARCH_NULLWIN
int count_nullwin = 0;       // number of null-window searches
int count_nullwin_fails = 0; // number of null-window fails
#endif
#ifdef STEPS_ORDER
int count_step_order[STEPS_MAX_COST+1] = {0,0,0}; // number of steps has the order
#endif 

void print_statistics()
{
	printf("Statistics:\n");
	printf("  alphabeta calls:   %d \n", count_alphabeta);
	printf("  prunings:          %d \n", count_prunings);
	printf("  evaluate calls:    %d \n", count_evaluate);
	printf("  game over found:   %d \n", count_over);
	printf("  steps generated:   %d \n", count_steps);
	#ifdef SEARCH_NULLWIN
	printf("  null-window calls: %d (%.3lf%% fails)\n", count_nullwin,
	       (double)(count_nullwin_fails)/count_nullwin);
	#endif
	#ifdef STEPS_ORDER
	printf("  order values:      ");
	int k;
	for (k = 0; k <= STEPS_MAX_COST; ++k)
		printf("%d(%.2lf%%) ", k, (double)(count_step_order[k])/count_steps);
	printf("\n");
	#endif
}

#define STAT_INC(V) ++V;
#else
#define STAT_INC(V) 
#endif // STATISTICS

/* STEPS function */
void sort_steps()
{
	struct Step tmp;
	struct Step *ptr, *s_ptr = &steps[depth][0];
	struct Step *end_ptr = &steps[depth][steps_num[depth]];
	int c;
	for (c = STEPS_MAX_COST; c >= 0; --c)
		for (ptr = s_ptr; ptr != end_ptr; ++ptr) {
			if (ptr->cost != c)
				continue;
			// pulling step up
			tmp = *s_ptr;
			*s_ptr = *ptr;
			*ptr = tmp;
			++s_ptr;
		}
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
			STAT_INC(count_steps);

			// deciding step cost
			#ifdef STEPS_ORDER
			st_p->cost = 0;
			if (matrix[rn][cn] == player2ch(op(player))) {
				st_p->cost = 1; // eating
				#ifdef STEPS_ORDER_OP_PARADE
				int t;
				for (t = 0; t < SIDE_LEN; ++t)
					if (h_count[op(player)][t] + 1 == team[op(player)] ||
					    v_count[op(player)][t] + 1 == team[op(player)])
					    st_p->cost = 2; // game over, opponent has a parade
				#endif
			}
			if (h_count[player][rn] + 1 == team[player] ||
			    v_count[player][cn] + 1 == team[player])
				st_p->cost = 2; // game over, player has a parade
			STAT_INC(count_step_order[st_p->cost]);
			#endif
		}
	}
	steps_num[depth] = num;
	#ifdef STEPS_ORDER
	sort_steps();
	#endif
}

/* EVALUATE function */
int evaluate()
{
	STAT_INC(count_evaluate);
	int res = 0;

	#ifdef EVALUATE_SCORE
	res += (score[player] - score[op(player)])*EVALUATE_SCORE_W;
	#endif

	#ifdef EVALUATE_STRAIGHT
	int i;
	unsigned char v_maxc = 0, h_maxc = 0;
	unsigned char opp_v_maxc = 0, opp_h_maxc = 0;
	for (i = 0; i < SIDE_LEN; i++) {
		if (v_maxc < v_count[player][i])
			v_maxc = v_count[player][i];
		if (h_maxc < h_count[player][i])
			h_maxc = h_count[player][i];
		if (opp_v_maxc < v_count[op(player)][i])
			opp_v_maxc = v_count[op(player)][i];
		if (opp_h_maxc < h_count[op(player)][i])
			opp_h_maxc = h_count[op(player)][i];
	}
	res += ((h_maxc > v_maxc ? h_maxc : v_maxc) -
	       (opp_h_maxc > opp_v_maxc ? opp_h_maxc : opp_v_maxc))*EVALUATE_STRAIGHT_W;
	#endif

	return res;
}

/* SEARCH function */
int alphabeta(int alpha, int beta)
{
	STAT_INC(count_alphabeta);
	if (over) {
		// evaluate game outcome
		STAT_INC(count_over);
		int delta_sc = score[player] - score[op(player)];
		if (delta_sc > 0)
			// win: as fast as possible
			return WIN_VALUE - depth;
		else if (delta_sc == 0)
			// draw: neither good nor bad
			return 0;
		else
			// loss: as shameless as possible
			return - WIN_VALUE + delta_sc;
	}
	if (depth >= max_depth)
		// evaluate game position
		return evaluate();

	generate_steps();
	struct Step *st_p = &steps[depth][0];

	#ifdef SEARCH_NULLWIN
	int b = beta;
	#endif
	int i, tmp;
	char old;
	for (i = 0; i < steps_num[depth]; ++i, ++st_p) {
		// make move
		old = move(st_p);
		depth++;

		#ifdef SEARCH_NULLWIN
		// null-window search
		STAT_INC(count_nullwin);
		tmp = -alphabeta(-b, -alpha);
		// full re-search, if null-window failed
		if ((alpha < tmp) && (tmp < beta) && i) {
			STAT_INC(count_nullwin_fails);
			tmp = -alphabeta(-beta, -alpha);
		}
		#else
		// simple search
		old = move(st_p);
		tmp = -alphabeta(-beta, -alpha);
		#endif

		// unmaking move
		depth--;
		unmove(st_p, old);

		// updating alpha
		if (tmp > alpha)
			alpha = tmp;

		if (depth == 0)
			// memorize first step value
			step_value[i] = tmp;

		if (alpha >= beta) {
			// prune
			STAT_INC(count_prunings);
			break;
		}

		#ifdef SEARCH_NULLWIN
		// update b
		b = alpha + 1;
		#endif
	}

	return alpha;
}

/* main function */
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

