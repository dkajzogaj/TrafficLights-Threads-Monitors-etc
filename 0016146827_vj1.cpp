#include <iostream>
#include <csignal>
#include <cstdlib>
#include <unistd.h>

using namespace std;

unsigned int sveukupnozazigice;
char sviigraci='A';

void zamijenisveigracefunkcija(){
	if(sviigraci== 'A')
		sviigraci = 'B';
	else
		sviigraci = 'A';
}

void uzimanjezigicasasignalima(int sig){
	switch(sig)
	{
		case SIGINT:{
			cout << " Zigica igrac "<< sviigraci << " uzeo za stola: 1.\n";
			sveukupnozazigice-=1;
			break;
		}
		case SIGQUIT:{
			cout << " Zigica igrac "<< sviigraci << " uzeo za stola: 2.\n";
			sveukupnozazigice-=2;
			break;
		}
		case SIGTSTP:{
			cout << " Zigica igrac "<< sviigraci << " uzeo za stola: 3.\n";
			sveukupnozazigice-=3;
			break;
		}
	}
}
int main(int argc, char *argv[])
{
	sveukupnozazigice=atoi(argv[1]);
	cout << "M = 3. N = " << sveukupnozazigice << ".\n";
	cout << "Parametri su ispravni. Zapocinjem igru.\n";
	do{
		cout << "Zigica na stolu: " << sveukupnozazigice << ". Na redu igrac: " << sviigraci << ".\n";
		sigset(SIGINT,uzimanjezigicasasignalima);
		sigset(SIGQUIT,uzimanjezigicasasignalima);
		sigset(SIGTSTP,uzimanjezigicasasignalima);
		pause();
		zamijenisveigracefunkcija();
	}while(sveukupnozazigice>0);
		cout << "Zigica na stolu: " << sveukupnozazigice << ". Pobjednik je: " << sviigraci << endl;

  return 0;
}

