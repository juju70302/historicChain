
make: main.o block.o sha256.o blockchain.o network.o
	g++ main.o block.o sha256.o blockchain.o network.o -o main.exe
warning:
	rm -rf *.o *.exe
	g++ -c -Wall -std=c++11 src/sha256.cpp
	g++ -c -Wall -std=c++11 src/block.cpp
	g++ -c -Wall -std=c++11 src/blockchain.cpp
	g++ -c -Wall -std=c++11 src/network.cpp
	g++ -c -Wall -std=c++11 src/main.cpp
	g++ main.o sha256.o block.o blockchain.o network.o -o main.exe
	rm -rf *.o
all:
	rm -rf *.o *.exe
	g++ -c -std=c++11 src/sha256.cpp
	g++ -c -std=c++11 src/block.cpp
	g++ -c -std=c++11 src/blockchain.cpp
	g++ -c -std=c++11 src/network.cpp
	g++ -c -std=c++11 src/main.cpp
	g++ main.o sha256.o block.o blockchain.o network.o -o main.exe
	rm -rf *.o
main.o: src/main.cpp
	g++ -c -std=c++11 src/main.cpp
sha256.o: src/sha256.cpp
	g++ -c -std=c++11 src/sha256.cpp
block.o: src/block.cpp
	g++ -c -std=c++11 src/block.cpp
blockchain.o: src/blockchain.cpp
	g++ -c -std=c++11 src/blockchain.cpp
network.o: src/network.cpp
	g++ -c -std=c++11 src/network.cpp
clean:
	rm -rf *.o *.exe