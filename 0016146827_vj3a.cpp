#include <iostream>
#include <cstring>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <csignal>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <pthread.h>
#include <semaphore.h>

using namespace std;

int m,n, *brojzadretve, brojzadataka;
unsigned long long int slucajnibroj;
sem_t semafor1g;
sem_t semafor2r;
pthread_t *kljuczadretve;

void *generiraj(void *gener){

    printf("Dretva koja generira zadatke pocela je s radom. Broj zadataka = %i\n", n);

    for(int i=0;i<n;i++){

        unsigned long long int generator=rand() % 10000000 + 1;
        slucajnibroj=generator;
        printf("Generiran broj %lli\n",slucajnibroj);
        sem_post(&semafor1g);
        sem_wait(&semafor2r);

    }

    return NULL;
};

void *racunaj(void *racun){

    int brojdretve = *((int*)racun);
    unsigned long long int suma=0;

    printf("Dretva %i pocela je s radom.\n",brojdretve);

    while(brojzadataka){
    
        sem_wait(&semafor1g);
        brojzadataka--;

        unsigned long long int pomocnizarandom=slucajnibroj;
        printf("Dretva %i. preuzela zadatak %lli\n",brojdretve,pomocnizarandom);
        
        sem_post(&semafor2r);

        for(int i=1;i<=pomocnizarandom;i++){
            suma+=i;
            
        }
        
        printf("Dretva %i. zadatak = %lli zbroj = %lli\n",brojdretve,pomocnizarandom,suma);
        
    }

    return NULL;
};

void zavrsetak(int sig){
    if(!sig){
        for(int i=0;i<m;i++)
            pthread_join(kljuczadretve[i],NULL);
    }
    else
    {
        for(int i=0;i<m;i++)
            pthread_kill(kljuczadretve[i],SIGKILL);
    }
    sem_destroy(&semafor1g);
    sem_destroy(&semafor2r);
}

int main(int argc, char *argv[]){

    srand(time(0));

    m = atoi(argv[1])+1;
    n = atoi(argv[2]);
    brojzadataka = n;

    kljuczadretve = new pthread_t [m];
    brojzadretve = new int[m];

    sem_init(&semafor1g,0,0);
    sem_init(&semafor2r,0,0);

    pthread_create(&kljuczadretve[0],NULL,generiraj,&brojzadretve[0]);
    
    for(int i=1;i<m;i++){
        brojzadretve[i]=i;
        pthread_create(&kljuczadretve[i],NULL,racunaj,&brojzadretve[i]);
    }

    sigset(SIGINT,zavrsetak);

    zavrsetak(0);

    return 0;
}
