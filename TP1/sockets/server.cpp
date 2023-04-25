
// Server side C/C++ program to demonstrate Socket
// programming
#include <netinet/in.h>
#include <stdio.h>
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
    int server_fd, new_socket, data;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char client_message[20];
    char send_message[20];
    // string hello = "Hello from server";
  
    // Creating socket file descriptor
    try {
        // AF_ LOCAL for communication between processes on the same host. 
        // AF_INET communicating between processes on different hosts connected by IPV4, 
        // AF_I NET 6 for processes connected by IPV6.

        // SOCK_STREAM: TCP(reliable, connection oriented) SOCK_DGRAM: UDP(unreliable, connectionless)

        // Protocol value for Internet Protocol(IP), which is 0
        int server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0){
            throw runtime_error("Fail to create the socket");
        }
        
        // It helps in reuse of address and port
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));


        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);

        int socket_bind = bind(server_fd, (struct sockaddr*)&address, sizeof(address));

        if (socket_bind < 0){
            throw runtime_error("Fail to bind socket");
        }

        int lst = listen(server_fd, 3);
        
        if (lst < 0){
            throw runtime_error("Fail to listen");
        }

        while(true){
            
            int new_socket = accept(server_fd, (struct sockaddr*)&address,(socklen_t*)&addrlen);
            int is_prime;

            if(new_socket < 0){
               continue;
            }

            while(true){
                data = read(new_socket, client_message, 20);
               
                is_prime = 1;
                int random_number = atoi(client_message);
                cout << "Server recieved number: " << random_number << '\n';
                if(random_number == 0){
                    break;
                }

                if (random_number == 1) {
                    is_prime = 0;
                }
                // loop to check if n is prime
                for (int i = 2; i <= random_number/2; ++i) {
                    if (random_number % i == 0) {
                        is_prime = 0;
                        break;
                    }
                }
                
                sprintf(send_message, "%d", is_prime);
                send(new_socket, send_message, 20, 0);
            }
           

            close(new_socket);
        }
        
    } catch (exception& e){
         cout << e.what() << '\n';
    }

    return 0;
}