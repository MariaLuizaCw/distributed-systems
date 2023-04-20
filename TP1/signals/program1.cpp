#include <csignal>
#include <iostream>
using namespace std;
  
void signal_handler(int signal_num)
{
    
	if(signal_num == 1){
		// It terminates the  program
    	exit(signal_num);
	}else if (signal_num == 2){
		cout << "The interrupt signal is (" << signal_num
				<< "). \n";
	}
    
}
  
int main(int argc, char const *argv[])
{
    // register signal SIGABRT and signal handler
    signal(SIGINT, signal_handler);
  
    while (true);
        
    return 0;
}
	
