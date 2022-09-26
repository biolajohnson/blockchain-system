# include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

/* reads from dedicated block text file to retriev information about  all the
* trasactions this server os responsible for. @returns a string.*/
string readfile(){
  string line;
  int n = 0;
  string str;
  fstream file("block1.txt");
  if(file.is_open()){
    while(file){
      getline(file, line);
			if(line.length() > 1){
      n += line.length() - 1;
      str += line + ',';
			}
    }
  }
  return str;
}
// parses the main server's request. helps to know if its a "save transaction request"
// or a "i want data" request. 
void tokenize(string const &str, const char delim, vector<string> &out){
    size_t start;
    size_t end = 0;
 
    while ((start = str.find_first_not_of(delim, end)) != string::npos){
        end = str.find(delim, start);
        out.push_back(str.substr(start, end - start));
    }
}

// converts the transaction the main server sent into a string an appends it to the file/block1.txt
int process_and_save_transaction_entry(char*command){
	string line(command);
	ofstream myfile;
	myfile.open("block1.txt", ios::app);  // opens the text file to append to it.
	myfile << line << endl;
	myfile.close();
	return 0;
}

int main(int argc, char* argv[]){
	int BUFFER_SIZE = 1024; // default buffer size
	struct sockaddr_in local_addr; // structure to store information about this udp server
	int socket_desc; // the socket file descriptor
	struct sockaddr_in remote_addr; // structure to store information about the remote server (main server)
	socklen_t remote_addr_size; // size of the remote server structure 
	char client_message[BUFFER_SIZE]; // main server buffer 
	char server_message[BUFFER_SIZE]; // this server's buffer

	// setting the the properties (attributes) of this server
	local_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(21702); // port number
	

	// creating the socket
	socket_desc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	// check for socket creation error
	if (socket_desc < 0)
	{
		cout << "Error has occured in the creation of socket" << endl;
		return -1;
	}
	// binding the socket to the local address (port number)
	if (::bind(socket_desc, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0)
	{
		cout << "unable to bind socket to local address" << endl;
		return -1;
	};

	int port_no = ntohs(local_addr.sin_port);
	cout << "The ServerA is up and running using UDP on port " << port_no << endl;

	// infinite loop (until ended manually)
	while(true){
		remote_addr_size = sizeof(remote_addr);

		// waiting to recieve from the main server
		if(recvfrom(socket_desc, client_message, BUFFER_SIZE, 0, (struct sockaddr *)&remote_addr, &remote_addr_size) < 0){
			cout << "Could not recieve message from client " << endl;
			return -1;
		}
		cout << "The ServerA received a request from the Main Server" << endl;

		// parsing the main_server message
		char delim = ' ';
		string str(client_message);
		vector<string> request_list;
		tokenize(str, delim, request_list);
			if (request_list.size() > 1){
				process_and_save_transaction_entry(client_message);
				if (sendto(socket_desc, "success", BUFFER_SIZE, 0, (struct sockaddr *)&remote_addr, remote_addr_size) < 0){
					cout << "could not send message" << endl;
					return -1;
				}

					cout << "The ServerA finished sending the response to the Main Server" << endl;
			}
			
			else
			{
				string lines = readfile();
				int len = lines.length();
				char file_lines[len];
				strcpy(file_lines, lines.c_str());
				if (sendto(socket_desc, file_lines, BUFFER_SIZE, 0, (struct sockaddr *)&remote_addr, remote_addr_size) < 0)
				{
					cout << "could not send message" << endl;
					return -1;
				}
						cout << "The ServerA finished sending the response to the Main Server" << endl;
			}
			
			// reset the main server buffer
			memset(client_message, 0, BUFFER_SIZE);
	}
}

