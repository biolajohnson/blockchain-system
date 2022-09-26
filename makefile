FLAGS = -std=c++11
CXX = g++

all:
	make serverA
	make serverB
	make serverC
	make clientA
	make clientB
	make serverM


serverA: serverA.cpp
	$(CXX) serverA.cpp $(FLAGS) -o serverA

serverB: serverB.cpp
	$(CXX) serverB.cpp $(FLAGS) -o serverB

serverC: serverC.cpp
	$(CXX) serverC.cpp $(FLAGS) -o serverC

serverM: serverM.cpp transaction.cpp transaction.h client.h client.cpp
	$(CXX) serverM.cpp transaction.cpp client.cpp $(FLAGS) -o serverM

clientA: clientA.cpp 
	$(CXX) clientA.cpp $(FLAGS) -o clientA

clientB: clientB.cpp
	$(CXX) clientB.cpp $(FLAGS) -o clientB

client: client.cpp 
	$(CXX) client.cpp $(FLAGS) -o client

transaction: transaction.cpp
	$(CXX) transaction.cpp $(FLAGS) -o transaction

