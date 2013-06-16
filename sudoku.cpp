/*
 * sudoku.cpp
 *
 *  Created on: May 10, 2009
 *      Author: Benson Che
 */

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <csignal>
#include <ctime>
#include "Board.h"

using namespace std;

// Global variables, taken from command line
bool forward_checking;
bool mrv;
Board *b;
Board *current;
int consistency_checks;
sigset_t block_alarm;
time_t start, finish;

// Prints progress every x seconds.
const int TIME = 1;
const int LIMIT = 10000000;

void read_args(int argc, char *argv[]);
Board* solve_puzzle(Board *board);
void alarm_handler(int sig);

int main(int argc, char *argv[])
{
	consistency_checks = 0;
	read_args(argc, argv);
	b->print_board();
	cout << "\n\n\n\nSolving.....\n";

	if(!b->is_valid_board())
	{
		cout << "invalid board" << endl;
		return 1;
	}

	// Apply forward checking to every variable
	if(forward_checking)
		for(int row=0; row<b->get_size(); row++)
			for(int col=0; col<b->get_size(); col++)
				b->forward_checking(row, col);

	current = 0;

	// Setting up alarm to print progress every few seconds
	sigemptyset(&block_alarm);
	sigaddset(&block_alarm, SIGALRM);
	signal(SIGALRM, alarm_handler);
	alarm(TIME);
	sigprocmask(SIG_BLOCK, &block_alarm, 0);

	start = time(0);

	Board *solution;
	if((solution = solve_puzzle(b)))
		solution->print_board();
	else
		cout << "no solution" << endl;

	finish = time(0);

	cout << "Total consistency checks: " << consistency_checks << endl;
	cout << "Took " << difftime(finish, start) << " seconds to find a solution." << endl;

	delete b;
	delete solution;

	return 0;
}

// Solves the puzzle using recursive backtracking
// Returns true when the assignments are complete
Board* solve_puzzle(Board *board)
{
	if(board->is_complete())
		return new Board(board);

	int row, col;
	board->select_var(&row, &col, mrv);
	for(int value=1; value<=board->get_size(); value++)
	{
		// if it has not been ruled out by forward checking
		if(board->is_valid_move(row, col, value))
		{
			Board *next = new Board(board, row, col, value);
			if(forward_checking)
				next->forward_checking(row, col);

			// current is used for printing progress
			current = next;

			// makes sure current is consistent when it prints
			sigprocmask(SIG_UNBLOCK, &block_alarm, 0);

			consistency_checks++;

			sigprocmask(SIG_BLOCK, &block_alarm, 0);

			// Checking for consistency
			if(next->is_valid_board())
			{
				// solution is 0 if this branch is dead
				Board *solution = solve_puzzle(next);
				if(solution)
				{
					delete next;
					return solution;
				}
			}
			delete next;
		}
		board->mark_move_invalid(row, col, value);
	}
	return 0;
}

// Prints progress when alarm fires
void alarm_handler(int sig)
{
	if(current)
	{
		finish = time(0);
		current->print_board();
		if(forward_checking)
			cout << "Using forward checking." << endl;
		if(mrv)
			cout << "Using MRV." << endl;
		cout << "Consistency checks so far: " << consistency_checks << endl;
		cout << difftime(finish, start) << " seconds has elapsed so far." << endl;
		cout << "Checking an average of " << (double)consistency_checks / difftime(finish,start)
		<< " checks per second." << endl;

		if(LIMIT && consistency_checks > LIMIT)
		{
			cout << "Solution takes > " << consistency_checks << " consistency checks." << endl;
			cout << "Stopping program." << endl;
			exit(0);
		}
	}
	alarm(TIME);
}

void read_args(int argc, char *argv[])
{
	string board_file = "example.sudoku";
	forward_checking = false;
	mrv = false;

	int i;
	for(i=1; i<argc; i++)
	{
		if(!strcmp(argv[i], "-mrv"))
			mrv = true;
		else if(!strcmp(argv[i], "-fc"))
			forward_checking = true;
		else if(!strcmp(argv[i], "-h"))
		{
			cout << "\t-fc enables forward checking" << endl;
			cout << "\t-mrv enables MRV" << endl;

			exit(0);
		}
		else
			board_file = argv[i];
	}

	b = new Board(board_file);
}
