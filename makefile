final: dir_help hash
	g++ -o plagiarismCatcher dir_help.o hash.o
dir_help:
	g++ -std=c++11 -O0 -g -Wall -Werror -o dir_help.o -c dir_help.cpp
hash: hash.cpp hash.h
	g++ -std=c++11 -O0 -g -Wall -Werror -o hash.o -c hash.cpp