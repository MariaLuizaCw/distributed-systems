
// Client side C/C++ program to demonstrate Socket
// programming
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <cstdlib>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream> 
#include <string>
#include <sstream>
#include <csignal>
#define PORT 8080
  
using namespace std;
int main(int argc, char const* argv[])
{
    int status, data, client_fd, delta;
    struct sockaddr_in serv_addr;
    char incoming_message[20];
    char send_message[20];

    int n_numbers = atoi(argv[1]);
    int random_num = 0;
    try {
        client_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (client_fd < 0) {
            throw runtime_error("Fail to create the socket");
        }
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);
  
        // Convert IPv4 and IPv6 addresses from text to binary
        // form
        if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
            throw runtime_error("Fail to convert address");
        }
    
        status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        
        if (status < 0) {
                throw runtime_error("Fail to connect");
        }
        
        while(n_numbers--){
            
            
            random_num = random_num + (rand()%10);
            cout << "Random number: " << random_num << '\n';
            
            sprintf(send_message, "%d", random_num);

            send(client_fd, send_message, 20, 0);
            data = read(client_fd, incoming_message, 20);
            while (data < 0);
            cout << incoming_message << '\n';
        }

          
        sprintf(send_message, "%d", 0);
        send(client_fd, send_message, 20, 0);


        // closing the connected socket
        close(client_fd);
    } catch (exception& e){
         cout << e.what() << '\n';
    }

  
  
    return 0;
}