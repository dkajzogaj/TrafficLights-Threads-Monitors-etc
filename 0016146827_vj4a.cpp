#include <iostream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <cmath>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>


using namespace std;

int n;
pthread_t *dretvinkljuc;
int *brojzadretve;
pthread_mutex_t sistemmonitora;
pthread_cond_t sistemredamonitora;
int uneseni_brojevi;


void *barijera4a(void *dretva){
    
    int identifikacijadretve = *(int*)dretva;
    int unos;

    pthread_mutex_lock(&sistemmonitora);

    cout << "Dretva " << identifikacijadretve << ". unesite broj ";
    cin >> unos;

    uneseni_brojevi++;


    if(uneseni_brojevi<n){
        pthread_cond_wait(&sistemredamonitora,&sistemmonitora);
    }
    else{
        uneseni_brojevi=0;
        pthread_cond_broadcast(&sistemredamonitora);
    }
    
    cout << "Dretva " << identifikacijadretve << ". uneseni broj je " << unos << endl;

    pthread_mutex_unlock(&sistemmonitora);

    return NULL;

}

void zavrsetak(int sig){

    if(!sig){
        for(int i=0;i<n;i++)
            pthread_join(dretvinkljuc[i],NULL);
    }
    else{
        for(int i=0;i<n;i++)
            pthread_kill(dretvinkljuc[i],SIGKILL);
    }

    delete(brojzadretve);
    delete(dretvinkljuc);

    pthread_mutex_destroy(&sistemmonitora);
    pthread_cond_destroy(&sistemredamonitora);

    exit(sig);

}

int main(int argc, char *argv[]){
    
    n = atoi(argv[1]);
    dretvinkljuc = new pthread_t[n];
    brojzadretve = new int [n];

    pthread_mutex_init(&sistemmonitora,NULL);
    pthread_cond_init(&sistemredamonitora,NULL);

    for(int i=0;i<n;i++){
        brojzadretve[i]=i;
        pthread_create(&dretvinkljuc[i],NULL,barijera4a, &brojzadretve[i]);
    }

    sigset(SIGINT,zavrsetak);
    zavrsetak(0);

    return 0;

}
