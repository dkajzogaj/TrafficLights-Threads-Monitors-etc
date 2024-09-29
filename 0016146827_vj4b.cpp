#include <iostream>
#include <ctime>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <random>
#include <wait.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>

using namespace std;

int max_u_restoranu;
int programeri;
int sveukupno;
int *pomocdretvi;
pthread_t *dretvinkljuc;
pthread_mutex_t sustavmonitora;
pthread_cond_t sustavredamonitora;
int cekaj[2]={0,0};
int najeli=0;
int brprog=0;
bool da=true;
bool dopustenje;

void ispis1(int dokle, bool ogranici){
    
    for(int i=0;i<dokle;i++){
        if(ogranici==false){
            cout << "M";
        }
        else{
            cout << "L";
        }
    }

}

void ispis2(int dokle, int kojiprog){

    int razlika=programeri-dokle;

    for(int i=0;i<dokle;i++){
        if(kojiprog==0){
            cout << "M";
        }
        else{
            cout << "L";
        }
    }

    for(int i=0;i<razlika;i++)
        cout << "-";

}


void izadi_iz(bool vrsta,char kojiprog){

    pthread_mutex_lock(&sustavmonitora); 
    
    brprog--; 

    cout << "Red Linux: "; 
    ispis2(cekaj[1],1);
    cout << " Red Microsoft: ";
    ispis2(cekaj[0],0);
    cout << " Restoran: ";
    ispis1(brprog,dopustenje);
    cout << " --> " << kojiprog << " iz restorana." << endl;

    if(brprog==0){ 
        najeli=0;   

        if(cekaj[!dopustenje]!=0) 
            dopustenje=!dopustenje; 
        
        pthread_cond_broadcast(&sustavredamonitora); 
    }

    pthread_mutex_unlock(&sustavmonitora); 

};

void udi_u(bool vrsta, char kojiprog){

    pthread_mutex_lock(&sustavmonitora); 

    if(da){ 
        da=false;
        dopustenje=vrsta; 
    }

    cekaj[vrsta]++; 

    if(vrsta!=dopustenje || (najeli>=max_u_restoranu && cekaj[!dopustenje]>0)){ 
        cout << "Red Linux: ";                                                  
        ispis2(cekaj[1],1);
        cout << " Red Microsoft: ";
        ispis2(cekaj[0],0);
        cout << " Restoran: ";
        ispis1(brprog,dopustenje);
        cout << " --> " << kojiprog << " u red cekanja." << endl;
        while((vrsta!=dopustenje)||(najeli>=max_u_restoranu && cekaj[!dopustenje]>0)){
            pthread_cond_wait(&sustavredamonitora,&sustavmonitora);
        }
    }

    cekaj[vrsta]--; 
    brprog++; 

    if(cekaj[!dopustenje]>0) 
        najeli++;       

    cout << "Red Linux: ";
    ispis2(cekaj[1],1);
    cout << " Red Microsoft: ";
    ispis2(cekaj[0],0);
    cout << " Restoran: ";
    ispis1(brprog,dopustenje);
    cout << " --> " << kojiprog << " u restoran." << endl;

    pthread_mutex_unlock(&sustavmonitora); 

};


void *Rizicni(void *KO){

    srand(time(0)-50);

    bool vrsta; 
    int x=*(int*)KO;
    char kojiprog; 
    
    if(x%2==0){
        kojiprog = 'M';
        vrsta = 0; 
    }
    else{
        kojiprog = 'L';
        vrsta = 1; 
    }

    usleep(rand()%100000);

    udi_u(vrsta,kojiprog);

    usleep(rand()%100000);

    izadi_iz(vrsta,kojiprog);

    return NULL;

};


void zavrsetak(int sig){

    if(!sig){
        for(int i=0;i<sveukupno;i++)
            pthread_join(dretvinkljuc[i],NULL);
    }
    else{
        for(int i=0;i<sveukupno;i++)
            pthread_kill(dretvinkljuc[i],SIGKILL);
    }

    delete(dretvinkljuc);

    pthread_mutex_destroy(&sustavmonitora);
    pthread_cond_destroy(&sustavredamonitora);

    exit(sig);

}

int main(int argc,char *argv[]){

    max_u_restoranu = atoi(argv[1]);
    programeri = atoi(argv[2]);
    sveukupno = programeri * 2;
    pomocdretvi = new int [sveukupno];

    dretvinkljuc = new pthread_t[sveukupno];

    pthread_mutex_init(&sustavmonitora,NULL);
    pthread_cond_init(&sustavredamonitora,NULL);
    

    for(int i=0;i<sveukupno;i++){
        pomocdretvi[i]=i;
        pthread_create(&dretvinkljuc[i],NULL,Rizicni, &pomocdretvi[i]);
    }

    sigset(SIGINT,zavrsetak);
    zavrsetak(0);

    return 0;

}
