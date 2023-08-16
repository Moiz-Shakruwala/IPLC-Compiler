CXX = g++-8
EXE = ipl_parser
CXXDEBUG = -g -Wall
CXXSTD = -std=c++11


.PHONY: all
all: parser lexer	
	$(CXX) $(CXXDEBUG) -o iplC driver.cpp parser.o scanner.o ast.cpp type.cpp symbtab.cpp

parser: parser.yy scanner.hh
	bison -d -v $<
	$(CXX) $(CXXDEBUG) -c parser.tab.cc -o parser.o 

lexer: scanner.l scanner.hh parser.tab.hh parser.tab.cc	
	flex++ --outfile=scanner.yy.cc  $<
	$(CXX)  $(CXXDEBUG) -c scanner.yy.cc -o scanner.o

clean: 
	rm location.hh parser.o parser.output parser.tab.cc parser.tab.hh position.hh 	scanner.o scanner.yy.cc stack.hh iplC
