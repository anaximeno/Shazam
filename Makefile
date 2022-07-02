.DEFAULT: main
.PHONY: main release clean test

CC = g++

FLAGS = -std=c++17 -fPIC -Wall

main: main.cpp
	$(CC) -o $@ main.cpp $(FLAGS)

release: main.cpp
	$(CC) -o main main.cpp $(FLAGS) -O3 -finline-functions

clean:
	rm main
