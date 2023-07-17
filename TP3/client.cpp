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
#include <chrono>
#include <ctime>
#include <thread>

using namespace std;

string CurrentTime()
{   
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
    return timeFormatted;
}

void generate_message(int code, int id, char message[10]) {
    string mes = to_string(code) + '|' + to_string(id) + '|';
    int len = mes.size();
    if (10 - len > 0){
        mes = mes.append(string( 10 - len, '0'));
    }
    strcpy(message, mes.c_str());

}

void WhriteResult(int k, string test)
{
    string fileName = "./results/resultado_" + test + ".txt";

    // Abrir o arquivo em modo append
    ofstream outputFile(fileName, ios::app);

    if (!outputFile)
    {
        cout << "Error opening file." << endl;
        return;
    }

    // Escrever a hora atual com milissegundos no arquivo
    outputFile << getpid()<< ' '  << CurrentTime() << endl;

    // Fechar o arquivo
    outputFile.close();

    // Aguardar k segundos
    this_thread::sleep_for(chrono::seconds(k));
}

int main(int argc, char* argv[])
{   
    //read var's process 
    int r = atoi(argv[1]);
    int k = atoi(argv[2]);
    string test = argv[3];
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
    for(int i =0; i < r; i++){

        char request_msg[10];
        generate_message(1, getpid(), request_msg);

        int send_resquest_to_server = send(csock, request_msg, 10, 0); 

        char buf[10];
        int recvmsg = read(csock, buf, 10); //receive message from server
        if(buf[0] == '2') //if server sends ok then proceed with the file operations == GRANT
        {   
            WhriteResult(k, test);       
            // sleep(4);
            
            char release_msg[10];
            generate_message(3, getpid(), release_msg);

            int send_response_to_server = send(csock, release_msg, 10, 0);;
            if(send_response_to_server==-1)
            {
                cout<<"Error in sending\n";
            }
        }
    }

    close(csock);
    
    return 0;
}
