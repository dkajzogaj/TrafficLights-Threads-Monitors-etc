#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iomanip>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <limits.h>
#include <sys/wait.h>
#include <pthread.h>

using namespace std;

int brojunosdretvi; 
pthread_t *zadretveID;
int *trazim;
int *broj;

int nadiNajveci(){
    int najveci=0;
    for(int i=0;i<brojunosdretvi;i++){
        if(broj[i]>najveci)
            najveci=broj[i];
    }
    return najveci;
};

void funkcijakriticnogodsjecka(void* kriticno){
    int i=*((int*)kriticno);
    trazim[i]=1;
    broj[i]=nadiNajveci()+1;
    trazim[i]=0;
    for(int j=0;j<brojunosdretvi;j++){
        while(trazim[j]!=0){
        };
        while(broj[j]!=0 &&(broj[j]<broj[i]||(broj[j]==broj[i]&& j<i))){
        };
    }
};
void izadiizkriticnogodsjecka(void* izadikriticno){
    broj[*((int*)izadikriticno)]=0;
}

void *Kreiranjedretve(void *argdretve){
    int i=*((int *)argdretve)+1;
    for(int k=1;k<=5;k++){
        funkcijakriticnogodsjecka(argdretve);
        for(int m=1;m<=5;m++){
            printf("Dretva: %d, kriticni odsjecak: %d (%d/5)\n", i, k, m); 
            usleep(600000); 
        }
        izadiizkriticnogodsjecka(argdretve);
    }
    return NULL;
};

void brisanjepomocusignala(int sig){
    if(!sig){
        for(int i=0;i<brojunosdretvi;i++)
            pthread_join(zadretveID[i],NULL);
    }     
    else{
        for(int i=0;i<brojunosdretvi;i++)
            pthread_kill(zadretveID[i],SIGKILL);
    } 
    exit(0);
};

int main(int argc, char *argv[]){

    brojunosdretvi=atoi(argv[1]);
    trazim=new int[brojunosdretvi];
    broj=new int[brojunosdretvi];
    int poljezai[brojunosdretvi];
    zadretveID=new pthread_t[brojunosdretvi];

    sigset(SIGINT,brisanjepomocusignala);

    for(int i=0;i<brojunosdretvi;i++){
        poljezai[i]=i;
        pthread_create(&zadretveID[i],NULL,Kreiranjedretve,&poljezai[i]);
    }

    brisanjepomocusignala(0);
    return 0;
};
