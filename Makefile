CXX = g++ -std=c++11

all: winzigc

winzigc: main.o scanner.o parser.o ScannerStatus.o Token.o Node.o
	$(CXX) -o winzigc main.o scanner.o parser.o ScannerStatus.o Token.o Node.o

main.o: main.cpp
	$(CXX) -c main.cpp

scanner.o: scanner.cpp
	$(CXX) -c scanner.cpp

parser.o: parser.cpp
	$(CXX) -c parser.cpp

ScannerStatus.o: Classes/ScannerStatus.cpp
	$(CXX) -c Classes/ScannerStatus.cpp

Token.o: Classes/Token.cpp
	$(CXX) -c Classes/Token.cpp

Node.o: Classes/Node.cpp
	$(CXX) -c Classes/Node.cpp

clean:
	 rm *.o winzigc