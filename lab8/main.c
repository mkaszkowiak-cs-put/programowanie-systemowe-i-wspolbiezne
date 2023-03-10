#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h> 
#include <fcntl.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

void zad1_server() {
    // generate unique key
    key_t key = ftok("/home/maciej/chess.zip", 65);
  
    int segment_id = shmget(key, 8000, IPC_CREAT | 0666);
    printf("Error: %s\n", strerror(errno));
    char* message = shmat(segment_id, NULL, 0);
    printf("Error: %s\n", strerror(errno));
    strcpy(message, "start");
    for (;;) {
        strcpy(message, "HEEEEEEEEEYYYYYYYYYYYY");
        strcpy(message, "HOOOOOOOOOOOOOOOOOOOOO");
    }
    shmdt(message);
    shmctl(segment_id, IPC_RMID, 0);
}

void zad1_client() {
    key_t key = ftok("/home/maciej/chess.zip", 65);
    int segment_id = shmget(key, 8000, IPC_CREAT | 0666);
    char* message = shmat(segment_id, NULL, 0);
    strcpy(message, "start");
    for (;;) {
        if (strcmp(message, "HEEEEEEEEEYYYYYYYYYYYY")) {
            printf("ok\n");
        } else if (strcmp(message, "HOOOOOOOOOOOOOOOOOOOOO")) {
            printf("ok\n");
        } else {
            printf("ERROR!!12312312312312!!!!\n");
        }
    }

    shmdt(message);
    shmctl(segment_id, IPC_RMID, 0);
}

/* 
1. Napisać dwa programy komunikujące się poprzez pamięć współdzieloną:
a) W nieskończonej pętli wpisuje do współdzielonej pamięci na przemian napisy:
"haaaa" i "hoooooo" (w to samo miejsce).
b) W nieskończonej pętli odczytuje z współdzielonej pamięci napis i sprawdza czy jest
to "haaaa" lub "hoooooo". Jeżeli napis jest różny od tych napisów, to powinien
pojawić się komunikat o błędzie i wartość błędnego napisu. 
*/
int zad1(int argc, char const *argv[])
{    
    pid_t pid = fork();
    if (pid == 0) {zad1_client();} else {zad1_server();}
    return 0;
}

void zad2_server() {

}

void zad2_client(int receive_type) {
}

/*
2. Pszczoły zbierają miód i zanoszą do ula. W pobliżu zaś czają się misie, które
podbierają raz na jakiś czas miód z ula i zjadają ze smakiem. Należy napisać
program symulujący powyżej opisaną sytuację.
Miód: ilość miodu aktualnie znajdującego się w ulu powinna być reprezentowana
przez wartość zmiennej całkowitoliczbowej w pamięci współdzielonej,
Pszczoły: produkują miód (zwiększają wartość zmiennej w pamięci współdzielonej),
każda pszczoła posiada wewnętrzny licznik określający ile miodu zaniosła
od początku swego istnienia do ula,
Misie: zjadają miód (zmniejszają wartość zmiennej w pamięci współdzielonej),
podobnie jak pszczoły, każdy miś powinien posiadać wewnętrzny licznik
określający ile miodu zjadł od początku swego istnienia; miś nie może zjeść
więcej miodu niż jest go w ulu (minimalna wartość zmiennej w pamięci
współdzielonej jest równa zeru)
 Procesy powinny generować komunikaty w momentach dodania miodu, zabrania
miodu z ula. Np.:
Pszczoła_1: zaniosłam do ula 1kg miodu z lipy, w ulu obecnie jest 342kg miodu.
Miś_007: zjadłem 10kg miodku, zostało jeszcze 332kg
*/

// todo: miec ten kod i bedziemy dodawac semafory 2022-12-20
int zad2(int argc, char const *argv[])
{    
    pid_t pid = fork();
    if (pid == 0) {zad2_client(6);} else {zad2_server();}
    return 0;
}

int main(int argc, char const *argv[])
{
    /*char const *testargv[] =  {"main", "tekst.txt"}; return zad7(2, testargv);*/

    return zad1(argc, argv);
}