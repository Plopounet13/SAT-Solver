#example name
NAME=expr
EXTENSION=toEns Tseitin formule backtrack myv equality union_find
#compiler name
CC=clang++
#lex name+path
LEX=flex
FLAGS= -O3 -g
STD = -std=c++11
LIBLEX=-L/usr/local/Cellar/flex/2.6.0/lib/ -lfl
#yacc/bison
YACC=bison

all: $(NAME).tab.cpp $(NAME).yy.c $(NAME).cpp main.cpp $(EXTENSION:=.cpp)
	$(CC) -Wno-write-strings -o resol $^ $(FLAGS) $(LIBLEX) $(STD)

$(NAME).yy.c :  $(NAME).l
	$(LEX)  -o $@ $^

$(NAME).tab.cpp : $(NAME).ypp
	$(YACC) --report=all -o $@ -d $^

clean:
	rm -f *~ $(NAME).yy.c $(NAME).tab.cpp $(NAME).tab.hpp $(NAME).output $(EXTENSION:=.o) $(NAME).o resol
	rm -r -f resol.dSYM
