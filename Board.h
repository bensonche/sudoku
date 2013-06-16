/*
 * board.h
 *
 *  Created on: May 11, 2009
 *      Author: "Benson Che"
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>

using namespace std;

class Board {
public:
	Board(string filename);
	Board(Board *old, int row, int col, int value);
	Board(Board *old);
	virtual ~Board();

	void print_board();
	// Checks for consistency
	bool is_valid_board();
	// Checks if the move has been ruled out by forward checking
	bool is_valid_move(int row, int col, int value);
	bool is_complete();
	int get_assignment(int row, int col);
	int get_size();
	void mark_move_invalid(int row, int col, int value);
	// Apply forward checking from the given row and column
	void forward_checking(int row, int col);
	// Selects the next variable to test
	void select_var(int *row, int *col, bool mrv);

private:
	int **grid;
	int size;

	// For forward checking, mark invalid moves
	bool ***invalid_moves;
};

#endif /* BOARD_H_ */
