CXX ?= g++
CXXFLAGS ?= -std=c++26 -Wall -Wextra -pedantic
SRC_DIR ?= ../source/

objects = machine.o string.o input.o control.o

all : ${objects}

machine.o : ${SRC_DIR}machine.cpp ${SRC_DIR}machine.hpp ${SRC_DIR}control.hpp ${SRC_DIR}string.hpp \
            ${SRC_DIR}fwd.hpp
	${CXX} ${CXXFLAGS} -c -o machine.o ${SRC_DIR}machine.cpp 

string.o : ${SRC_DIR}string.cpp ${SRC_DIR}string.hpp ${SRC_DIR}fwd.hpp
	${CXX} ${CXXFLAGS} -c -o string.o ${SRC_DIR}string.cpp 

input.o : ${SRC_DIR}input.cpp ${SRC_DIR}input.hpp ${SRC_DIR}machine.hpp ${SRC_DIR}string.hpp \
          ${SRC_DIR}fwd.hpp
	${CXX} ${CXXFLAGS} -c -o input.o ${SRC_DIR}input.cpp 

control.o : ${SRC_DIR}control.cpp ${SRC_DIR}control.hpp ${SRC_DIR}machine.hpp \
	        ${SRC_DIR}string.hpp ${SRC_DIR}fwd.hpp
	${CXX} ${CXXFLAGS} -c -o control.o ${SRC_DIR}control.cpp 

.PHONY : clean
clean :
	rm ${objects}

