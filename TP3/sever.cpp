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
#include <map>
pthread_mutex_t client_queue = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t client_map = PTHREAD_MUTEX_INITIALIZER;

using namespace std;
int fileinuse=0;
int thread_ctr=0;
int listening=0;
int go_on = 1;
vector<pthread_t>threads;
vector<int>clientsocket;
queue<int>pending_clients;
map<int,int> served_clients;


void generate_message(int code, int id, char message[10]) {
    string mes = to_string(code) + '|' + to_string(id) + '|';
    int len = mes.size();
    if (10 - len > 0){
        mes = mes.append(string( 10 - len, '0'));
    }
    strcpy(message, mes.c_str());

}



void request(int client_socket){

    pthread_mutex_lock(&client_queue);
    pending_clients.push(client_socket); //adding pending clients in the queue
    pthread_mutex_unlock(&client_queue);

    while(pending_clients.front() != client_socket && go_on == 1);

    if (go_on == 0){
        return;
    } else {
        char grant_msg[10];
        generate_message(2, client_socket, grant_msg);
        int send_response_to_client = send(client_socket, grant_msg, 10, 0);

        pthread_mutex_lock(&client_map); 
        if (served_clients.find(client_socket) != served_clients.end()){
            served_clients[client_socket] += 1; 
        } else {
            served_clients[client_socket] = 1;
        }
        pthread_mutex_unlock(&client_map);
    }
}

void release(){

    pthread_mutex_lock(&client_queue);
    pending_clients.pop(); //adding pending clients in the queue
    pthread_mutex_unlock(&client_queue);
}



void *communicate_with_clients (void *arg)
{  
    int client_socket = *((int *)arg);
    char msg_buf[10];
    while(go_on == 1){
        int read_request_from_client = read(client_socket,msg_buf, 10);

        if(read_request_from_client==-1){

            cout <<"Error in reading message from client\n";

        } else if (read_request_from_client == 0){

            break;

        } else if(msg_buf[0] == '1'){
            request(client_socket);
        }
        else if (msg_buf[0] == '3'){

            release();
        }
    }   
    cout << "Client " << client_socket <<  " Leaving" << '\n';
    pthread_exit(NULL);
    
}



int open_socket(){
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
    return listening;
}


void *thread_creator(void* x){
    sockaddr_in client;
    socklen_t clientsize = sizeof(client);
    while (go_on == 1) {
        int new_socket =  accept(listening, (struct sockaddr *)&client, (socklen_t *)&clientsize);
        if (new_socket < 0) {
            continue;
        }
        clientsocket.push_back(new_socket);
        pthread_t new_thread;
        threads.push_back(new_thread);
        cout << "Client " << thread_ctr  << " connected" << '\n';
        pthread_create(&threads[thread_ctr], NULL, communicate_with_clients, &clientsocket[thread_ctr]);
        thread_ctr++;
       
    }
    cout << "Creator Leaving" << '\n';
    pthread_exit(NULL);
}


int main() {  
    listening = open_socket();
    pthread_t creator_thread;
    pthread_create(&creator_thread, NULL, thread_creator, NULL);
    cout << "Press 1 to see the threads queau of requests" << '\n';
    cout << "Press 2 to end the coordinator" << '\n';
    cout << "Press 3 to see how many times each processor was served" << '\n';

    int input;
    while (true){
        cin >> input;
        if(input == 2){
            go_on = 0;
            shutdown(listening, SHUT_RDWR);
            for(int p = 0; p < threads.size(); p++){
                    pthread_join(threads[p], NULL);
                    close(clientsocket[p]);     //waiting for all threads to finish                    
            }
            pthread_join(creator_thread, NULL);
            cout << "Ended threads" << '\n';
            exit(0);

        } else if (input == 1){
            cout << "Fila de Pedidos" << '\n';
            pthread_mutex_lock(&client_queue);
            int queau_size = pending_clients.size();
            for(int i = 0; i < queau_size; i++){
                int front = pending_clients.front();
                cout << "Pedido " << i << ": " << front << '\n';
                pending_clients.pop();  
                pending_clients.push(front);
            }
            pthread_mutex_unlock(&client_queue);

        } else if (input == 3){
            for(const auto& elem : served_clients){
                cout << "Client " << elem.first << " was served " << elem.second << " times \n";
            }
        }
    }

  
    
    return 0;
}