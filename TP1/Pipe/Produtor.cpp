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

void consumidor(int file){
    int received;
    char cnum;
    string current = "";

    FILE *pipe = fdopen(file, "r");
    if(pipe == NULL){
        printf("Error opening pipe\n");
    }
    while(1) {
        received = fscanf(pipe, "%c", &cnum);
        if(received >= 1){
            if (cnum == '0'){
            
                printf("Consumer received 0! Finishing process \n");
                break;
            }
            else if (cnum == ' '){
                if (primo(stoi(current)))
                {
                    printf("%s -> Prime number\n", current.c_str());
                }
                else
                {
                    printf("%s -> Not a prime number\n", current.c_str());
                }
                current = "";
            }else{
                current.push_back(cnum);

            }}else{
                printf("Empty Pipe\n");
            }
    }
    fclose(pipe);
}
	  
  
void produtor( int inter, int file){
    FILE *pipe;
    pipe = fdopen(file,"w");

    int N0 = 1;
    int delta, N;
    int sent = 0;

    unsigned seed = time(0);
    srand(seed); 
    delta = 1 + rand()%100;

    for(int i=0; i < inter; i++){
        N = N0 + delta;
        string pd = to_string(N) + " " ;
        fprintf(pipe, "%s", pd.c_str());
        printf("Producer send:  %d\n", N);
        N0 = N;
    };

    fprintf(pipe, "%d", 0);
    fclose(pipe);
}

	

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

    if(pid == (pid_t)0) { //child
        close(pd[1]);
        consumidor(pd[0]);
        return EXIT_SUCCESS;
    } else{ //parent
        close(pd[0]);
        produtor(atoi(argv[1]), pd[1]);
        waitpid(pid, NULL, 0);
        return EXIT_SUCCESS;
    }
}


	
