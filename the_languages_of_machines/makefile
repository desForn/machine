CXX ?= g++
CXXFLAGS ?= -std=c++26 -Wall -Wextra -pedantic
SRC_DIR ?= ../source/
OBJ_DIR ?= ../object/

OBJECTS = \
	${OBJ_DIR}machine.o \
	${OBJ_DIR}string.o \
	${OBJ_DIR}control.o \
	${OBJ_DIR}input.o \
	${OBJ_DIR}output.o \
	${OBJ_DIR}stack.o \
	${OBJ_DIR}unsigned_counter.o \
	${OBJ_DIR}counter.o \
	${OBJ_DIR}tape.o

programs = example_1_3
all : ${programs}

objects : ${SRC_DIR}*
	cd ${OBJ_DIR} && ${MAKE}

example_1_3 : example_1_3.cpp objects
	${CXX} ${CXXFLAGS} -I ${SRC_DIR} -o example_1_3 ${OBJECTS} example_1_3.cpp

clean :
	rm ${programs}

clean_all : clean
	cd ${OBJ_DIR} && ${MAKE} clean
