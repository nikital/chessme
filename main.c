/*
 *	MAIN.C
 *	Tom Kerrigan's Simple Chess Program (TSCP)
 *
 *	Copyright 1997 Tom Kerrigan
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "defs.h"
#include "data.h"
#include "protos.h"


/* main() is basically an infinite loop that either calls
   think() when it's the computer's turn to move or prompts
   the user for a command (and deciphers it). */

int main(int argc, char * argv[])
{
	int computer_side;
	char s[256] = {0};
	int m;
    FILE * file;
    int win = 0;

    if (argc != 2) {
        printf("Gimme something!\n");
        return 0;
    }

    if (!(file = fopen(argv[1], "r"))) {
        printf("No key :(\n");
        return 0;
    }

	init_hash();
	init_board();
	gen();
	computer_side = xside;
	max_depth = 1;
	for (;;) {
		if (side == computer_side) {  /* computer's turn */
			
			/* think about the move and make it */
			think();
			if (!pv[0][0].u) {
				printf("(no legal moves)\n");
				break;
			}
            if (!move_str(pv[0][0].b)) {
                printf("invalid challenge response.\n");
                break;
            }
			makemove(pv[0][0].b);
			ply = 0;
			gen();
			print_result();
			continue;
		}

		/* get user input */
        if (NULL == fgets(s, sizeof(s) - 1, file)) {
            break;
        }

		/* maybe the user entered a move? */
		m = parse_move(s);
		if (m == -1 || !makemove(gen_dat[m].m.b)) {
			printf("invalid challenge request.\n");
            break;
        }
		else {
			ply = 0;
			gen();
			if (print_result()) {
                win = 1;
                break;
            }
		}
	}

    if (win) {
        printf("Awesome!\n");
    }
    print_board();
    fclose(file);
	return 0;
}


/* parse the move s (in coordinate notation) and return the move's
   index in gen_dat, or -1 if the move is illegal */

int parse_move(char *s)
{
	int from, to, i;

	/* make sure the string looks like a move */
	if (s[0] < 'a' || s[0] > 'h' ||
			s[1] < '0' || s[1] > '9' ||
			s[2] < 'a' || s[2] > 'h' ||
			s[3] < '0' || s[3] > '9')
		return -1;

	from = s[0] - 'a';
	from += 8 * (8 - (s[1] - '0'));
	to = s[2] - 'a';
	to += 8 * (8 - (s[3] - '0'));

	for (i = 0; i < first_move[1]; ++i)
		if (gen_dat[i].m.b.from == from && gen_dat[i].m.b.to == to) {

			/* if the move is a promotion, handle the promotion piece;
			   assume that the promotion moves occur consecutively in
			   gen_dat. */
			if (gen_dat[i].m.b.bits & 32)
				switch (s[4]) {
					case 'N':
						return i;
					case 'B':
						return i + 1;
					case 'R':
						return i + 2;
					default:  /* assume it's a queen */
						return i + 3;
				}
			return i;
		}

	/* didn't find the move */
	return -1;
}


/* move_str returns a string with move m in coordinate notation */

char *move_str(move_bytes m)
{
	static char str[6];

	char c;

    // Should never happen
    if (m.promote > 20) {
        return 0;
    }

	if (m.bits & 32) {
		switch (m.promote) {
			case KNIGHT:
				c = 'n';
				break;
			case BISHOP:
				c = 'b';
				break;
			case ROOK:
				c = 'r';
				break;
			default:
				c = 'q';
				break;
		}
		sprintf(str, "%c%d%c%d%c",
				COL(m.from) + 'a',
				8 - ROW(m.from),
				COL(m.to) + 'a',
				8 - ROW(m.to),
				c);
	}
	else
		sprintf(str, "%c%d%c%d",
				COL(m.from) + 'a',
				8 - ROW(m.from),
				COL(m.to) + 'a',
				8 - ROW(m.to));
	return str;
}


/* print_board() prints the board */

void print_board()
{
	int i;
	
	printf("\n8 ");
	for (i = 0; i < 64; ++i) {
		switch (color[i]) {
			case EMPTY:
				printf(" .");
				break;
			case LIGHT:
				printf(" %c", piece_char[piece[i]]);
				break;
			case DARK:
				printf(" %c", piece_char[piece[i]] + ('a' - 'A'));
				break;
		}
		if ((i + 1) % 8 == 0 && i != 63)
			printf("\n%d ", 7 - ROW(i));
	}
	printf("\n\n   a b c d e f g h\n\n");
}


/* print_result() checks to see if the game is over, and if so,
   prints the result. */

int print_result()
{
	int i;

	/* is there a legal move? */
	for (i = 0; i < first_move[1]; ++i)
		if (makemove(gen_dat[i].m.b)) {
			takeback();
			break;
		}

	if (i == first_move[1]) {
		if (in_check(side)) {
			if (side == LIGHT)
				printf("Not bad, but try again...\n");
			else
                return 1;
		}
		else
			printf("Good, but not enough :/\n");
	}
	else if (reps() == 3)
		printf("You are both equal, but you should be better\n");
	else if (fifty >= 100)
		printf("You are both equal, but you should be better\n");
    return 0;
}

