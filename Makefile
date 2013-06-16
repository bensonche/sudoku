FLAGS=-O3

all: sudoku
	
sudoku: sudoku.o Board.o
	g++ ${FLAGS} $^ -o $@

%.o: %.cpp
	g++ ${FLAGS} $< -c

run: sudoku
	@./sudoku

clean:
	-rm *.o
	-rm sudoku
	