#include <iostream> 
#include <string>
#include <sstream>
#include <csignal>

#include <iostream>
using std::stringstream;
using namespace std;

int main(int argc, char  *argv[])
{
    int id, sig;
 
	(argv[1] - 48)>> id;
	(argv[2] - 48) >> sig;
	
    try{
        int status = kill(id, sig);
        if(status != 0){
            throw CException("Erro no sinal");
        }
    }
    catch(std::exception& e){
        cout << e.what() << '\n';
    }
    

	return 0;
}
