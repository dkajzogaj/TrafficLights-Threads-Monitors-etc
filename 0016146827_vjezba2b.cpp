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
#include <sys/wait.h>

using namespace std;

long double *brojevia;
long double *brojevib;
bool ostalo = false, indikator= false;
int l,m,n,memorijadij,brojprocesazaostatak;


void eksponentirandomplusispisFunkcija(){
    for(int i=0;i<l;i++){
        brojevia[i]=(double)rand()/(RAND_MAX)+0+(rand()%9);
        cout << brojevia[i] << endl;
    }
};

void eksponencijaleispisFunkcija(){
    for(int i=0;i<l;i++)
        cout << brojevib[i] << endl;
};
    
long double faktorijeliFunkcija(int n){
    return (n==0) || (n==1) ? 1 : n* faktorijeliFunkcija(n-1);
};

void procesplusracunanjeEksponencijala(int i){
    int pocetak= brojprocesazaostatak*i;
    int zavrsetak= pocetak + brojprocesazaostatak;
    if(zavrsetak > l){
        zavrsetak=l;
    }
    for(pocetak;pocetak<zavrsetak;pocetak++){
        if(ostalo&&indikator)
            zavrsetak=l;
        long double suma=0;
        for(int k=pocetak;k<zavrsetak;k++){
            for(int j=0;j<=m;j++){
                suma+=(pow(brojevia[k],j))/(faktorijeliFunkcija(j));
                
            }
            brojevib[k]=suma;
        }
    }
};

void brisanjebrimem(int sig){
    shmdt(brojevia);
    shmdt(brojevib);
    shmctl(memorijadij, IPC_RMID, NULL);
    exit(0);
};

int main(int argc, char *argv[]){
    
    l=atoi(argv[1]);
    m=atoi(argv[2]);
    n=atoi(argv[3]);
    memorijadij=shmget(IPC_PRIVATE,2*l*sizeof(long double),0600);
    brojevia=reinterpret_cast<long double *>(shmat(memorijadij,NULL,0));
    brojevib=brojevia+l;

    srand(time(0));
    cout << fixed << showpoint;
    cout << setprecision(12);

    sigset(SIGINT,brisanjebrimem);

    cout << "Eksponenti= \n"; 
    eksponentirandomplusispisFunkcija();

    brojprocesazaostatak=l/n;
    if(l%n){
        brojprocesazaostatak++;
        ostalo = true;
    }

    for(int i=0;i<n;i++){
        switch(fork()){
            if(i==n-1)
                indikator=true;
            case 0:{
                procesplusracunanjeEksponencijala(i);
                exit(0);
            }
            case -1:{
                cout << "Pogreska!\n";
                exit(1);
            }
        }
    }
    for(int i=0;i<n;i++)
        wait(NULL);

    cout << "Eksponencijale= \n";
        eksponencijaleispisFunkcija();
    
    brisanjebrimem(0); 

    return 0;
}
