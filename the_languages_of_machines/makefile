CXX ?= g++
CXXFLAGS ?= -std=c++26 -Wall -Wextra -pedantic -g
SRC_DIR ?= ../source/
OBJ_DIR ?= ../object/

OBJECTS = \
	${OBJ_DIR}machine.o \
	${OBJ_DIR}string.o \
	${OBJ_DIR}encoder.o \
	${OBJ_DIR}control.o \
	${OBJ_DIR}input.o \
	${OBJ_DIR}output.o \
	${OBJ_DIR}stack.o \
	${OBJ_DIR}unsigned_counter.o \
	${OBJ_DIR}counter.o \
	${OBJ_DIR}tape.o

programs = run
all : ${programs}

objects : ${SRC_DIR}*
	cd ${OBJ_DIR} && ${MAKE}

run : run.cpp objects
	${CXX} ${CXXFLAGS} -I ${SRC_DIR} -o run ${OBJECTS} run.cpp

clean :
	rm ${programs}

clean_all : clean
	cd ${OBJ_DIR} && ${MAKE} clean
