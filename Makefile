# Based on
# https://stackoverflow.com/a/53138757

MKDIR	:= mkdir
RMDIR 	:= rm -Rf
CC 		:= gcc
BIN 	:= ./bin
OBJ 	:= ./obj
INCLUDE	:= ./include
SRC		:= ./src
SRCS	:= $(wildcard $(SRC)/*.c)
OBJS	:= $(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(SRCS))
EXEEXT 	:= elf
EXE 	:= $(BIN)/tp1-grupo11.$(EXEEXT)
CFLAGS 	:= -I$(INCLUDE) -fdiagnostics-color=auto --std=gnu11 -Wall --pedantic -g
LDLIBS	:=

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJS) | $(BIN)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN) $(OBJ):
	$(MKDIR) $@

clean:
	$(RMDIR) $(OBJ) $(BIN)
