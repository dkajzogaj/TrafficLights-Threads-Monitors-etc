#include <iostream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <csignal>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

using namespace std;

int brojprocesaproizvodaca;
int brojslucajnihbrojevaproizvodaca;
int dijeljenamemorija;
int idSemafora;
int *ulaz;
int *izlaz;
int *ukupno;
int *m;
sembuf Semafori;

void postavi_semafor(int semaforID){
    Semafori.sem_num = semaforID;
    Semafori.sem_op = 1;
    semop(idSemafora,&Semafori, 1);
}

void ispitaj_semafor(int semaforID){
    Semafori.sem_num = semaforID;
    Semafori.sem_op = -1;
    semop(idSemafora,&Semafori, 1);
}

void proizvodac(int procesp){
    srand(getpid());
    for(int i=0;i<brojslucajnihbrojevaproizvodaca;i++){
        ispitaj_semafor(0);
        ispitaj_semafor(1);

        m[*ulaz] = rand()%100000+1;
        cout << "Proizvodac " << procesp << " salje " << m[*ulaz] << endl;
        *ulaz=(*ulaz + 1)%5;

        postavi_semafor(1);
        postavi_semafor(2);
    }
    cout << "Proizvodac " << procesp << " zavrsio sa slanjem." << endl;
};

void potrosac(){
    unsigned long long int suma = 0;
    for(int i=0; i<*ukupno;i++){
        ispitaj_semafor(2);
        cout << "Potrosac prima " << m[*izlaz] << endl;
        suma += m[*izlaz];
        *izlaz = (*izlaz + 1)%5;
        postavi_semafor(0);
    }
    cout << "Potrosac - zbroj primljenih brojeva = " << suma << endl;
};


void zavrsetak(int sig){
    for(int i=0;i<=brojprocesaproizvodaca;i++)
        wait(NULL);
    shmdt(ukupno);
    shmdt(ulaz);
    shmdt(izlaz);
    shmdt(m);
    shmctl(dijeljenamemorija,IPC_RMID,NULL);
    semctl(idSemafora,IPC_RMID,0);
    exit(sig);
}

int main(int arc, char *argv[]){

    srand(time(0));

    brojprocesaproizvodaca = atoi(argv[1]);
    brojslucajnihbrojevaproizvodaca = atoi(argv[2]);
    

    dijeljenamemorija = shmget (IPC_PRIVATE, 8*sizeof(int),0600);
    m = reinterpret_cast<int*>(shmat(dijeljenamemorija,NULL,0));
    ukupno = m + 5;
    izlaz = m + 6;
    ulaz = m + 7;
    *ukupno = brojprocesaproizvodaca * brojslucajnihbrojevaproizvodaca;
    *ulaz = 0;
    *izlaz = 0;

    idSemafora = semget(IPC_PRIVATE,3,0600);
    Semafori.sem_flg = 0;
    semctl(idSemafora,0,SETVAL,atoi(argv[2]));
    semctl(idSemafora,1,SETVAL,1);
    semctl(idSemafora,2,SETVAL,0);


    sigset(SIGINT,zavrsetak);

    for(int i=0;i<=brojprocesaproizvodaca;i++){
        switch(fork()){
            case 0:
                if(i==0)
                    potrosac();
                else
                    proizvodac(i);
                exit(0);
            case -1:
                cout << "Greska u stvaranju procesa!\n";
            default: 
                break;
        }
    }

    zavrsetak(0);

    return 0;
}
