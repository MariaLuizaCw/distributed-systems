#include <iostream> 
#include <string>
#include <sstream>
#include <csignal>
using std::stringstream;
using namespace std;

int main(int argc, char  *argv[])
{
    int id, sig;
    id  = atoi(argv[1]);
    sig = atoi(argv[2]);

    cout << "Sending Signal " << sig << " to PID " << id << '\n';

    try {
        int status = kill(id, sig);
        if(status != 0){
            string error_message = "Program with PID " +  std::to_string(id) + " not found";
            throw std::invalid_argument(error_message);
        }
    } catch(exception& e){
        cout << e.what() << '\n';
    }
 
 
	return 0;
}
