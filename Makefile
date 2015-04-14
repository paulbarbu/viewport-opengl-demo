OBJS = main.cpp

CC = g++

COMPILER_FLAGS = -Wall

LINKER_FLAGS = -lGL -lGLU -lglut

OBJ_NAME = barbu_paul_gheorghe_2015

.PHONY: clean

all: $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

clean:
	rm -rf $(OBJ_NAME)
