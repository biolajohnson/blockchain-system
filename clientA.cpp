# include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>

using namespace std;


// parses the clients request to the main server and prints invalid input if input is passed. 

int prepare_request_for_server(int argc, char* argv[], char* &request){
  if (argc == 2){
  request = argv[1];
  
  }else if(argc > 2){
    int length1 = strlen(argv[1]);
    int length2 = strlen(argv[2]);
    int length3 = strlen(argv[3]);

    char result[length1 + length2 + length3 + 2];
    memset(result, 0, sizeof(result));
    strcpy(result, argv[1]);
    result[length1] = ' ';
    strcat(result, argv[2]);
    result[length1 + length2 + 1] = ' ';
    strcat(result, argv[3]);
    request = result;
  }
  else
  {
    cout << "invalid input" << endl;
    return -1;
  }

  return 0;
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


// processes the response from a server (UDP). It concatenates the response to "str". returns 0 when successful and -1 in error. 
int process_response(string &str, char*response){
  int i = 0;
  while (response[i] != 0)
  {
    str += response[i];
    i++;
  }
  return 0;
}

int main(int argc, char* argv[]){
  char *request;

  if(prepare_request_for_server(argc, argv, request) < 0){
    return -1;
  };
  int socket_desc;
  struct sockaddr_in remote_addr;
  int BUFFER_SIZE = 1024;
  char buf[BUFFER_SIZE];

  remote_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  remote_addr.sin_port = htons(25702);
  remote_addr.sin_family = AF_INET;

  socket_desc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (socket_desc < 0){
    cout << "Error has occured in the creation of client's socket" << endl;
    return -1;
  }
   // connecting to the main server
  if(connect(socket_desc, (struct sockaddr*)&remote_addr, sizeof(remote_addr)) < 0){
    cout << "can not connect to TCP server." << endl;
    return -1;
  }

  cout << "The client A is up and running." << endl;

  if (send(socket_desc, request, strlen(request), 0) < 0)
  {
    cout << "could not send message" << endl;
    return -1;
  }
  if(argc == 2 && strcmp(argv[1], "TXLIST") != 0){
    cout << argv[1] << " sent a balance enquiry request to the main server " << endl;
  }else if(strcmp(argv[1], "TXLIST") == 0){
    cout << "clientA sent a sorted list request to the main server" << endl;
  }else if(argc > 2){
    cout << argv[1] << " has requested to transfer " << argv[3] << " alicoins to " << argv[2] << endl;
  }else if(argc == 3){
    cout << argv[1] << " sent a statictics enquiry request to the main server " << endl;
  }

  if (recv(socket_desc, buf, BUFFER_SIZE, 0) < 0){
    cout << "could not receive message" << endl;
    return -1;
  }
  string resp = "";
  vector<string> response;
  process_response(resp, buf);
  tokenize(resp, ',', response);
  if (argc > 2)
  {
    if (response[0] == "success"){
      cout << argv[1] << " successfully transferred " << argv[3] << " alicoins to " << argv[2] << "\n"
         << response[1] << endl;
    }else if (response[0] == "funds"){
      cout << argv[1] << " was unable to transfer " << argv[3] << " alicoins to " << argv[2] << " because of insufficient balance.\n" << response[1] << endl;  
    }else if (response.size() == 1){
      cout << argv[1] << " was unable to proceed with transaction as " << response[0] << " is not part of the network" << endl; 
    }else{
      cout << argv[1] << " was unable to proceed with transaction as " << response[0] << " and " << response[1] << " are not part of the network" << endl;
    }
  }
   else
  {
    if(strcmp(buf, "unsuccessful operation") == 0){
      cout << argv[1] << " is not part of this network" << endl;
    }else{
    cout << buf << endl;
    }
  }
  // closes the socket
  close(socket_desc);
}