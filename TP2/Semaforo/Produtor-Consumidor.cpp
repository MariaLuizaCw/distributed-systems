#include <iostream>                      
#include <bits/stdc++.h>
#include <semaphore.h>
#include <thread>
#include <queue>
#include <time.h>

using namespace std;


sem_t full;
sem_t empty_mem;
int num_val_processar = 100000;
int tamanho_fila;
sem_t mutex_fila;
sem_t mutex_encerramento;

vector<int> memoria;


int is_prime(int n)
{
    if (n % 2 == 0 || n <= 1) return 0;

    for (int i=3; i<n/2; i=i+2)
    {
        if (n % i == 0) return 0;
    }
    return 1;
}


void produtor()
{
    unsigned seed = time(0);
    srand(seed);
    int numMax = pow(10, 7) - 1;    

    while(num_val_processar > 0)
    {
        int valor_aleatorio = 1 + rand() % numMax;
        sem_wait(&empty_mem);
        sem_wait(&mutex_fila);
            auto posicao = find(memoria.begin(), memoria.end(), 0);
            if (posicao != memoria.end()) {
                int pos = posicao - memoria.begin();
                memoria[pos] = valor_aleatorio;
            }
        sem_post(&mutex_fila);
        sem_post(&full);
    }
    sem_post(&full);
    sem_post(&empty_mem);
}


void consumidor()
{   
    int valor_recolhido;
    while(num_val_processar > 0) {
        sem_wait(&full);
        sem_wait(&mutex_fila);
            auto posicao = find_if(memoria.begin(), memoria.end(),[](int x) { return x != 0; });
            if (posicao != memoria.end()) {    
                int pos = posicao - memoria.begin();
                valor_recolhido = memoria[pos];
                memoria[pos] = 0;
            }    
        sem_post(&mutex_fila);
        sem_post(&empty_mem);

        sem_wait(&mutex_encerramento);
        if(is_prime(valor_recolhido)){
            cout << valor_recolhido << " é primo\n";
        }else{
            cout << valor_recolhido << " não é primo\n";
        } 
            num_val_processar--;
        sem_post(&mutex_encerramento);
    }
     
    sem_post(&full);
    sem_post(&empty_mem);
}


int main(int argc, char* argv[])
{
    int num_th_prod = atoi(argv[1]);
    int num_th_consum = atoi(argv[2]);
    tamanho_fila = atoi(argv[3]);
    memoria.resize(tamanho_fila);

    sem_init(&full,0,0);
    sem_init(&empty_mem,0,tamanho_fila);
    sem_init(&mutex_fila,0,1);
    sem_init(&mutex_encerramento,0,1);
    
    vector<thread> threads;

    struct timespec start, finish;
    double elapsed;

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < num_th_prod; i++) threads.push_back(thread(produtor));
    for (int i = 0; i < num_th_consum; i++) threads.push_back(thread(consumidor));

    for (auto& th : threads) th.join();

    clock_gettime(CLOCK_MONOTONIC, &finish);

    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    cout << " rodou todos os testes\n";
    cout << "M: " << tamanho_fila << " np: "<< num_th_prod << " np: "<< num_th_consum << " tempo: " << elapsed << '\n';

    return 0;
}

// g++ -std=c++11 -pthread -o p ProdutorConsumidor.cpp