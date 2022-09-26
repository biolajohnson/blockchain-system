# include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include "client.h"
#include "transaction.h"
#include <vector>
#include <unordered_map>
#include <fstream>
#include <set>


using namespace std;

/**
 * Utility function to see all the elements in the unordered map */

void print_map(unordered_map<string, Client>  client_map){
  std::unordered_map<string, Client>::iterator iter;
  for (iter = client_map.begin(); iter != client_map.end(); iter++){
    cout << iter->first << endl;
  }
}

// splt a string ito a vector of strings wih a delimiter

void tokenize(string const &str, const char delim, vector<string> &out){
    size_t start;
    size_t end = 0;
 
    while ((start = str.find_first_not_of(delim, end)) != string::npos){
        end = str.find(delim, start);
        out.push_back(str.substr(start, end - start));
    }
}


// processes the response from a server (UDP). it concatenates the response to "str". returns 0 when successful and -1 in error. 

int process_response(string &str, char*response){
  int i = 0;
  while (response[i] != 0)
  {
    str += response[i];
    i++;
  }
  return 0;
}



// create a udp server and bind to the specified port number. returns 0 when successful and -1 in error

int create_udp_socket(sockaddr_in &server, int port_number){
  struct sockaddr_in local_address;
 
  local_address.sin_family = AF_INET;
  local_address.sin_addr.s_addr = inet_addr("127.0.0.1");
  local_address.sin_port = htons(24702);

  int udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr("127.0.0.1");
  server.sin_port = htons(port_number);

  if(::bind(udp_socket, (struct sockaddr*)&local_address, sizeof(local_address)) < 0){
    cout << "can not bind the udp socket to the local address" << endl;
    return -1;
  }
  return udp_socket;
}


// sends a udp datagram to the specified server (specified with the port number and address info) 

int send_to_udp_server(sockaddr_in &server, socklen_t server_len, int port_number, int buffer_size, string str_request, char*response, string server_letter, string operation){
      int udp_socket = create_udp_socket(server, port_number);
      char request[str_request.length()];
      strcpy(request, str_request.c_str());

      if (sendto(udp_socket, request, buffer_size, 0, (struct sockaddr *)&server, sizeof(server)) < 0)
      {
        cout << "could not send message to server " << server_letter << endl;
        return -1;
       }
       cout << "The main server sent a request to server " << server_letter << endl;
       if (recvfrom(udp_socket, response, buffer_size, 0, (struct sockaddr *)&server, &server_len) < 0)
       {
         cout << "could not receive from server b" << endl;
         return -1;
       }
       if (operation == "TXCOINS"){
        cout << "The main server received the feedback from server " << server_letter << " using UDP over port " << port_number << endl;
       }else{
        cout << "The main server received transactions from Server " << server_letter << " using UDP over port " << port_number << endl;
       }
       close(udp_socket);
       return 0;
}



// clears the client_map map and transaction history. 

int clean_up(vector<Transaction> &transactions, unordered_map<string, Client> &client_map){
  client_map.clear();
  vector<Transaction>().swap(transactions);
  return 0;
}



// checks the client map for the client and retrieve remaining balance

int process_check_wallet(string input, char *result, unordered_map<string, Client>client_map){
  if (client_map.find(input) == client_map.end())
  {
    return -1;
  }
  string str = client_map[input].to_string();
  strcpy(result, str.c_str());
  return 0;
}




// parse client message to determine aht operation to perform

int parse_client_message(string client_message, vector<string> &inputs, string &operation){
  char delim = ' ';
  tokenize(client_message, delim, inputs);
  int size = inputs.size();
  if(size == 3){
    operation = "TXCOINS";
  }else if(inputs[0] == "TXLIST"){
    operation = "TXLIST";
  }
  else
  {
    operation = "CHECK WALLET";
  }

  return 0;
}


// parse input from client and sent "from", "to" and "amount" parameters to prepare for "process_transfer_coins".

int parse_client_input_for_transfer(vector<string> inputs, string&from, string&to, string&tx_amount){
  from = inputs[0];
  to = inputs[1];
  tx_amount = inputs[2];
  return 0;
}


// processes the transfer and receipt of coins from one client to another. 

int process_transfer_coins(string from, string to, string tx_amount, unordered_map<string,Client> &client_map, string& result, string& result_for_udp, int n){
  if (client_map.count(from) != 1 || client_map.count(to) != 1){
    if(client_map.count(from) != 1 && client_map.count(to) != 1){
      result = from + "," + to;
    }
    else if(client_map.count(from) != 1){
      result = from;
    }
    else if (client_map.count(to) != 1)
    {
      result =  to;
    }
    return -1;
  }
  Client sender = client_map[from];
  Client receiver = client_map[to];
  if(sender.transfer(stoi(tx_amount)) < 0){
    result = "funds," + sender.to_string();
    return -1;
  };
  receiver.receive(stoi(tx_amount));
  Transaction transaction = Transaction(n + 1, from, to, stoi(tx_amount));
  result = "success," + sender.to_string();
  result_for_udp = *transaction.stringify();
  return 0;
}

// swap function to help with sorting. the swap function swaps the the positions of
// transactions in the transaction list. 
void swap(vector<Transaction> &transactions, int i, int j){
  Transaction temp = transactions[i];
  transactions[i] = transactions[j];
  transactions[j] = temp;
}

// sort function to help sort the transactions
void sort_transactions(vector<Transaction> &transactions, int n)
{
  int i = 0;
  while (i < transactions.size())
  {
    int j = transactions[i].transaction_number - 1;
    if (transactions[i].transaction_number != transactions[j].transaction_number)
    {
      swap(transactions, i, j);
    }else{
      i++;
    }
  }
}


// processes transactions and builds the transaction list
int process_transactions(string str, vector<Transaction> &transactions, vector<string>&lines){
  vector<Transaction>().swap(transactions);
  char delim = ',';
  tokenize(str, delim, lines);

  for (int i = 0; i < lines.size(); i++)
  {
    vector<string> transac;
    tokenize(lines[i], ' ', transac);
    int tx_number = stoi(transac[0]);
    string sender = transac[1];
    string receiver = transac[2];
    int amount = stoi(transac[3]);
    Transaction transaction = Transaction(tx_number, sender, receiver, amount);
    transactions.push_back(transaction);
  }
  vector<string>().swap(lines);
  // sort(transactions.begin(), transactions.end(), comparator);
  sort_transactions(transactions, transactions.size());
  return 0;
}


// process clients from a transactions vectors. places the clients names in a set and builds the client map.
int process_clients(vector <Transaction>transactions, unordered_map<string, Client> &client_map){
  for (int i = 0; i < transactions.size(); i++){
    Transaction transaction = transactions[i];
    string sender = transaction.client_sender;
    string receiver = transaction.client_receiver;
    int amount_tx = transaction.amount_tx;

    if(client_map.count(sender) != 1){
      client_map[sender] = Client(sender);
    }
    if(client_map.count(receiver) != 1){
      client_map[receiver] = Client(receiver);
    }

    Client client_sender = client_map[sender];
    Client client_receiver = client_map[receiver];
    client_sender.transfer(amount_tx);
    client_receiver.receive(amount_tx);
    client_map[sender] = client_sender;
    client_map[receiver] = client_receiver;
  }
    return 0;
}

// random server generator helper function
string random_server_generator()
{
  vector<string> servers;
  servers.push_back("serverA");
  servers.push_back("serverB");
  servers.push_back("serverC");
  int n = rand() % 3;
  return servers[n];

  // operations to handle client a and client b. the operation function includes the handling the
  // requirement from the client. (and all the operation require the main server to get data from the 3 backend server)
}
int operations(struct sockaddr_in remote_addr, socklen_t remote_size, int BUFFER_SIZE, char client[], char response[], string str, int child_socket, char client_terminal){
  sockaddr_in server_a; // data structure to store the information on serverA
  sockaddr_in server_b; // data structure to store the information on serverB
  sockaddr_in server_c; // data structure to store the information on serverC
  socklen_t server_a_len;
  socklen_t server_b_len;
  socklen_t server_c_len;
  vector<string> lines;
  string operation; // operations ("CHECK WALLET", "TXLIST", "TXCOINS")
  char result[BUFFER_SIZE];
  string failure = "unsuccessful operation.";
  string success = "successful operation";
  vector<string> inputs;
  string request = "DATA";

       parse_client_message(client, inputs, operation);
       if (inputs.size() == 1 && inputs[0] != "TXLIST"){
          cout << "The main server received input=" << client << " from the client using TCP over port " << ntohs(remote_addr.sin_port) << endl;
       }else if(inputs[0] != "TXLIST"){
         cout << "The main server recieved from " << inputs[0] << " to transfer " << inputs[2] << "coins to " << inputs[1] << " using TCP over port " << ntohs(remote_addr.sin_port) << endl;
       }

       send_to_udp_server(server_a, server_a_len, 21702, BUFFER_SIZE, request, response, "A", operation);

       process_response(str, response);
      // resets the response char * to prepare for incoming data from another udp server
       memset(response, 0, BUFFER_SIZE);

       send_to_udp_server(server_b, server_b_len, 22702, BUFFER_SIZE, request, response, "B", operation);

       process_response(str, response);
  
      // resets the response char * to prepare for incoming data from another udp server
       memset(response, 0, BUFFER_SIZE);

       send_to_udp_server(server_c, server_c_len, 23702, BUFFER_SIZE, request, response, "C", operation);

       process_response(str, response);

       char tx_response[BUFFER_SIZE];
       unordered_map<string, Client> client_map;
       vector<Transaction> transactions;
    

       if(process_transactions(str, transactions, lines) < 0){
         return -1;
       };

       if(process_clients(transactions, client_map) < 0){
         return -1;
       }

       if (operation == "CHECK WALLET")
       {
         if(process_check_wallet(inputs[0], result, client_map) < 0){
      
          strcpy(result, failure.c_str());
         }

       if (send(child_socket, result, BUFFER_SIZE, 0) < 0)
       {
         cout << "could not send back to client." << endl;
         return -1;
       }
       cout << "The main server sent the current balance to client "<< client_terminal << endl;
        // perform clean up operation (resets the transaction and client_map data structures)
       clean_up(transactions, client_map);
        // closes a socket
       close(child_socket);
       }
       else if (operation == "TXCOINS")
       {
          string to; // client reciever 
          string from; // client sender 
          string tx_amount; // amount to be transfered 
          int tx_number = transactions.size(); // transaction list sise
          string str_result; // result of transfer (to be sent to the client)
          string result_for_udp; // result of the transfer to be sent to a random udp server
          parse_client_input_for_transfer(inputs, from, to, tx_amount);
          if (process_transfer_coins(from, to, tx_amount, client_map, str_result, result_for_udp, tx_number) < 0)
          {
            strcpy(result, str_result.c_str());
            if (send(child_socket, result, BUFFER_SIZE, 0) < 0){
            cout << "could not send back to client." << endl;
            return -1;
            }
           // perform clean up operation (resets the transaction and client_map data structures)
            clean_up(transactions, client_map);
          // closes a socket
            close(child_socket);
       }else{
         // handles choosing a random UDP server to send a transaction to after a successful transaction 
         string random_server = random_server_generator();
         if(random_server == "serverA"){
           send_to_udp_server(server_a, server_a_len, 21702, BUFFER_SIZE, result_for_udp, tx_response, "A", operation);
         }else if(random_server == "serverB"){
           send_to_udp_server(server_b, server_b_len, 22702, BUFFER_SIZE, result_for_udp, tx_response, "B", operation);
         }else if(random_server == "serverC"){
           send_to_udp_server(server_c, server_c_len, 23702, BUFFER_SIZE, result_for_udp, tx_response, "C", operation);
         }
         char client_response[str_result.length()];
         strcpy(client_response, str_result.c_str());
         if (send(child_socket, client_response, BUFFER_SIZE, 0) < 0)
         {
           cout << "could not send back to client." << endl;
           return -1;
       };
        cout << "The main server sent the result of the transaction to client "<< client_terminal << endl;
       // perform clean up operation (resets the transaction and client_map data structures)
        clean_up(transactions, client_map);
      // closes the socket
         close(child_socket);
       }
       
       }
       else if (operation == "TXLIST")
       {
         // handles writing all the transactions to the "alicoins.txt" file
         ofstream myfile;
         myfile.open("alicoins.txt");
         for (int i = 0; i < transactions.size(); i++){
           myfile << *transactions[i].stringify() << endl;
         }
          myfile.close();
          strcpy(result, success.c_str());
          if (send(child_socket, result, BUFFER_SIZE, 0) < 0)
          {
            cout << "could not send back to client." << endl;
            return -1;
       };
       // perform clean up operation (resets the transaction and client_map data structures)
         clean_up(transactions, client_map);
     // closing the socket
         close(child_socket);
       }
       memset(client, 0, BUFFER_SIZE);
       return 0;
}

// main function 

int main(int argc, char* argv[]){
  struct sockaddr_in remote_addr;
  int BUFFER_SIZE = 1024;
  socklen_t remote_size;
  struct sockaddr_in local_addr;
  struct sockaddr_in local_addr_2;
  int parent_socket_A, parent_socket_B, child_socket_A, child_socket_B, udp_socket;


  // TCP socket to the client A
  local_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  local_addr.sin_port = htons(25702);
  local_addr.sin_family = AF_INET;

  // TCP socket to the client B
  local_addr_2.sin_addr.s_addr = inet_addr("127.0.0.1");
  local_addr_2.sin_port = htons(26702);
  local_addr_2.sin_family = AF_INET;



  parent_socket_A = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  parent_socket_B = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (parent_socket_A < 0){
    cout << "Error has occured in the making of parent socket" << endl;
    return -1;
  }
   if (parent_socket_B < 0){
    cout << "Error has occured in the making of parent socket" << endl;
    return -1;
  }
  int bind_result_A = ::bind(parent_socket_A, (struct sockaddr *)&local_addr, sizeof(local_addr));
  int bind_result_B = ::bind(parent_socket_B, (struct sockaddr *)&local_addr_2, sizeof(local_addr_2));

  if (bind_result_A < 0)
  {
    cout << "Error has occured in the binding of parent socket to local address." << endl;
    return -1;
  }
    if (bind_result_B < 0)
  {
    cout << "Error has occured in the binding of parent socket to local address." << endl;
    return -1;
  }


    int listen_result_A = listen(parent_socket_A, 25);
    int listen_result_B = listen(parent_socket_B, 25);
    if (listen_result_A < 0){
      cout << "Error occured while parent socket was trying to listen for incomming connections" << endl;
    return -1;
    }
    if (listen_result_B < 0){
      cout << "Error occured while parent socket was trying to listen for incomming connections" << endl;
    return -1;
    }
     cout << "The main server is up and running." << endl;
     while (true)
     {
       remote_size = sizeof(remote_addr);
       child_socket_A = accept(parent_socket_A, (struct sockaddr *)&remote_addr, &remote_size);

       if (child_socket_A < 0)
       {
         cout << "Error has occured while trying to accept connections and create child socket" << endl;
         return -1;
       }
       char client[BUFFER_SIZE];
       char response[BUFFER_SIZE];
       string str = "";

       if (recv(child_socket_A, client, BUFFER_SIZE, 0) < 0)
       {
         cout << "could not receive data from client." << endl;
         return -1;
       }
       if(operations(remote_addr, remote_size, BUFFER_SIZE, client, response, str, child_socket_A, 'A') < 0){
         return -1;
       }
       child_socket_B = accept(parent_socket_B, (struct sockaddr *)&remote_addr, &remote_size);

      if (child_socket_B < 0)
       {
         cout << "Error has occured while trying to accept connections and create child socket" << endl;
         return -1;
       }

       memset(client, 0, BUFFER_SIZE);
       memset(response, 0, BUFFER_SIZE);
       str = "";
     

       if (recv(child_socket_B, client, BUFFER_SIZE, 0) < 0)
       {
         cout << "could not receive data from client." << endl;
         return -1;
       }
       if(operations(remote_addr, remote_size, BUFFER_SIZE, client, response, str, child_socket_B, 'B') < 0 ){
         return -1;
       }
    }
}


