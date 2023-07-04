#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <queue>
#define number_of_clients 2
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
using namespace std;
int thread_ctr;
int fileinuse=0;
queue<int>pending_clients;
void communicate_with_clients(int x);



void *operations(void *arg) {
    int client_socket = *((int *)arg);
    cout << "Client connected"
         << " " << client_socket << " "
         << "Thread ID"
         << " " << pthread_self() << endl;
    while (thread_ctr!=(number_of_clients)) //to cgheck if all the clients are connected or not
    {
        continue;
    }
    
    communicate_with_clients(client_socket);
    pthread_exit(NULL);
}

void communicate_with_clients (int x)
{  
    int client_socket = x;
    char msg_buf[4096];
    memset(msg_buf,0,4096);
    while(true){
        pthread_mutex_lock(&lock); //aquiring the lock 
        int read_request_from_client = read(client_socket,msg_buf, 4096);
        if(read_request_from_client==-1)
        {
            cout<<"Error in reading message from client\n";
        }
        
        if(strcmp(msg_buf,"REQUEST")==0)
        {
            if(fileinuse==1)
            {
                pending_clients.push(client_socket); //adding pending clients in the queue
            }  
            else if (fileinuse==0)
            {
                string msg = "OK"; 
                fileinuse=1;               
                int send_response_to_client = write(client_socket, msg.c_str(), msg.size() + 1);;
                if(send_response_to_client==-1)
                {
                    cout<<"Error in sending\n";
                }
            }
        
        }
        else if (strcmp(msg_buf,"RELEASED")==0)
        {
            fileinuse=0;
            if(!pending_clients.empty()==true)
            {
                int temp=pending_clients.front();
                string msg = "OK";
                fileinuse=1;                
                int send_response_to_client = write(client_socket, msg.c_str(), msg.size() + 1); //informing client to access the shared file
                if(send_response_to_client==-1)
                {
                    cout<<"Error in sending\n";
                }
                pending_clients.pop();
            }
        }
        pthread_mutex_unlock(&lock); //releasing the lock
    }   
        
}

int main() {  
    //For socket part I have referred below video
    //https://www.youtube.com/watch?v=cNdlrbZSkyQ
    pthread_t newthread[number_of_clients];
    //Create a server socket

    int listening = 0;
    listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1) {
        cerr << "socket not created\n";
    }

    else {
        cout << "Socket created with FD: " << listening << "\n";
    }

    int reuse_address = 1;
    //Below code is referred from: https://pubs.opengroup.org/onlinepubs/000095399/functions/setsockopt.html
    //To reuse the port
    if (setsockopt(listening, SOL_SOCKET, SO_REUSEPORT, &reuse_address, sizeof(reuse_address)) != 0) {
        cout << "Failed to reuse the port" << endl;
    }

    //Bind socket on ip & port
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54004);
    inet_pton(AF_INET, "127.0.0.1", &hint.sin_addr);

    if (bind(listening, (sockaddr *)&hint, sizeof(hint)) == -1) {
        cerr << "Binding failed\n";
    }

    //Make the socket listen
    if (listen(listening, 5) == -1) {
        cerr << "Listening failed\n";
    }

    //accpet the connection
    sockaddr_in client;
    socklen_t clientsize = sizeof(client);
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];
    int clientsocket[number_of_clients];
    for (int j =0; j < number_of_clients; j++) {
        clientsocket[j] = 0;
    }
    thread_ctr=0;
    while (true) {
        while (clientsocket[thread_ctr] = accept(listening, (struct sockaddr *)&client, (socklen_t *)&clientsize)) {
            if (clientsocket[thread_ctr] == -1) {
                cerr << "Unable to connect with client\n";
                continue;
            } else {
                pthread_create(&newthread[thread_ctr], NULL, operations, &clientsocket[thread_ctr]);
                thread_ctr++;
                cout<<"Thread counter: "<<thread_ctr<<endl;
            }
        }
        for(int p = 0; p < number_of_clients; p++)
                {
                    pthread_join(newthread[p], NULL);       //waiting for all threads to finish                    
                }
            cout << "closing " << clientsocket << endl;
    }
    
    return 0;
}