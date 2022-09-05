# Based on
# https://stackoverflow.com/a/53138757

MKDIR		:= mkdir -p
RMDIR		:= rm -Rf
CC			:= gcc

BIN			:= ./bin
OBJ			:= ./obj
INCLUDE		:= ./include
SRC			:= ./src

SRCS_UTIL	:= $(wildcard $(SRC)/util/*.c)
OBJS_UTIL	:= $(patsubst $(SRC)/util/%.c,$(OBJ)/util/%.o,$(SRCS_UTIL))
SRCS_MD5	:= $(wildcard $(SRC)/md5/*.c)
OBJS_MD5	:= $(patsubst $(SRC)/md5/%.c,$(OBJ)/md5/%.o,$(SRCS_MD5))
SRCS_VIEW	:= $(wildcard $(SRC)/vista/*.c)
OBJS_VIEW	:= $(patsubst $(SRC)/vista/%.c,$(OBJ)/vista/%.o,$(SRCS_VIEW))

EXE_MD5		:= $(BIN)/md5
EXE_VIEW	:= $(BIN)/vista

CFLAGS		:= -I$(INCLUDE) -fdiagnostics-color=auto --std=gnu11 -Wall \
			   --pedantic -g
LDLIBS		:= -pthread -lrt

.PHONY: all clean

all: $(EXE_MD5) $(EXE_VIEW)

# MD5
$(EXE_MD5): $(OBJS_MD5) $(OBJS_UTIL) | $(BIN)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

# VISTA
$(EXE_VIEW): $(OBJS_VIEW) $(OBJS_UTIL) | $(BIN)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(MKDIR) $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN) $(OBJ):
	$(MKDIR) $@

clean:
	$(RMDIR) $(OBJ) $(BIN)
