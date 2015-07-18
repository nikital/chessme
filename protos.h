/*
 *	PROTOS.H
 *	Tom Kerrigan's Simple Chess Program (TSCP)
 *
 *	Copyright 1997 Tom Kerrigan
 */


/* prototypes */

/* board.c */
void init_board();
void init_hash();
int hash_rand();
void set_hash();
BOOL in_check(int s);
BOOL attack(int sq, int s);
void gen();
void gen_caps();
void gen_push(int from, int to, int bits);
void gen_promote(int from, int to, int bits);
BOOL makemove(move_bytes m);
void takeback();

/* search.c */
void think();
int search(int alpha, int beta, int depth);
int quiesce(int alpha, int beta);
int reps();
void sort_pv();
void sort(int from);
void checkup();

/* eval.c */
int eval();
int eval_light_pawn(int sq);
int eval_dark_pawn(int sq);
int eval_light_king(int sq);
int eval_lkp(int f);
int eval_dark_king(int sq);
int eval_dkp(int f);

/* main.c */
int get_ms();
int parse_move(char *s);
char *move_str(move_bytes m);
void print_board();
void xboard();
int print_result();
void print_quote();
void bench();
