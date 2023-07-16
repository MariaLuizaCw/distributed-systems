#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <queue>
using namespace std;

void generate_message(int code, int id, char message[10]) {
    string mes = to_string(code) + '|' + to_string(id) + '|';
    int len = mes.size();
    if (10 - len > 0){
        mes = mes.append(string( 10 - len, '0'));
    }
    strcpy(message, mes.c_str());

}
//     if (len >= width) strcpy(dest, src);
// //   else 

int main()
{   
    char request_msg[10];
    generate_message(1, 20, request_msg);
    cout << request_msg << '\n';
    // char request_msg[10];

    
    // sprintf(request_msg, "1|1|%0*d%s", 1, 10);
    // cout << request_msg << '\n';

    return 0;
}