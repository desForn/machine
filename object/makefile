CXX ?= g++
CXXFLAGS ?= -std=c++26 -Wall -Wextra -pedantic -g
SRC_DIR ?= ../source/

devices = control.o input.o output.o stack.o unsigned_counter.o counter.o tape.o

objects = machine.o string.o encoder.o ${devices}

all : ${objects}

machine.o : ${SRC_DIR}machine.cpp ${SRC_DIR}machine.hpp ${SRC_DIR}string.hpp \
			${SRC_DIR}encoder.hpp ${SRC_DIR}control.hpp ${SRC_DIR}input.hpp \
			${SRC_DIR}output.hpp ${SRC_DIR}stack.hpp ${SRC_DIR}unsigned_counter.hpp \
			${SRC_DIR}counter.hpp ${SRC_DIR}tape.hpp ${SRC_DIR}fwd.hpp
	${CXX} ${CXXFLAGS} -c -o machine.o ${SRC_DIR}machine.cpp 

string.o : ${SRC_DIR}string.cpp ${SRC_DIR}string.hpp ${SRC_DIR}fwd.hpp
	${CXX} ${CXXFLAGS} -c -o string.o ${SRC_DIR}string.cpp 

encoder.o : ${SRC_DIR}encoder.cpp ${SRC_DIR}encoder.hpp ${SRC_DIR}string.hpp ${SRC_DIR}fwd.hpp
	${CXX} ${CXXFLAGS} -c -o encoder.o ${SRC_DIR}encoder.cpp 

control.o : ${SRC_DIR}control.cpp ${SRC_DIR}control.hpp ${SRC_DIR}machine.hpp \
	        ${SRC_DIR}string.hpp ${SRC_DIR}fwd.hpp
	${CXX} ${CXXFLAGS} -c -o control.o ${SRC_DIR}control.cpp 

input.o : ${SRC_DIR}input.cpp ${SRC_DIR}input.hpp ${SRC_DIR}machine.hpp \
		  ${SRC_DIR}string.hpp ${SRC_DIR}fwd.hpp
	${CXX} ${CXXFLAGS} -c -o input.o ${SRC_DIR}input.cpp 

output.o : ${SRC_DIR}output.cpp ${SRC_DIR}output.hpp ${SRC_DIR}machine.hpp \
		  ${SRC_DIR}string.hpp ${SRC_DIR}fwd.hpp
	${CXX} ${CXXFLAGS} -c -o output.o ${SRC_DIR}output.cpp 

stack.o : ${SRC_DIR}stack.cpp ${SRC_DIR}stack.hpp ${SRC_DIR}machine.hpp \
		  ${SRC_DIR}string.hpp ${SRC_DIR}fwd.hpp
	${CXX} ${CXXFLAGS} -c -o stack.o ${SRC_DIR}stack.cpp 

unsigned_counter.o : ${SRC_DIR}unsigned_counter.cpp ${SRC_DIR}unsigned_counter.hpp ${SRC_DIR}machine.hpp \
		  ${SRC_DIR}string.hpp ${SRC_DIR}fwd.hpp
	${CXX} ${CXXFLAGS} -c -o unsigned_counter.o ${SRC_DIR}unsigned_counter.cpp 

counter.o : ${SRC_DIR}counter.cpp ${SRC_DIR}counter.hpp ${SRC_DIR}machine.hpp \
		  ${SRC_DIR}string.hpp ${SRC_DIR}fwd.hpp
	${CXX} ${CXXFLAGS} -c -o counter.o ${SRC_DIR}counter.cpp 

tape.o : ${SRC_DIR}tape.cpp ${SRC_DIR}tape.hpp ${SRC_DIR}machine.hpp \
		  ${SRC_DIR}string.hpp ${SRC_DIR}fwd.hpp
	${CXX} ${CXXFLAGS} -c -o tape.o ${SRC_DIR}tape.cpp 

.PHONY : clean
clean :
	rm ${objects}

