CCC = g++
CCFLAGS= -g -fpermissive
LEX = flex
LFLAGS= -8     
YACC= bison 
YFLAGS= -d -t -y

RM = /bin/rm -f

lex-pars: y.tab.o lex.yy.o lex-pars.o
	${CCC} ${CCFLAGS} lex.yy.o y.tab.o lex-pars.o -o lex-pars 

lex-pars.o: lex-pars.cpp lex-pars.h
	${CCC} -c lex-pars.cpp
y.tab.o: lex-pars.yacc
	${YACC} ${YFLAGS} lex-pars.yacc
	${CCC} ${CCFLAGS} y.tab.c -c 

lex.yy.o: lex-pars.lex
	${LEX} $(LFLAGS) lex-pars.lex
	${CCC} ${CCFLAGS} lex.yy.c -c 

clean:
	/bin/rm -f lex.yy.* y.tab.* *.o lex-pars
