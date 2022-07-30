.DEFAULT := main

.PHONY: main clean test

CC = g++

FLAGS = -std=c++17 -fPIC -g -Wextra

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

SHAZAM_FILES = src/shazam.cc \
			   src/common.cc

SHAZAM_OBJS = shazam.o \
			  common.o

shazam: main.o $(SHAZAM_OBJS) $(HLIB_OBJS)
	@echo -n "Compiling shazam... "
	@$(CC) -o $@ $(FLAGS) $^
	@echo Done.
	@echo
	@echo Run it using ./shazam

main.o: main.cpp
	@echo -n "Compiling main.cpp... "
	@$(CC) -c $(FLAGS) $^
	@echo Done.

$(SHAZAM_OBJS): $(SHAZAM_FILES)
	@echo -n "Compiling lib shazam... "
	@$(CC) -c $(FLAGS) $^
	@echo Done.

$(HLIB_OBJS): $(HLIB_FILES)
	@echo -n "Compiling lib hashlib2plus... "
	@$(CC) -c $(FLAGS) $^
	@echo Done.

test: tests.cpp $(SHAZAM_OBJS) $(HLIB_OBJS)
	@echo -n "Compiling the test... "
	@$(CC) -o test $(FLAGS) $^
	@echo Done.
	@echo
	@echo -n "Running test... "
	@./test

clean:
	@echo -n "Cleaning... "
	@rm test shazam *.o
	@echo Done.
