#specify target                                                                 
all: cache-sim

cache-sim: cache-sim.o                                               
	g++ cache-sim.o -o cache-sim

cache-sim.o: cache-sim.cpp
	g++ -c cache-sim.cpp -o cache-sim.o

#removes executables                                                            
clean:
	rm -f *.o cache-sim


