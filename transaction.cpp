#include "transaction.h"
#include "client.h"

// the transaction constructor
Transaction::Transaction(int tx_number, std::string sender, std::string receiver, int amount){
  transaction_number = tx_number;
  client_sender = sender;
  client_receiver = receiver;
  amount_tx = amount;
}
// gets the sender of a transaction object
int Transaction::getSender(std::string &sender){
  sender = client_sender;
  return 0;
}
// gets the reciever of a transaction object
int Transaction::getReceiver(std::string &receiver){
  receiver = client_receiver;
  return 0;
}
// gets the amount of a transaction object
int Transaction::getAmount(){
  return amount_tx;
}
// transforms the transaction object to a string (to save in the block file)
std::string* Transaction::stringify(){
  std::string* p = new std::string(std::to_string(transaction_number) + " " + client_sender + " " + client_receiver + " " + std::to_string(amount_tx));
  return p;
}
// transforms the transaction obect to a string (to be readable for tests)
std::string Transaction::to_string(){
  return client_sender + "--> "  + client_receiver + " => " + std::to_string(amount_tx);
}

