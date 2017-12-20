
CC=g++
CFLAGS=-pedantic -Wextra -Wall
CXXFLAGS=$(CFLAGS) -std=c++11

AR=ar

SRC_DIR=./
OBJ_DIR=./

INC=-I ./

UNAME_S:=$(shell uname -s)
ifeq ($(UNAME_S),Linux)
	CV:=opencv
else
	CV:=OpenCV
endif

LIBS=$(shell pkg-config --libs --cflags ${CV})

CXXFLAGS+=$(INC)

SRC=$(shell find . -name "*.cpp")

OBJ=$(patsubst $(SRC_DIR)%.cpp,$(OBJ_DIR)%.o,$(SRC))

EXECUTABLE=NeuralObjectClassification

release: all

all: CFLAGS+=-O2
all: $(EXECUTABLE)

debug: CFLAGS+=-O0 -g
debug: $(EXECUTABLE)

%.cpp%.o:
	$(CC) -c -o $@ $< $(CXXFLAGS)

# Make the executable named EXECUTABLE using CC
$(EXECUTABLE): $(OBJ)
	$(CC) -o $@ $(CXXFLAGS) $(OBJ) $(INC) $(LIBS)

clean:
	rm -rf $(OBJ)
	rm -f $(EXECUTABLE)
