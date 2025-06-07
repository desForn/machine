CXX ?= g++
CXXFLAGS ?= -std=c++26 -Wall -Wextra -pedantic
SRC_DIR ?= ../source/
OBJ_DIR ?= ../object/

programs = example_1_3
all : ${programs}

object :
	cd ${OBJ_DIR} && ${MAKE}

example_1_3 : example_1_3.cpp object
	${CXX} ${CXXFLAGS} -I ${SRC_DIR} -o example_1_3 ${OBJ_DIR}machine.o ${OBJ_DIR}string.o \
	   ${OBJ_DIR}input.o ${OBJ_DIR}control.o example_1_3.cpp

clean :
	rm ${programs}

clean_all : clean
	cd ${OBJ_DIR} && ${MAKE} clean
