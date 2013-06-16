/*
 * board.cpp
 *
 *  Created on: May 11, 2009
 *      Author: "Benson Che"
 */

#include "Board.h"
#include <math.h>

// Creates a new board from the input file
Board::Board(string filename)
{
	ifstream in;
	in.open(filename.c_str(), ifstream::in);

	if(!in)
	{
		cerr << "Error: file not found: " << filename << endl;
		exit(1);
	}

	// Assumes correct formatting of input file
	in >> size;

	// allocates the arrays
	grid = new int*[size];
	invalid_moves = new bool**[size];
	for(int i=0; i<size; i++)
	{
		grid[i] = new int[size];
		invalid_moves[i] = new bool*[size];
		for(int j=0; j<size; j++)
		{
			grid[i][j] = 0;
			invalid_moves[i][j] = new bool[size];
			for(int k=0; k<size; k++)
				invalid_moves[i][j][k] = false;
		}
	}

	int initial_values;
	in >> initial_values;

	// initialize the array
	for(int i=0; i<initial_values; i++)
	{
		int row, col, value;
		in >> row;
		in >> col;
		in >> value;

		grid[row-1][col-1] = value;
	}
}

// Clones the board
Board::Board(Board *old)
{
	size = old->size;

	grid = new int*[size];
	invalid_moves = new bool**[size];
	for(int i=0; i<size; i++)
	{
		grid[i] = new int[size];
		invalid_moves[i] = new bool*[size];
		for(int j=0; j<size; j++)
		{
			grid[i][j] = old->grid[i][j];
			invalid_moves[i][j] = new bool[size];
			for(int k=0; k<size; k++)
				invalid_moves[i][j][k] = old->invalid_moves[i][j][k];
		}
	}
}

// Clones the board and add the assignment
Board::Board(Board *old, int row, int col, int value)
{
	size = old->size;

	grid = new int*[size];
	invalid_moves = new bool**[size];
	for(int i=0; i<size; i++)
	{
		grid[i] = new int[size];
		invalid_moves[i] = new bool*[size];
		for(int j=0; j<size; j++)
		{
			grid[i][j] = old->grid[i][j];
			invalid_moves[i][j] = new bool[size];
			for(int k=0; k<size; k++)
				invalid_moves[i][j][k] = old->invalid_moves[i][j][k];
		}
	}

	grid[row][col] = value;
}

Board::~Board()
{
	for(int i=0; i<size; i++)
	{
		for(int j=0; j<size; j++)
			delete[] invalid_moves[i][j];
		delete[] invalid_moves[i];
		delete[] grid[i];
	}
	delete invalid_moves;
	delete grid;
}

// Returns the number of digits, solely used for printing
int get_digits(int num)
{
	int digits = 1;
	int step = 10;
	while(step <= num)
	{
		step *= 10;
		digits++;
	}
	return digits;
}

// whether the move is ruled out by forward checking
bool Board::is_valid_move(int row, int col, int value)
{
	return !invalid_moves[row][col][value-1];
}

int Board::get_assignment(int row, int col)
{
	return grid[row][col];
}

// marks all the invalid moves caused by this new assignment
void Board::forward_checking(int row, int col)
{
	int value = grid[row][col];

	if(!value)
		return;

	for(int i=0; i<size; i++)
	{
		invalid_moves[row][i][value-1] = true;
		invalid_moves[i][col][value-1] = true;
	}

	int root = sqrt(size);
	int row_root, col_root;

	for(row_root=0; row_root<root; row_root++)
		if(row < (row_root + 1) * root)
			break;
	for(col_root=0; col_root<root; col_root++)
		if(col < (col_root + 1) * root)
			break;

	for(int i=0; i< root; i++)
		for(int j=0; j<root; j++)
			invalid_moves[i+row_root*root][j+col_root*root][value-1] = true;

	invalid_moves[row][col][value-1] = false;
}

int Board::get_size()
{
	return size;
}

void Board::print_board()
{
	int digits = get_digits(size);
	int root = sqrt(size);

	cout << "++";
	for(int i=0; i<size; i++)
	{
		cout << "=";
		for(int j=0; j<digits; j++)
			cout << "=";
		cout << "=+";
		if(!((i+1) % root))
			cout << "+";
	}
	cout << endl;

	for(int i=0; i<size; i++)
	{
		cout << "||";
		for(int j=0; j<size; j++)
		{
			if(grid[i][j])
			{
				cout << " " << grid[i][j];
				for(int k=0; k<digits-get_digits(grid[i][j]); k++)
					cout << " ";
				cout << " |";
			}
			else
			{
				cout << " ";
				for(int k=0; k<digits; k++)
					cout << " ";
				cout << " |";
			}
			if(!((j+1) % root))
				cout << "|";
		}
		cout << endl;

		cout << "++";
		for(int j=0; j<size; j++)
		{
			if((i+1) % root)
			{
				cout << "-";
				for(int k=0; k<digits; k++)
					cout << "-";
				cout << "-+";
			}
			else
			{
				cout << "=";
				for(int k=0; k<digits; k++)
					cout << "=";
				cout << "=+";
			}
			if(!((j+1) % root))
				cout << "+";
		}
		cout << endl;
	}
}

// true when it is complete
bool Board::is_complete()
{
	if(is_valid_board())
	{
		for(int i=0; i< size; i++)
			for(int j=0; j<size; j++)
				if(!grid[i][j])
					return false;

		return true;
	}

	return false;
}

// consistency check
bool Board::is_valid_board()
{
	bool exists1[size], exists2[size];

	// Checks horizontal and vertical
	for(int i=0; i<size; i++)
	{
		for(int j=0; j<size; j++)
			exists1[j] = exists2[j] = false;
		for(int j=0; j<size; j++)
		{
			if(grid[i][j])
			{
				if(exists1[grid[i][j]-1])
					return false;
				else
					exists1[grid[i][j]-1] = true;

				// Checking if there are any valid moves
				bool not_valid = true;
				for(int k=0; k<size; k++)
					if(!invalid_moves[i][j][k])
					{
						not_valid = false;
						break;
					}
				if(not_valid)
					return false;
			}

			if(grid[j][i])
			{
				if(exists2[grid[j][i]-1])
				{
					return false;
				}
				else
					exists2[grid[j][i]-1] = true;
			}
		}
	}

	// Checking each sub-grid
	int root = sqrt(size);
	for(int i=0; i<size; i+=root)
		for(int j=0; j<size; j+=root)
		{
			for(int k=0; k<size; k++)
				exists1[k] = false;
			for(int k=0; k<root; k++)
				for(int l=0; l<root; l++)
					if(grid[k+i][l+j])
					{
						if(exists1[grid[k+i][l+j]-1])
							return false;
						else
							exists1[grid[k+i][l+j]-1] = true;
					}
		}

	return true;
}

// selects the next variable to check, whether using MRV or not
void Board::select_var(int *row, int *col, bool mrv)
{
	if(mrv)
	{
		int min = size + 1;

		for(int i=0; i<size; i++)
			for(int j=0; j<size; j++)
			{
				if(!grid[i][j])
				{
					int count = 0;
					for(int k=0; k<size; k++)
						if(!invalid_moves[i][j][k])
							count++;

					if(count < min)
					{
						*row = i;
						*col = j;
						min = count;
						if(count == 1)
							return;
					}
				}
			}
	}
	else
	{
		for(int i=0; i<size; i++)
			for(int j=0; j<size; j++)
				if(!grid[i][j])
				{
					*row = i;
					*col = j;
					return;
				}

	}
}

void Board::mark_move_invalid(int row, int col, int value)
{
	invalid_moves[row][col][value-1] = true;
}

