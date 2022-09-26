#ifndef CLIENT_H
#define CLIENT_H
#include "transaction.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>


// the client data structure to store client information such as, client name, amount left.
// this class also has ability to transfer coins and recieve coins from other clients

class Client{
  std::string client_name;
  int* amount_left;
  public:
    std::vector<Transaction> transaction_list;
    Client();
    Client(std::string const name);
    int transfer(int amount);
    int receive(int amount);
    int add_to_transaction_list(Transaction transaction);
    std::string to_string();
};

#endif
