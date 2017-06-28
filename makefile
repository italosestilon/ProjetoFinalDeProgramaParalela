
COMPILE: paralelo/stack/STACK.cpp paralelo/load/LOAD.cpp sequencial/SERIAL.cpp
	g++ -std=c++11 -O3 paralelo/stack/STACK.cpp paralelo/stack/ThreadPool.h paralelo/stack/ThreadPool.cpp -o STACK -lpthread
	g++ -std=c++11 -O3 paralelo/load/LOAD.cpp paralelo/load/ThreadPool.h paralelo/load/ThreadPool.cpp -o LOAD -lpthread
	g++ -O3 sequencial/SERIAL.cpp -o SERIAL
run:
	./run.sh

clean:
	@rm -f run STACK LOAD SERIAL input/*.out