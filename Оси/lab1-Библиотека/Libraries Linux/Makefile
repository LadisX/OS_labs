CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -fPIC


all: libNumber.a libVector.so main

libNumber.a: Number.o
	ar rcs libNumber.a Number.o

Number.o: Number.cpp Number.h
	$(CXX) $(CXXFLAGS) -c Number.cpp -o Number.o


libVector.so: Vector.o libNumber.a
	$(CXX) -shared Vector.o -L. -lNumber -o libVector.so

Vector.o: Vector.cpp Vector.h Number.h
	$(CXX) $(CXXFLAGS) -c Vector.cpp -o Vector.o


main: Libraries.o libNumber.a libVector.so
	$(CXX) Libraries.o -L. -lNumber -lVector -Wl,-rpath,. -o main

Libraries.o: Libraries.cpp Number.h Vector.h
	$(CXX) $(CXXFLAGS) -c Libraries.cpp -o Libraries.o


clean:
	rm -f *.o *.a *.so main

run: main
	./main

.PHONY: all clean run