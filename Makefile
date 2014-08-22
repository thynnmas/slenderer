CC = clang
SRC_PTH = ./src
INC_PTH = -I. -I./include/ -I./dependancies/ -I/usr/include
BLD_PTH = ./obj/
BIN_PTH = ./bin
LIB_PTH = ./lib
LIB_NAME = slenderer
BIN_NAME = example
CFLAGS = -std=c99 -Wall -Wextra -g -Werror -DVUL_LINUX $(INC_PTH)
LDFLAGS = -lglfw -lGL
SHELL = /bin/bash

SOURCES = $(shell find $(SRC_PTH)/ -name '*.c')
OBJECTS = $(SOURCES:$(SRC_PTH)/%.c=$(BLD_PTH)%.o)

.PHONE: pre
	@echo "Beginning build"
	$(MAKE) all --no-pring-directory

.PHONY: dirs
dirs:
	@echo "Creating directories"
	@mkdir -p $(dir $(OBJECTS))
	@mkdir -p $(BIN_PTH)
	@mkdir -p $(LIB_PTH)

.PHONY: clean
clean:
	@echo "Deleting directories"
	@$(RM) -r $(BIN_PTH)
	@$(RM) -r $(BLD_PTH)
	@$(RM) -r $(LIB_PTH)

all: $(LIB_PTH)/$(LIB_NAME) $(BIN_PTH)/$(BIN_NAME)
	@echo $(OBJECTS)

$(LIB_PTH)/$(LIB_NAME): $(OBJECTS)

$(OBJECTS): $(SOURCES)
	@echo "Compiling: $@"
	$(CMD_PREFIX)$(CC) $(CFLAGS) -o $@ $<

$(BIN_PTH)/$(BIN_NAME):
	@echo "Compiling: $@"
	$(CMD_PREFIX)$(CC) $(CFLAGS) example.c -o $(BIN_PTH)/$(BIN_NAME)

