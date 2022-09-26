#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <iostream>
#include <stdio.h>
#include <string.h>

// the transaction data structure
class Transaction{
public:
  int transaction_number; // transaction number (used to sort the transactions)
  std::string client_sender;  // string represenation of a sender name
  std::string client_receiver; // string representation of the reciever name
  int amount_tx; // transaction amount

public:
  Transaction(int tx_number, std::string sender, std::string reciever, int amount);
  int getSender(std::string &sender);
  int getReceiver(std::string &receiver);
  std::string* stringify();
  std::string to_string();
  int getAmount();
};

#endif
