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


struct sembuf semopinc = {
    .sem_num = 0,
    .sem_op = 1,
    .sem_flg = 0
}; 

struct sembuf semopdec = {
    .sem_num = 0,
    .sem_op = -1,
    .sem_flg = 0
};   

void zad1_server() {
    // generate unique key
    key_t key = ftok("/home/maciej/chess.zip", 65);
  
    int segment_id = shmget(key, 8000, IPC_CREAT | 0666);
    printf("Error: %s\n", strerror(errno));
    char* message = shmat(segment_id, NULL, 0);
    printf("Error: %s\n", strerror(errno));

    key_t key2 = ftok("/home/maciej/htdocs.zip", 65);
    int sem_id = semget(key2, 1, IPC_CREAT | 0666);
    printf("Error: %s\n", strerror(errno));

    semop(sem_id, &semopinc, 1);

    strcpy(message, "start");
    for (;;) {
        semop(sem_id, &semopdec, 1);
        strcpy(message, "HEEEEEEEEEYYYYYYYYYYYY");
        semop(sem_id, &semopinc, 1);

        semop(sem_id, &semopdec, 1);
        strcpy(message, "HOOOOOOOOOOOOOOOOOOOOO");
        semop(sem_id, &semopinc, 1);
    }
    shmdt(message);
    shmctl(segment_id, IPC_RMID, 0);
}

void zad1_client() {
    key_t key = ftok("/home/maciej/chess.zip", 65);
    int segment_id = shmget(key, 8000, IPC_CREAT | 0666);
    char* message = shmat(segment_id, NULL, 0);

    key_t key2 = ftok("/home/maciej/htdocs.zip", 65);
    int sem_id = semget(key2, 1, IPC_CREAT | 0666);

    

    strcpy(message, "start");
    for (;;) {
        semop(sem_id, &semopdec, 1);
        if (strcmp(message, "HEEEEEEEEEYYYYYYYYYYYY")) {
            printf("hey\n");
        } else if (strcmp(message, "HOOOOOOOOOOOOOOOOOOOOO")) {
            printf("ho\n");
        } else {
            printf("ERROR!!12312312312312!!!!\n");
        }
        semop(sem_id, &semopinc, 1);
    }

    shmdt(message);
    shmctl(segment_id, IPC_RMID, 0);
}

/* 
1. Napisa?? dwa programy komunikuj??ce si?? poprzez pami???? wsp????dzielon??:
a) W niesko??czonej p??tli wpisuje do wsp????dzielonej pami??ci na przemian napisy:
"haaaa" i "hoooooo" (w to samo miejsce).
b) W niesko??czonej p??tli odczytuje z wsp????dzielonej pami??ci napis i sprawdza czy jest
to "haaaa" lub "hoooooo". Je??eli napis jest r????ny od tych napis??w, to powinien
pojawi?? si?? komunikat o b????dzie i warto???? b????dnego napisu. 
*/
int zad1(int argc, char const *argv[])
{    
    pid_t pid = fork();
    if (pid == 0) {zad1_client();} else {zad1_server();}
    return 0;
}

void zad2_server() {
    // generate unique key
    key_t key = ftok("/home/maciej/chess.zip", 65);
  
    int segment_id = shmget(key, 8000, IPC_CREAT | 0666);
    printf("Error: %s\n", strerror(errno));
    int* honey = shmat(segment_id, NULL, 0);
    printf("Error: %s\n", strerror(errno));

    *honey = 6789;

    key_t key2 = ftok("/home/maciej/htdocs.zip", 65);
    int sem_id = semget(key2, 1, IPC_CREAT | 0666);
    printf("Error: %s\n", strerror(errno));

    semop(sem_id, &semopinc, 1);

    for (;;) {
        semop(sem_id, &semopdec, 1);
        if (*honey > 0) {
             *honey -= 1;
             printf("[Mis] Odjalem miod, zostalo %d\n", *honey);
        } else {
            printf("[Mis] Ul pusty!\n");
        }
        semop(sem_id, &semopinc, 1);
    }
    
    shmdt(honey);
    shmctl(segment_id, IPC_RMID, 0);
}

void zad2_client() {
    key_t key = ftok("/home/maciej/chess.zip", 65);
    int segment_id = shmget(key, 8000, IPC_CREAT | 0666);
    int* honey = shmat(segment_id, NULL, 0);

    key_t key2 = ftok("/home/maciej/htdocs.zip", 65);
    int sem_id = semget(key2, 1, IPC_CREAT | 0666);

    for (;;) {
        semop(sem_id, &semopdec, 1);
        *honey += 1;
        printf("[Pszczola] Dodalam miod, jest %d\n", *honey);
        semop(sem_id, &semopinc, 1);
    }

    shmdt(honey);
    shmctl(segment_id, IPC_RMID, 0);
}

/*
2. Pszczo??y zbieraj?? mi??d i zanosz?? do ula. W pobli??u za?? czaj?? si?? misie, kt??re
podbieraj?? raz na jaki?? czas mi??d z ula i zjadaj?? ze smakiem. Nale??y napisa??
program symuluj??cy powy??ej opisan?? sytuacj??.
Mi??d: ilo???? miodu aktualnie znajduj??cego si?? w ulu powinna by?? reprezentowana
przez warto???? zmiennej ca??kowitoliczbowej w pami??ci wsp????dzielonej,
Pszczo??y: produkuj?? mi??d (zwi??kszaj?? warto???? zmiennej w pami??ci wsp????dzielonej),
ka??da pszczo??a posiada wewn??trzny licznik okre??laj??cy ile miodu zanios??a
od pocz??tku swego istnienia do ula,
Misie: zjadaj?? mi??d (zmniejszaj?? warto???? zmiennej w pami??ci wsp????dzielonej),
podobnie jak pszczo??y, ka??dy mi?? powinien posiada?? wewn??trzny licznik
okre??laj??cy ile miodu zjad?? od pocz??tku swego istnienia; mi?? nie mo??e zje????
wi??cej miodu ni?? jest go w ulu (minimalna warto???? zmiennej w pami??ci
wsp????dzielonej jest r??wna zeru)
 Procesy powinny generowa?? komunikaty w momentach dodania miodu, zabrania
miodu z ula. Np.:
Pszczo??a_1: zanios??am do ula 1kg miodu z lipy, w ulu obecnie jest 342kg miodu.
Mi??_007: zjad??em 10kg miodku, zosta??o jeszcze 332kg
*/

// todo: miec ten kod i bedziemy dodawac semafory 2022-12-20
int zad2(int argc, char const *argv[])
{    
    pid_t pid = fork();
    if (pid == 0) {zad2_client();} else {zad2_server();}
    return 0;
}

int main(int argc, char const *argv[])
{
    /*char const *testargv[] =  {"main", "tekst.txt"}; return zad7(2, testargv);*/

    return zad2(argc, argv);
}