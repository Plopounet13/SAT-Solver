#example name
NAME=expr
EXTENSION=toEns Tseitin formule backtrack
#compiler name
CC=g++
#lex name+path
LEX=flex
FLAG= -O3
STD = -std=c++11
LIBLEX=-L/usr/local/Cellar/flex/2.6.0/lib/ -lfl
#yacc/bison
YACC=bison

all: $(NAME).tab.cpp $(NAME).yy.c $(NAME).cpp main.cpp formule.cpp $(EXTENSION:=.cpp)
	$(CC) -Wno-write-strings -o resol $^ $(FLAG) $(LIBLEX) $(STD)

$(NAME).yy.c :  $(NAME).l
	$(LEX)  -o $@ $^

$(NAME).tab.cpp : $(NAME).ypp
	$(YACC) --report=all -o $@ -d $^

clean:
	rm -f *~ $(NAME).yy.c $(NAME).tab.cpp $(NAME).tab.hpp $(NAME).output $(EXTENSION:=.o) $(NAME).o resol
	rm -r resol.dSYM
