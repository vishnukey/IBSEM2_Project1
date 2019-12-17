SRC = ./src

all:
	clang++ -lpthread -o chat -std=c++14 ${SRC}/*
debug:
	clang++ -lpthread -g -o chat_DEBUG -std=c++14 ${SRC}/*
