#include "client.h"
#include "transaction.h"

// the constructor for the client
Client::Client(){}
Client::Client(std::string const name){
  client_name = name;
  amount_left = new int(1000);
}
// the transfer function from the client
int Client::transfer(int amount){
  int p = *amount_left;
  if (p >= amount)
  {
    p -= amount;
    amount_left = new int(p);
    return 0;
  }
  return -1;
}
// recieves coins from a client and adds to the client amount. 
int Client::receive(int amount){
  int p = *amount_left;

  p += amount;
  amount_left = new int(p);
  return 0;
}
// adds to the transaction list
int Client::add_to_transaction_list(Transaction transaction){
  Client::transaction_list.push_back(transaction);
  return 0;
}
// converts the client object to a string format
std::string Client::to_string(){
  return "The current balance of " + client_name + " is " + std::to_string(*amount_left) + " alicoins";
}