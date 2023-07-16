
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

#include <chrono>
#include <ctime>
#include <thread>
#include <iostream>

using namespace std;

string CurrentTime()
{   
    cout << "begin CurrentTime" << endl;
    auto now = chrono::system_clock::now();
    auto now_ms = chrono::time_point_cast<chrono::milliseconds>(now);
    auto value = now_ms.time_since_epoch().count();

    time_t current_time = chrono::system_clock::to_time_t(now);
    string time_string = ctime(&current_time);

    // Remover o caractere de nova linha da string
    time_string.erase(time_string.length() - 1);
    
    tm timeInfo;
    strptime(time_string.c_str(), "%a %b %e %H:%M:%S %Y", &timeInfo);
    char timeString[9];
    strftime(timeString, sizeof(timeString), "%T", &timeInfo);
    string timeFormatted(timeString);

    // Converter o valor dos milissegundos para string
    string ms = to_string(value % 1000);

    // Formatando a string com milissegundos
    timeFormatted += ":" + string(3 - ms.length(), '0') + ms;
    cout << "end CurrentTime" << endl;
    return timeFormatted;
}

void WhriteResult(int k)
{
    cout << "begin white" << endl;
    string fileName = "resultado.txt";

    // Abrir o arquivo em modo append
    ofstream outputFile(fileName, ios::app);

    if (!outputFile)
    {
        cout << "Error opening file." << endl;
        return;
    }

    // Escrever a hora atual com milissegundos no arquivo
    outputFile << pthread_self()<< ' '  << CurrentTime() << endl;

    // Fechar o arquivo
    outputFile.close();

    // Aguardar k segundos
    this_thread::sleep_for(chrono::seconds(k));
    cout << "end white" << endl;
}

int main(int argc, char* argv[])
{   
    //read var's process 
    int r = atoi(argv[1]);
    int k = atoi(argv[2]);
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
    while (r > 0)
    {   
        cout << r <<"\n";
        string msg = "1|"+ to_string(pthread_self()) + '|';
                
        int send_resquest_to_server = write(csock, msg.c_str(), msg.size() + 1);
        cout << "Send the resquest" <<"\n";
        if(send_resquest_to_server==-1)
        {
            cout<<"Error in sending\n";
        }
        char buf[4096];
        memset(buf,0,4096);
        cout << "Whait for server message" <<"\n";
        int recvmsg = recv(csock, buf, 4096,0); //receive message from server
        cout << "buf : "<< buf[0] <<"\n";
        if(buf[0] == '2') //if server sends ok then proceed with the file operations == GRANT
        {   
            cout << "reseave GRANT" <<"\n";
            WhriteResult(k);       
            string msg1 = "3|"+to_string(pthread_self()) + '|';
            int send_response_to_server = write(csock, msg1.c_str(), msg1.size() + 1);;
                if(send_response_to_server==-1)
                {
                    cout<<"Error in sending\n";
                }
        }
    r--;
    }
  

    close(csock);
    
    return 0;
}
