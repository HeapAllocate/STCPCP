CC		:= gcc
CFLAGS	:= -Wall -Wextra -g

BIN		    := ../bin
SRC		    := src
SHARED  	:= ../shared
INCLUDE	    := include
LIB		    := lib

LIBRARIES	:=

ifeq ($(OS),Windows_NT)
EXECUTABLE	:= server.exe
else
EXECUTABLE	:= server
endif

SOURCEDIRS	:= $(shell find $(SRC) -type d)
INCLUDEDIRS	:= $(shell find $(INCLUDE) -type d)
LIBDIRS		:= $(shell find $(LIB) -type d)
SHAREDIRS   := $(shell find $(SHARED) -type d)

CINCLUDES	:= $(patsubst %,-I%, $(INCLUDEDIRS:%/=%))
CLIBS		:= $(patsubst %,-L%, $(LIBDIRS:%/=%))

SOURCES		:= $(wildcard $(patsubst %,%/*.c, $(SHAREDIRS) $(SOURCEDIRS)))
OBJECTS		:= $(SOURCES:.c=.o)


all: $(BIN)/$(EXECUTABLE)

.PHONY: clean
clean:
	-$(RM) $(BIN)/$(EXECUTABLE)
	-$(RM) $(OBJECTS)


run: all
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(CINCLUDES) $(CLIBS) $^ -o $@ $(LIBRARIES)