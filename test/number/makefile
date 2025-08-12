CXX ?= "g++"
CXXFLAGS ?= -std=c++26 -Wall -Wextra -pedantic -g
SRC_DIR ?= ../../source/
OBJ_DIR ?= ../../object/

number_test : ../*
	cd ${OBJ_DIR} && ${MAKE}
	${CXX} ${CXXFLAGS} -o number_test number_test.cpp ${OBJ_DIR}string.o ${OBJ_DIR}encoder.o ${OBJ_DIR}number.o -I ${SRC_DIR}

