.DEFAULT := main

.PHONY: main release clean test

CC = g++

FLAGS = -std=c++17 -fPIC -Wall -g

HLIB_FILES = include/external/hashlib2plus/hl_md5.cpp \
            include/external/hashlib2plus/hl_md5wrapper.cpp \
			include/external/hashlib2plus/hl_sha1.cpp \
			include/external/hashlib2plus/hl_sha1wrapper.cpp \
			include/external/hashlib2plus/hl_sha2ext.cpp \
			include/external/hashlib2plus/hl_sha256.cpp \
			include/external/hashlib2plus/hl_sha256wrapper.cpp \
			include/external/hashlib2plus/hl_sha384wrapper.cpp \
			include/external/hashlib2plus/hl_sha512wrapper.cpp \
			include/external/hashlib2plus/hl_wrapperfactory.cpp

HLIB_OBJS = hl_md5.o \
            hl_md5wrapper.o \
			hl_sha1.o \
			hl_sha1wrapper.o \
			hl_sha2ext.o \
			hl_sha256.o \
			hl_sha256wrapper.o \
			hl_sha384wrapper.o \
			hl_sha512wrapper.o \
			hl_wrapperfactory.o

shazam: main.o shazam.o $(HLIB_OBJS)
	$(CC) -o $@ $(FLAGS) $^

main.o: main.cpp
	$(CC) -c $(FLAGS) $^

shazam.o: src/shazam.cc
	$(CC) -c $(FLAGS) $^

$(HLIB_OBJS): $(HLIB_FILES)
	$(CC) -c $(FLAGS) $^

release: main.cpp
	$(CC) -o main main.cpp $(FLAGS) -O3 -finline-functions

clean:
	rm shazam *.o