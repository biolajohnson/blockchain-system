Name: Abiola Johnson
Student ID: 4745823702
Course: EE450

HOW TO RUN CODE:
please run the program in the following order, with clientA first then clientB. e.g 
./clientA Alice
******
./clientB Ben
******
note: The last 3 digits of my ports are 702 (in coherence with my student ID)

DESCRIPTION OF PROJECT 
===========================================================================================
In this project i developed a distributed system which consist of 3 backend servers 
that communicate to a main server through UPD sockets and a main server that communicates with a client application through TCP sockets. 
in this project, this program is an abstraction of a real blockchain service where members of the network can transfer coins among themselves and check their account balance.
an additional bonus functionality is the ability of a get sorted list of all the transactions made on the network from the inception of the network. 

GUIDELINES TO RUN THE PROJECT 
===========================================================================================
	1. compile all the source code with the make tool by running make: (the default action is to make all the source files and create the executables.
	2. create 5 terminal windows dedicated to the 3 backend servers (servers A, B, and C) the main server and the 2 clients (clients A, and B)
	3. run all the executables. on on each terminal create with appropriate commands. the various servers run and keep listening for incoming connections. the main server by design will be waiting for a client connection and the backend servers will be expecting a request from the main server. 
	4. the client terminal can perform the following actions: 
		check wallet: with ./client* <username>
		transfer coins: with ./client* <username1> <username2> <amount> 
		get a sorted list of transactions on the network: the command ./client* TXLIST

FILES IN THE PROJECT 
===========================================================================================
1. serverA.cpp
2. serverB.cpp
3. serverC.cpp
4. serverM.cpp
5. clientA.cpp
6. clientB.cpp

BRIEF SUMMARY OF THE FUCNTIONALITIES OF EACH FILE
===========================================================================================
1. serverA: this file reads from files/block1.txt, which contains a list of unordered previous transactions that this server is responsible for. 
while reading from the text file, it parses the information from the text file into a string with delimiters concatenated per transaction.
this is done in order to successfully send all the transactions in the list to the main server in one send operation. 
the file is also responsible for receiving (random) transactions from the main server and saving each transaction it receives in files/block1.txt

2. serverB: this file reads from files/block2.txt, which contains a list of unordered previous transactions that this server is responsible for. 
while reading from the text file, it parses the information from the text file into a string with delimiters concatenated per transaction.
this is done in order to successfully send all the transactions in the list to the main server in one send operation. 
the file is also responsible for receiving (random) transactions from the main server and saving each transaction it receives in files/block2.txt

3. serverC: this file reads from files/block3.txt, which contains a list of unordered previous transactions that this server is responsible for. 
while reading from the text file, it parses the information from the text file into a string with delimiters concatenated per transaction.
this is done in order to successfully send all the transactions in the list to the main server in one send operation. 
the file is also responsible for receiving (random) transactions from the main server and saving each transaction it receives in files/block3.txt

4. serverM.cpp: this file is the heart of the application. it is responsible for creating and maintaining the 2 TCP sockets (one for each client) 
and 1 UDP socket (for communications with the backend servers). It also contains the core functionalities and operations the clients request,
such requests include sorting the transactions into a list and saving that list in a file titled "alicoins.txt"

5. clientA.cpp: this client file creates an interface for any member of the network to retrieve thier current balance, and transfer coins to other members of the network.
this client file also provides teh abililty to download a sorted list of all the transactions performed on the network with the "TXLIST" command 

6. clientB.cpp: this client file creates an interface for any member of the network to retrieve thier current balance, and transfer coins to other members of the network.
this client file also provides teh abililty to download a sorted list of all the transactions performed on the network with the "TXLIST" command



===========================================================================================
file produced: alicoins.txt (when "TXLIST" is run by the clientA or clientB)

EXTRA FILES USED 
===========================================================================================
My implementation includes 4 additional files which are, 
1. transaction.cpp
2. transaction.h
3. client.cpp
4. client.h

These files help me organise my project better. 
The transaction.cpp abd transaction.h files handle organizing all transaction instances and store each transaction as an object in which information such as "amount, or transaction number" can be easilty assessed.
The client.cpp and client.h files handle organizing the clients by storing each client as an object with properties such as "name" and "amount left".
- each client object also has methods to transfer and to receive coins. 


