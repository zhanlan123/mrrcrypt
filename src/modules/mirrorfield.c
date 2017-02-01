// Copyright (c) 2016 Brian Barto
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the MIT License. See LICENSE for more details.

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "main.h"
#include "modules/mirrorfield.h"

/*
 * MODULE DESCRIPTION
 * 
 * The mirrorfield module manages the loading, validating, and traversing
 * of the mirror field. The cryptographic algorithm is implemented here.
 * If the debug flag is set then this module also draws the mirror field
 * and animates the encryption process.
 */

#define MIRROR_NONE      -1
#define MIRROR_FORWARD   -2
#define MIRROR_STRAIGHT  -3
#define MIRROR_BACKWARD  -4
#define DIR_UP            1
#define DIR_DOWN          2
#define DIR_LEFT          3
#define DIR_RIGHT         4

// Static Variables
//static int grid[GRID_SIZE * GRID_SIZE];
//static unsigned char perimeterChars[GRID_SIZE * 4];

struct gridnode {
	int value;
	struct gridnode *up;
	struct gridnode *down;
	struct gridnode *left;
	struct gridnode *right;
};
static struct gridnode gridnodes[GRID_SIZE * GRID_SIZE];
static struct gridnode perimeter[GRID_SIZE * 4];
static int so_perimeter[GRID_SIZE * 4];

// Static Function Prototypes
static struct gridnode *mirrorfield_crypt_char_advance(struct gridnode *, int);
static void mirrorfield_roll_chars(struct gridnode *, struct gridnode *);

/*
 * The mirrorfield_init() function initializes any static variables.
 */
void mirrorfield_init(void) {
	int i;
	
	// Init gridnode values
	for (i = 0; i < GRID_SIZE * GRID_SIZE; ++i) {
		gridnodes[i].value = 0;
		gridnodes[i].up = NULL;
		gridnodes[i].down = NULL;
		gridnodes[i].left = NULL;
		gridnodes[i].right = NULL;
	}
	
	// Init perimeter values
	for (i = 0; i < GRID_SIZE * 4; ++i) {
		perimeter[i].value = 0;
		perimeter[i].up = NULL;
		perimeter[i].down = NULL;
		perimeter[i].left = NULL;
		perimeter[i].right = NULL;
		
		so_perimeter[i] = 0;
	}
}

/*
 * The mirrorfield_set() function accepts one character at a time and
 * loads them sequentially into the static variables that contain the
 * mirror field and perimeter characters.
 * 
 * Zero is returned if it gets a character it doesn't expect, although
 * this is just a cursory error checking process. 
 */
int mirrorfield_set(unsigned char ch) {
	static int i = 0;
	int j, k;
	
	// Set mirror value and links
	if (i < GRID_SIZE * GRID_SIZE) {
		
		// Set mirror value
		if (ch == '/') {
			gridnodes[i].value = MIRROR_FORWARD;
		} else if (ch == '\\') {
			gridnodes[i].value = MIRROR_BACKWARD;
		} else if (ch == '-') {
			gridnodes[i].value = MIRROR_STRAIGHT;
		} else if (ch == ' ') {
			gridnodes[i].value = MIRROR_NONE;
		} else {
			return 0;
		}
		
		// Link left/right
		for (j = 1; j <= i % GRID_SIZE; ++j) {
			if (gridnodes[i - j].value < MIRROR_NONE) {
				gridnodes[i].left = &gridnodes[i - j];
				gridnodes[i - j].right = &gridnodes[i];
				break;
			}
		}
		
		// Link up/down
		for (j = GRID_SIZE; i - j >= 0; j += GRID_SIZE) {
			if (gridnodes[i - j].value < MIRROR_NONE) {
				gridnodes[i].up = &gridnodes[i - j];
				gridnodes[i - j].down = &gridnodes[i];
				break;
			}
		}

	// Set perimiter value and link
	} else if (i < (GRID_SIZE * GRID_SIZE) + (GRID_SIZE * 4)) {
		
		// Store order or perimeter characters
		so_perimeter[i - (GRID_SIZE * GRID_SIZE)] = (int)ch;
		
		// Setting perimeter value by index
		perimeter[(int)ch].value = (int)ch;
		
		// Linking top row characters
		if (i - (GRID_SIZE * GRID_SIZE) < GRID_SIZE * 1) {

			// Link down/up
			for (j = i - (GRID_SIZE * GRID_SIZE); j < GRID_SIZE * GRID_SIZE; j += GRID_SIZE) {
				if (gridnodes[j].value < MIRROR_NONE) {
					perimeter[(int)ch].down = &gridnodes[j];
					gridnodes[j].up = &perimeter[(int)ch];
					break;
				}
			}
			
		// Linking right row characters
		} else if (i - (GRID_SIZE * GRID_SIZE) < GRID_SIZE * 2) {

			// Getting adjacent grid position
			j = i % GRID_SIZE;
			j = (j * GRID_SIZE) + (GRID_SIZE - 1);
			
			// Link left/right
			for (k = j; k > j - GRID_SIZE; --k) {
				if (gridnodes[k].value < MIRROR_NONE) {
					perimeter[(int)ch].left = &gridnodes[k];
					gridnodes[k].right = &perimeter[(int)ch];
					break;
				}
			}
			
		// Linking left row
		} else if (i - (GRID_SIZE * GRID_SIZE) < GRID_SIZE * 3) {
			
			// Getting adjacent grid position
			j = i % GRID_SIZE;
			j = (j * GRID_SIZE);
			
			// Link left/right
			for (k = j; k < j + GRID_SIZE; ++k) {
				if (gridnodes[k].value < MIRROR_NONE) {
					perimeter[(int)ch].right = &gridnodes[k];
					gridnodes[k].left = &perimeter[(int)ch];
					break;
				}
			}
			
			// If we don't have a mirror, link to opposite perimeter character
			if (k >= j + GRID_SIZE) {
				perimeter[(int)ch].right = &perimeter[so_perimeter[i - (GRID_SIZE * GRID_SIZE) - GRID_SIZE]];
				perimeter[so_perimeter[i - (GRID_SIZE * GRID_SIZE) - GRID_SIZE]].left = &perimeter[(int)ch];
			}
			
		// Linking bottom row characters
		} else if (i - (GRID_SIZE * GRID_SIZE) < GRID_SIZE * 4) {

			// Link up/down
			for (j = i - (GRID_SIZE * 4); j >= 0; j -= GRID_SIZE) {
				if (gridnodes[j].value < MIRROR_NONE) {
					perimeter[(int)ch].up = &gridnodes[j];
					gridnodes[j].down = &perimeter[(int)ch];
					break;
				}
			}
			
			// If we don't have a mirror, link to opposite perimeter character
			if (j < 0) {
				perimeter[(int)ch].up = &perimeter[so_perimeter[i - (GRID_SIZE * GRID_SIZE) - (GRID_SIZE * 3)]];
				perimeter[so_perimeter[i - (GRID_SIZE * GRID_SIZE) - (GRID_SIZE * 3)]].down = &perimeter[(int)ch];
			}
		}
		
	// Ignore extra characters
	} else {
		return 0;
	}
	
	// Increment our static counter
	++i;
	
	return 1;
}

/*
 * The mirrorfield_validate() function checks that the data contained in
 * the static variables for the mirror field and perimeter character is
 * valid.
 * 
 * Zero is returned if invalid.
 */
int mirrorfield_validate(void) {
	int i, j;

	// Check mirrors
	for (i = 0; i < GRID_SIZE * GRID_SIZE; ++i) {
		if (gridnodes[i].value > MIRROR_NONE || gridnodes[i].value < MIRROR_BACKWARD) {
			return 0;
		}
	}
	
	// Check for duplicate perimeter chars
	for (i = 0; i < GRID_SIZE * 4; ++i) {
		for (j = i+1; j < GRID_SIZE * 4; ++j) {
			if (perimeter[i].value == perimeter[j].value) {
				return 0;
			}
			if (so_perimeter[i] == so_perimeter[j]) {
				return 0;
			}
		}
	}
	
	return 1;
}

/*
 * The mirrorfield_crypt_char() function receives a cleartext character
 * and traverses the mirror field to find it's cyphertext equivelent,
 * which is then returned. It also calls mirrorfield_roll_chars() after
 * the cyphertext character is determined.
 */
unsigned char mirrorfield_crypt_char(unsigned char ch, int debug) {
	int d;
	struct gridnode *startnode = &perimeter[(int)ch];
	struct gridnode *endnode = NULL;
	
	//static int evenodd = 0;
	
	// Toggle evenodd var
	//evenodd = (evenodd + 1) % 2;
	
	// For the debug flag
	//struct timespec ts;
	//ts.tv_sec = debug / 1000;
	//ts.tv_nsec = (debug % 1000) * 1000000;
	
	// Set initial direction
	if (startnode->down != NULL) {
		d = DIR_DOWN;
	} else if (startnode->up != NULL) {
		d = DIR_UP;
	} else if (startnode->left != NULL) {
		d = DIR_LEFT;
	} else if (startnode->right != NULL) {
		d = DIR_RIGHT;
	}
	
	// Traverse the mirror field and find the cyphertext node
	endnode = mirrorfield_crypt_char_advance(startnode, d);

	// This is a way of returning the cleartext char as the cyphertext char and still preserve decryption.
	//if (evenodd && ((int)perimeterChars[startCharPos] == startCharPos || (int)perimeterChars[endCharPos] == endCharPos)) {
	//	ech = perimeterChars[startCharPos];
	//}
	
	// Roll start and end chars
	mirrorfield_roll_chars(startnode, endnode);

	// Return crypted char
	return (unsigned char)endnode->value;
}

/*
 * The mirrorfield_crypt_char_advance() is a recursive function that traverses
 * the mirror field and returns a pointer to the node containing the cypthertext
 * character. This function also handles mirror rotation.
 */
static struct gridnode *mirrorfield_crypt_char_advance(struct gridnode *p, int d) {
	struct gridnode *t;

	// Advance character
	switch (d) {
		case DIR_DOWN:
			p = p->down;
			break;
		case DIR_LEFT:
			p = p->left;
			break;
		case DIR_RIGHT:
			p = p->right;
			break;
		case DIR_UP:
			p = p->up;
			break;
	}
	
	// If we don't have the cypthertext, determine new direction and perform
	// a recursive call.
	if (p->value < 0) {

		switch (p->value) {
			case MIRROR_FORWARD:
				switch (d) {
					case DIR_DOWN:
						d = DIR_LEFT;
						break;
					case DIR_LEFT:
						d = DIR_DOWN;
						break;
					case DIR_RIGHT:
						d = DIR_UP;
						break;
					case DIR_UP:
						d = DIR_RIGHT;
						break;
				}
				break;
			case MIRROR_BACKWARD:
				switch (d) {
					case DIR_DOWN:
						d = DIR_RIGHT;
						break;
					case DIR_LEFT:
						d = DIR_UP;
						break;
					case DIR_RIGHT:
						d = DIR_DOWN;
						break;
					case DIR_UP:
						d = DIR_LEFT;
						break;
				}
				break;
				
		}
		
		// Perform recursive call. t will be our cyphertext node.
		t = mirrorfield_crypt_char_advance(p, d);
		
		// Rotate mirror after we get cyphertext
		switch (p->value) {
			case MIRROR_FORWARD:
				p->value = MIRROR_STRAIGHT;
				break;
			case MIRROR_BACKWARD:
				p->value = MIRROR_FORWARD;
				break;
			case MIRROR_STRAIGHT:
				p->value = MIRROR_BACKWARD;
				break;
		}
		
		// After rotating mirror at p, assign it the cyphertext node.
		p = t;
	}
	
	// Return cyphertext node
	return p;
}

/*
 * The mirrorfield_roll_chars() function received the starting and ending
 * positions of the cleartext and cyphertext character respectively, and
 * implements a character rolling process to make the perimeter character
 * positions dynamic and increase randomness in the output.
 * 
 * No value is returned.
 */
static void mirrorfield_roll_chars(struct gridnode *startnode, struct gridnode *endnode) {
	int i;
	static int j = 0;
	struct gridnode *rollstart;
	struct gridnode *rollend;
	struct gridnode temp;
	
	i = 1;
	do {
		rollstart = &perimeter[(startnode->value + j + i++) % (GRID_SIZE * 4)];
	} while (rollstart == endnode);
	
	i = 1;
	do {
		rollend = &perimeter[(endnode->value + j + i++) % (GRID_SIZE * 4)];
	} while (rollend == startnode);
	
	j = (j + 1) % (GRID_SIZE * 4);
	
	if (startnode->value > endnode->value) {
		// roll start node
		if (startnode->up != NULL)
			startnode->up->down = rollstart;
		if (startnode->down != NULL)
			startnode->down->up = rollstart;
		if (startnode->left != NULL)
			startnode->left->right = rollstart;
		if (startnode->right != NULL)
			startnode->right->left = rollstart;
		
		if (rollstart->up != NULL)
			rollstart->up->down = startnode;
		if (rollstart->down != NULL)
			rollstart->down->up = startnode;
		if (rollstart->left != NULL)
			rollstart->left->right = startnode;
		if (rollstart->right != NULL)
			rollstart->right->left = startnode;
		
		temp.up = startnode->up;
		temp.down = startnode->down;
		temp.left = startnode->left;
		temp.right = startnode->right;
		
		startnode->up = rollstart->up;
		startnode->down = rollstart->down;
		startnode->left = rollstart->left;
		startnode->right = rollstart->right;
		
		rollstart->up = temp.up;
		rollstart->down = temp.down;
		rollstart->left = temp.left;
		rollstart->right = temp.right;
		
		// roll end node
		if (endnode->up != NULL)
			endnode->up->down = rollend;
		if (endnode->down != NULL)
			endnode->down->up = rollend;
		if (endnode->left != NULL)
			endnode->left->right = rollend;
		if (endnode->right != NULL)
			endnode->right->left = rollend;
		
		if (rollend->up != NULL)
			rollend->up->down = endnode;
		if (rollend->down != NULL)
			rollend->down->up = endnode;
		if (rollend->left != NULL)
			rollend->left->right = endnode;
		if (rollend->right != NULL)
			rollend->right->left = endnode;

		temp.up = endnode->up;
		temp.down = endnode->down;
		temp.left = endnode->left;
		temp.right = endnode->right;
		
		endnode->up = rollend->up;
		endnode->down = rollend->down;
		endnode->left = rollend->left;
		endnode->right = rollend->right;
		
		rollend->up = temp.up;
		rollend->down = temp.down;
		rollend->left = temp.left;
		rollend->right = temp.right;
	} else {
		// roll end node
		if (endnode->up != NULL)
			endnode->up->down = rollend;
		if (endnode->down != NULL)
			endnode->down->up = rollend;
		if (endnode->left != NULL)
			endnode->left->right = rollend;
		if (endnode->right != NULL)
			endnode->right->left = rollend;
		
		if (rollend->up != NULL)
			rollend->up->down = endnode;
		if (rollend->down != NULL)
			rollend->down->up = endnode;
		if (rollend->left != NULL)
			rollend->left->right = endnode;
		if (rollend->right != NULL)
			rollend->right->left = endnode;

		temp.up = endnode->up;
		temp.down = endnode->down;
		temp.left = endnode->left;
		temp.right = endnode->right;
		
		endnode->up = rollend->up;
		endnode->down = rollend->down;
		endnode->left = rollend->left;
		endnode->right = rollend->right;
		
		rollend->up = temp.up;
		rollend->down = temp.down;
		rollend->left = temp.left;
		rollend->right = temp.right;
		
		// roll start node
		if (startnode->up != NULL)
			startnode->up->down = rollstart;
		if (startnode->down != NULL)
			startnode->down->up = rollstart;
		if (startnode->left != NULL)
			startnode->left->right = rollstart;
		if (startnode->right != NULL)
			startnode->right->left = rollstart;
		
		if (rollstart->up != NULL)
			rollstart->up->down = startnode;
		if (rollstart->down != NULL)
			rollstart->down->up = startnode;
		if (rollstart->left != NULL)
			rollstart->left->right = startnode;
		if (rollstart->right != NULL)
			rollstart->right->left = startnode;

		temp.up = startnode->up;
		temp.down = startnode->down;
		temp.left = startnode->left;
		temp.right = startnode->right;
		
		startnode->up = rollstart->up;
		startnode->down = rollstart->down;
		startnode->left = rollstart->left;
		startnode->right = rollstart->right;
		
		rollstart->up = temp.up;
		rollstart->down = temp.down;
		rollstart->left = temp.left;
		rollstart->right = temp.right;
	}

	return;
}

/*
 * The mirrorfield_draw() function draws the current state of the mirror
 * field and perimeter characters. It receives x/y coordinates and highlights
 * that position on the field.
 */
 /*
void mirrorfield_draw(int pos_r, int pos_c) {
	int r, c;
	static int resetCursor = 0;
	
	// Save cursor position if we need to reset it
	// Otherwise, clear screen.
	if (resetCursor)
		printf("\033[s");
	else
		printf("\033[2J");

	// Set cursor position to 0,0
	printf("\033[H");
	
	for (r = -1; r <= GRID_SIZE; ++r) {

		for (c = -1; c <= GRID_SIZE; ++c) {
			
			// Highlight cell if r/c match
			if (r == pos_r && c == pos_c) {
				printf("\x1B[30m"); // foreground black
				printf("\x1B[47m"); // background white
			}
			
			// Print apropriate mirror field character
			if (r == -1 && c == -1)                // Upper left corner
				printf("%2c", ' ');
			else if (r == -1 && c == GRID_SIZE)         // Upper right corner
				printf("%2c", ' ');
			else if (r == GRID_SIZE && c == -1)         // Lower left corner
				printf("%2c", ' ');
			else if (r == GRID_SIZE && c == GRID_SIZE)  // Lower right corner
				printf("%2c", ' ');
			else if (r == -1)                           // Top chars
				printf("%2x", perimeterChars[c]);
			else if (c == GRID_SIZE)                    // Right chars
				printf("%2x", perimeterChars[r + GRID_SIZE]);
			else if (r == GRID_SIZE)                    // Bottom chars
				printf("%2x", perimeterChars[c + (GRID_SIZE * 3)]);
			else if (c == -1)                           // Left chars
				printf("%2x", perimeterChars[r + (GRID_SIZE * 2)]);
			else if (grid[(r * GRID_SIZE) + c] == MIRROR_FORWARD)
				printf("%2c", '/');
			else if (grid[(r * GRID_SIZE) + c] == MIRROR_BACKWARD)
				printf("%2c", '\\');
			else if (grid[(r * GRID_SIZE) + c] == MIRROR_STRAIGHT)
				printf("%2c", '-');
			else
				printf("%2c", ' ');
			
			// Un-Highlight cell if r/c match
			if (r == pos_r && c == pos_c)
				printf("\x1B[0m");

		}
		printf("\n");
	}
	printf("\n");
	
	// Restore cursor position if we need to
	if (resetCursor)
		printf("\033[u");
	else
		resetCursor = 1;
}
*/

