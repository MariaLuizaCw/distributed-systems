#include <csignal>
#include <unistd.h>
#include <iostream>
#include <cstring>
using namespace std;
  
void signal_handler_1(int signal_num)
{	

	cout << "First type of signal (" << signal_num << "). Interromping process! \n";
	exit(2);
}

void signal_handler_2(int signal_num)
{	
	cout << "Second type of signal (" << signal_num << "). \n";
}

void signal_handler_3(int signal_num)
{	

	cout << "Third type of signal (" << signal_num << "). \n";
	
}
  
int main(int argc, const char *argv[])
{
	string wait_format;
	wait_format = argv[1];
	cout << "Current PID of Process: " << getpid() << '\n';
	cout << "Waiting for Signals: " << SIGINT << ", " << SIGALRM << " or " << SIGUSR1 << '\n';
	
	signal(SIGINT, signal_handler_1);
	signal(SIGALRM, signal_handler_2);
	signal(SIGUSR1, signal_handler_3);

	try {
		if (wait_format == "blocking"){
			while(true){
				sleep(5);
			}
		} else if (wait_format == "busy") {
			while (true);
		} else {
			throw std::invalid_argument("You must specify type of wait:  busy or blocking?");
		}
	}  catch(exception& e){
        cout << e.what() << '\n';
    }

   
    return 0;
}
	
