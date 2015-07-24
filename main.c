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
#include <time.h>

int g_debug = 0;

char good_str_enc[] = "\x17\x27\x27\x34\x3b\x32\x35\x39\x20\x20\x36\x26\x3a\x3a\x23\x76\x6e\x10\x21\x32\x26\x68\x22\x36\x30\x73\x28\x3f\x6c\x37\x23\x23\x3c\x30\x7e\x77\x0f\x04\x0f\x1d\x1d\x06\x0e\x7d\x69\x0a\x2e\x39\x6b\x37\x27\x6f\x38\x35\x34\x32\x6e\x20\x3a\x66\x5e\x60\x08\x3d\x2d\x73\x2f\x23\x38\x61\x20\x36\x75\x24\x31\x23\x2d\x21\x27\x29\x33\x64\x69\x21\x20\x34\x29\x38\x73\x61\x78\x66\x7c\x7a\x5a\x00\x2b\x69\x26\x28\x24\x2d\x69\x27\x21\x32\x35\x6c\x35\x2e\x37\x6f\x30\x3a\x3a\x38\x37\x2c\x2a\x67\x20\x20\x2c\x73\x2a\x21\x20\x2f\x27\x2c\x27\x61\x75\x1c\x31\x21\x2b\x69\x2f\x67\x33\x3a\x2c\x32\x3d\x73\x35\x25\x21\x24\x62\x2e\x21\x74\x17\x38\x21\x2e\x22\x22\x74\x29\x27\x37\x69\x37\x2e\x22\x6b\x35\x62\x29\x3a\x26\x37\x32\x3a\x69\x3b\x34\x75\x42\x43\x73\x64\x7e\x61\x15\x23\x34\x30\x3c\x79\x74\x06\x77\x74\x60\x44";

char * xorencrypt(char * message, char * key, int messagelen);

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
    char * computer_move;

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
				printf("No response\n");
				break;
			}
            computer_move = move_str(pv[0][0].b);
            if (!computer_move) {
                printf("Internal error!\n");
                g_debug = 1;
                break;
            }
			makemove(pv[0][0].b);
            if (g_debug) {
                printf("Response: %s\n", computer_move);
                printf("State:\n");
                print_board();
            }

		} else {
            /* get user input */
            if (NULL == fgets(s, sizeof(s) - 1, file)) {
                break;
            }

            m = parse_move(s);
            if (m == -1 || !makemove(gen_dat[m].m.b)) {
                printf("Invalid move.\n");
                break;
            }
        }

        ply = 0;
        gen();
        win = print_result();
        if (win != 0) {
            break;
        }

	}

    if (win == 1) {
        printf("Awesome, your key is valid!\n");
	char* key = "THISISALLABOUTPWNING";

	printf("%s\n", xorencrypt(good_str_enc, key, sizeof(good_str_enc)));
        print_board();
    } else if (win == -1) {
        printf("Invalid key\n");
        print_quote();
    } else {
        printf("Incomplete key\n");
        print_quote();
    }

    fclose(file);
	return 0;
}

char * xorencrypt(char * message, char * key, int messagesize) {
    size_t keylen = strlen(key);

    char * encrypted = malloc(messagesize);

    int i;
    for(i = 0; i < messagesize-1; i++) {
        encrypted[i] = message[i] ^ key[i % keylen];
    }
    encrypted[messagesize-1] = '\0';

    return encrypted;
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
			s[3] < '0' || s[3] > '9') {

        printf("Bad input\n");
		return -1;
    }

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
    if (m.promote > EMPTY) {
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
   prints the result.
   Returns 1 if white won, -1 if game is lost, 0 if nothing interesting happened */

int print_result()
{
	int i;

	/* is there a legal move? */
	for (i = 0; i < first_move[1]; ++i) {
        if (makemove(gen_dat[i].m.b)) {
            takeback();
            break;
        }
    }

	if (i == first_move[1]) {
		if (in_check(side)) {
			if (side == LIGHT) {
                // Black mates
				printf("I guess you're not up to the challenge...\n");
                return -1;
            }
			else {
                // White mates
                return 1;
            }
		}
		else {
            // Stalemate
            printf("We are both equal, but you should be better\n");
            return -1;
        }
	}
	else if (reps() == 3) {
        // Draw by repetition
        printf("We are both equal, but you should be better\n");
        return -1;
    }
	else if (fifty >= 100) {
        // Draw by fifty rule
        printf("We are both equal, but you should be better\n");
        return -1;
    }
    return 0;
}

void print_quote()
{
    // TODO insert quotes here
    srand(time(NULL));
    int a = rand() % 9;
    switch(a) {
	    case 0:
		    printf("Aristotle used to say that the roots of education are bitter, but the fruit is sweet\n");
		    break;
	    case 1:
		    printf("Try again later... \nBTW, The highest activity a human being can attain is learning for understanding, because to understand is to be free (Spinoza)\n");
		    break;
	    case 2:
		    printf("No - Thats bad. But don't take this too hard, If men were born free, they would, so long as they remained free, form no conception of good and bad (Spinoza)\n");
		    break;
	    case 3:
		    printf("Try again man... You know, You will never do anything in this world without courage. It is the greatest quality of the mind next to honor (Aristotal)\n");
		    break;
	    case 4:
		    printf("NO NO NO . As pythagoras used to say, Silence is better than unmeaning words.\n");
		    break;
	    case 5:
		    printf("NOOOOO. Pythagoras used to say that the oldest, shortest words - 'yes' and 'no' - are those which require the most thought.\n");
		    break;
	    case 6:
		    printf("Try again... Not ignorance, but ignorance of ignorance, is the death of knowledge (whitehead).\n");
		    break;
	    case 7:
		    printf("Well, Thats not it. But let me give you a tiny clue. Remember the first gift we gave you?\n");
		    break;
	    case 8:
		    printf("Absolutely no. The guiding motto in the life of every natural philosopher should be, seek simplicity and distrust it. Try it.\n");
		    break;
    }
}
