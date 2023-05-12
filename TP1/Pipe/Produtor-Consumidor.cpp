#include <cstdlib>
#include <ctime>
#include <iostream>
#include <array>
#include <cstdio>
#include <unistd.h>
#include <sstream>
#include <vector>
#include <sys/wait.h>
#include <string>

using namespace std;

bool primo(int n){
    if (n % 2 == 0 || n <= 1) return false;
    
    for (int i=3; i<n/2; i=i+2){
        if (n % i == 0){
            return 0;
        }
    }
    return true;
}

void consumidor(int pipe_rd){
    int received= 1;
    string current = "";

    while(received != 0) {
        if (primo(received)){
                printf("Prime number received from child process: %d\n", received);
            }
        read(pipe_rd, &received, sizeof(received));
        }

}
	  
  
void produtor( int inter, int pipe_wr){
    int N0 = 1;
    int N;
    int sent = 0;

    unsigned seed = time(0);
    srand(seed); 
    int delta = 0;

    for(int i=0; i < inter; i++){
        N = N0 + delta;
        delta = 1 + rand()%100;
        write(pipe_wr, &N, sizeof(N));
        printf("Producer send:  %d\n", N);
        N0 = N;
    }; 
};

	

int main(int argc, char* argv[]){
    pid_t pid;  // Store PID number
    int pd[2]; // File descriptors for Pipe 
    
    if(pipe(pd) == -1){
        printf("Pipe Error\n");
        return EXIT_FAILURE;
    }
    
    pid = fork();
    if(pid == -1 ){
        printf("Error creating child process.\n");
        return EXIT_FAILURE;
    }

    if(pid == (pid_t)0) { //child -> producer 
        close(pd[0]);
        produtor(atoi(argv[1]), pd[1]);
        int N = 0;
        write(pd[1], &N , sizeof(N));        
        close(pd[1]);

    } else{ //parent -> consumer
        close(pd[1]);
        consumidor(pd[0]);
        printf("Consumer terminated\n");
        close(pd[0]);
    }
    return 0;
}


	
