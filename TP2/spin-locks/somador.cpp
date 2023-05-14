#include <bits/stdc++.h>
#include <iostream>
#include <cmath>
#include <thread>
#include <time.h>
#include <algorithm>
using namespace std;



//Test and Set implementation
struct ttas_lock {
    atomic_flag lock = ATOMIC_FLAG_INIT;

    void acquire(){
        while(lock.test_and_set());
    }

    void release(){
        lock.clear();
    }
};


// Global variable
long threads_sum;
int k, n;
ttas_lock spinlock;
vector<int8_t>numbers;

//Threads function to add
void Add(int i){

    int mod = n%k;
    int part = (n - mod)/k;
    int start = part*i; 
    int end = i == k-1 ? part*i + part + mod : part*i + part;

    int thread_result = 0;


    for (int i = start; i < end; i++){
        thread_result += numbers[i];
    }

    spinlock.acquire();
    threads_sum += thread_result;
    spinlock.release();
    
}




int main(int argc, const char *argv[]){
    long check_sum = 0;
    unsigned seed = time(0);
    threads_sum = 0;
    n = pow(10, atoi(argv[1]));
    k =  atoi(argv[2]);
    
    numbers.resize(n);
    
    cout << n << " numbers will be added by " << k << " threads" << "\n";

    // Generate Random vector
    srand(seed);
	for (int i =0; i < n; i++){
		numbers[i] =  (rand()%201 - 100); 
    } 

    // Spawn Threads
    vector<thread> threads(n);
    for (int i = 0; i < k; i++) {
        threads[i] = thread(Add, i);
    }
    for (int i = 0; i < k; i++) {
        threads[i].join();
    }


    //Check Sum
    for (int i =0; i < n; i++){
		check_sum +=  numbers[i]; 
    } 

    try{
        cout << "Result "<< check_sum << ' ' << threads_sum << '\n';
        if (check_sum != threads_sum){
            throw runtime_error("Results are not the same!");
        } else {
            cout << "Numbers were succesfuly added by threads!" << "\n";
        }
    } catch (exception& e){
         cout << e.what() << '\n';
    }


}
