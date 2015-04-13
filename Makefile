OBJS = main.cpp

CC = g++

COMPILER_FLAGS = -Wall

LINKER_FLAGS = -lGL -lGLU -lglut

OBJ_NAME = proiect_barbu

all: $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
