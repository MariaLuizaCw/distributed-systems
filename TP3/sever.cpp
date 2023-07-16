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
pthread_mutex_t client_queue = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t file = PTHREAD_MUTEX_INITIALIZER;

using namespace std;
int fileinuse=0;
int thread_ctr=0;
int listening=0;
int go_on = 1;
vector<pthread_t>threads;
vector<int>clientsocket;
queue<int>pending_clients;



void generate_message(int code, int id, char message[10]) {
    string mes = to_string(code) + '|' + to_string(id) + '|';
    int len = mes.size();
    if (10 - len > 0){
        mes = mes.append(string( 10 - len, '0'));
    }
    strcpy(message, mes.c_str());

}

int readmessage(const char* message){
    string msg = message;
    string message_cut =  msg.substr(2); 

    int  position_second = message_cut.find("|");
    int pid = stoi(message_cut.substr(0,position_second));
    cout << pid << "\n";
    return pid;
}

void request(int pid,int client_socket){

    pthread_mutex_lock(&client_queue);
    pending_clients.push(pid); //adding pending clients in the queue
    cout << "Front " << pending_clients.front()  << '\n';
    pthread_mutex_unlock(&client_queue);

    while(pending_clients.front() != pid && go_on == 1){
        cout << "Client " << pid << " waiting to be front" << '\n';
        sleep(1);
    }

    if (go_on == 0){
        return;
    } else {
        char grant_msg[10];
        generate_message(2, pid, grant_msg);
        cout << grant_msg << '\n';
        int send_response_to_client = send(client_socket, grant_msg, 10, 0);
    }
}

void release(int pid){

    pthread_mutex_lock(&client_queue);
    cout << "Front Before " << pending_clients.front()  << '\n';
    pending_clients.pop(); //adding pending clients in the queue
    cout << "Front After " << pending_clients.front()  << '\n';
    pthread_mutex_unlock(&client_queue);
}



void *communicate_with_clients (void *arg)
{  
    int client_socket = *((int *)arg);
    cout << "Client connected"
         << " " << client_socket << " "
         << "Thread ID"
         << " " << pthread_self() << endl;

    char msg_buf[10];
    while(go_on == 1){
        cout << "Waiting for client " << client_socket <<  " messages" << '\n';
        int read_request_from_client = read(client_socket,msg_buf, 10);
        cout << "Client " << client_socket << " send " << msg_buf << '\n';

        if(read_request_from_client==-1){

            cout <<"Error in reading message from client\n";

        } else if (read_request_from_client == 0){

            break;

        } else if(msg_buf[0] == '1'){
            int pid = readmessage(msg_buf);
            request(pid,client_socket);
        }
        else if (msg_buf[0] == '3'){
            int pid = readmessage(msg_buf);
            release(pid);
        }
    }   

    // char leave_msg[10];
    // generate_message(4, client_socket, leave_msg);
    // cout << leave_msg << '\n';
    // int send_response_to_client = send(client_socket, leave_msg, 10, 0);
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
        cout << "counter " << thread_ctr << '\n';
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
            cout << "ended threads" << '\n';
            exit(0);
        } else if (input == 1){
            cout << "teste1" << '\n';
        } else if (input == 3){
            cout << "teste3" << '\n';
        }
    }

  
    
    return 0;
}