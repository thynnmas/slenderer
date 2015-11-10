CC = clang
SRC_PTH = ./src
INC_PTH = -I. -I./include/ -I./dependancies/ -I/usr/include
BLD_PTH = ./obj/
BIN_PTH = ./bin
LIB_PTH = ./lib
LIB_NAME = slenderer
BIN_NAME = example
CFLAGS = -std=gnu99 -Wall -Wno-unused-function -Wno-unused-variable -fno-strict-aliasing -g -DVUL_VECTOR_C89_ITERATORS $(INC_PTH)
LDFLAGS =  -L$(LIB_PTH) -l$(LIB_NAME) -lportaudio -lGLEW -lglfw3 -lm
SHELL = /bin/bash

OS := $(shell uname -s)
ifeq ($(OS),Linux)
CFLAGS += -DVUL_LINUX
LDFLAGS += -lrt -lGL -lGLU -lX11 -lXrandr -lXi -lXxf86vm -lXcursor -lpthread -ldl -lXinerama
else
CFLAGS += -DVUL_OSX
LDFLAGS += -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo
endif

SOURCES = $(shell find $(SRC_PTH)/ -name '*.c')
OBJECTS = $(SOURCES:$(SRC_PTH)/%.c=%)
OBJECT_PATHS = $(SOURCES:$(SRC_PTH)/%.c=$(BLD_PTH)%.o)

.PHONE: pre
	@echo "Beginning build"
	$(MAKE) all --no-pring-directory

.PHONY: dirs
dirs:
	@echo "Creating directories"
	@mkdir -p $(dir $(OBJECT_PATHS))
	@mkdir -p $(BIN_PTH)
	@mkdir -p $(LIB_PTH)

.PHONY: clean
clean:
	@echo "Deleting directories"
	@$(RM) -r $(BIN_PTH)
	@$(RM) -r $(BLD_PTH)
	@$(RM) -r $(LIB_PTH)

all: dirs $(LIB_PTH)/$(LIB_NAME) $(BIN_PTH)/$(BIN_NAME)

$(LIB_PTH)/$(LIB_NAME): $(OBJECTS)
	ar rcs $(LIB_PTH)/lib$(LIB_NAME).a $(OBJECT_PATHS)

$(OBJECTS): $(SOURCES)
	@echo "Compiling: $(SRC_PTH)/$@.c to $(BLD_PTH)$@.o"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -c $(SRC_PTH)/$@.c -o $(BLD_PTH)$@.o

$(SOURCES):
	@echo "Found $@"

$(BIN_PTH)/$(BIN_NAME): 
	@echo "Compiling: $@"
	$(CMD_PREFIX)$(CC) $(CFLAGS) example.c -o $(BIN_PTH)/$(BIN_NAME) $(LDFLAGS)

