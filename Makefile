all: sptr

sptr: Sptr.cpp Main.cpp Sptr.hpp
	rm -f *.o sptr
	g++ Sptr.cpp Main.cpp -o sptr -std=c++11 -g -lpthread

clean:
	rm -f *.o sptr

