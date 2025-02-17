INCLUDE=-I/home/vinilicia/cplex/cplex/include -I/home/vinilicia/cplex/concert/include

FLAGS=-DIL_STD -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -w

LPATH=-L/home/vinilicia/cplex/cplex/lib/x86-64_linux/static_pic -L/home/vinilicia/cplex/concert/lib/x86-64_linux/static_pic

LIBRARIES=-lconcert -lilocplex -lcplex -lpthread -ldl

all: main.o
	g++ -O3 main.o -o main.exe $(LPATH) $(LIBRARIES)

main.o: main.cpp
	g++ -O3 -c main.cpp $(INCLUDE) $(FLAGS)

run:
	./main.exe < in.txt
	
clean:
	rm -f *.exe
	rm -f *.o
