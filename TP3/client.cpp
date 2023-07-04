#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string.h>
using namespace std;

int main()
{   
    //creating client socket
    int csock = socket(AF_INET, SOCK_STREAM,0);
    if (csock == -1)
    {
        cerr << "socket not created\n";
    }

    int port = 54004;
    string IP = "127.0.0.1";
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, IP.c_str(), &hint.sin_addr);

    //connecting to the server
    int connreq = connect(csock,(sockaddr *)&hint, sizeof(sockaddr_in));
    //send to server
    string msg = "REQUEST";
                
        int send_resquest_to_server = write(csock, msg.c_str(), msg.size() + 1);;
        if(send_resquest_to_server==-1)
        {
            cout<<"Error in sending\n";
        }
    char buf[4096];
        memset(buf,0,4096);
        int recvmsg = recv(csock, buf, 4096,0); //receive message from server
    if(strcmp(buf,"OK")==0) //if server sends ok then proceed with the file operations
    {
        fstream file1;
        string first_item;
        file1.open("shared_file.txt",ios::out | ios::in);
        getline(file1,first_item);
        file1.close(); 
        cout<<"Value read from shared file: "<<first_item<<endl;
        int temp = stoi(first_item);
        temp++;
        ofstream file2;
        file2.open("shared_file.txt",ios::out | ios::in);
        file2<<to_string(temp);
        file2.close(); 
        cout<<"Updated value: "<<temp<<endl;        
        string msg1 = "RELEASED";
        int send_response_to_server = write(csock, msg1.c_str(), msg1.size() + 1);;
        if(send_response_to_server==-1)
        {
            cout<<"Error in sending\n";
        }
       
    }
  

    close(csock);
    
    return 0;
}
