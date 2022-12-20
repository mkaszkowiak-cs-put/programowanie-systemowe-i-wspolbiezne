#include <sys/types.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h> 
#include <fcntl.h>
#include <signal.h>


void handler(int sig) {
    printf("Sygnal %d", sig);
}

/*
1. Napisz program który sprawdzi jakie sygnały są odbierane po naciśnięciu
kombinacji klawiszy Ctrl- C, Ctrl – Z, Ctrl - \.
*/
int zad1(int argc, char const *argv[])
{
    for (int i = 1; i <= 19; i++)
        signal(i, handler);
    for (;;) {}
    return 0;
}

/*
2. Napisz program ignorujący sygnał SIGINT.
*/
int zad2(int argc, char const *argv[])
{
    signal(SIGINT, SIG_IGN);
    return 0;
}

/*
4. Napisz program implementujący funkcję sleep.
*/
void wakeup(int sign){}
void psiw_sleep(unsigned int time_s) {
    signal(SIGALRM, wakeup);
    alarm(time_s);
    pause();
    // ulepszenie: przechwytywac wszystkie sygnaly
    //   i sprawdzac, czy na pewno otrzymalo sie SIGALRM
}
int zad4(int argc, char const *argv[])
{
    printf("Czesc... \n");
    psiw_sleep(1);
    printf("1s... \n");
    psiw_sleep(2);
    printf("3s!\n");
    return 0;
}


/*
3. Napisz program wyświetlający komunikat po liczbie sekund podanej jako
parametr wejściowy i jednocześnie pozwalający do czasu wyświetlenia
komunikatu wykonywać inne zadania.
*/
void zad3_task(int sig) {
    printf("Wykonano zadanie! \n");
}

int zad3(int argc, char const *argv[])
{
    int seconds = 2; // (int) argv[1]
    int number;

    signal(SIGALRM, zad3_task);
    alarm(seconds);

    printf("Podaj liczbe: ");
    scanf("%d", &number);

    printf("Twoja liczba to: %d\n", number);
    return 0;
}


/*
5. Napisz program sprawdzający jak zachowa się proces macierzysty próbujący
utworzyć proces potomny w przypadku gdy sygnał SIGCLD jest
przechwytywany, a jak w przypadku gdy sygnał ten jest ignorowany.
*/
int zad5(int argc, char const *argv[])
{
    signal(SIGCHLD, SIG_IGN);

    printf("Paczkujemy...\n");
    pid_t ppid = fork();
    if (ppid == 0) {
        exit(0);
    } else {
        sleep(3);
    }
    printf("Przy ignorowaniu SIGCHLD dzieci sa zabijane bez zamieniania ich w zombie.\n");

    signal(SIGCHLD, zad3_task);

    printf("Paczkujemy... (od razu przyjdzie wiadomosc pomimo sleep bo fork od razu zostanie wybity)\n");
    ppid = fork();
    if (ppid == 0) {
        exit(0);
    } else {
        sleep(2);
    }
    printf("Zombie ozylo!\n");

    return 0;
}

/*
6. Napisz program, w którym proces macierzysty posiadający zadaną liczbę
procesów potomnych czeka na pierwsze zakończenie potomka, a po nim kończy
pozostałe procesy potomne wysyłając sygnał SIG_TERM. (procesy potomne
powinny po uruchomieniu wywołać funkcję sleep z losowo wybraną wartością)
*/
int zad6(int argc, char const *argv[])
{
    pid_t pid1 = fork();
    pid_t pid2, pid3;
    if (pid1 != 0) {
        pid2 = fork();
    }

    if (pid1 != 0 && pid2 != 0) {
        pid3 = fork();
    }

    if (pid1 != 0 && pid2 != 0 && pid3 != 0) {
        int status;
        wait(&status);
        kill(pid1, SIGTERM);
        kill(pid2, SIGTERM);
        kill(pid3, SIGTERM);
        exit(0);
    } else {
        sleep(getpid() % 4);
        exit(0);
    }
    return 0;
}

/*
7. Napisz program wykonujący ciągle polecenia ls i ps, tzn. zaraz po zakończeniu
jednego polecenia ps wykonane powinno być następne polecenie ps i podobnie
zaraz po zakończeniu polecenia ls wykonane powinno być następne polecenie
ls.
*/


int ignoreps = 0;
void runps(int sig);

void runls(int sig) {
    if (ignoreps == 1) {
        signal(SIGCHLD, runps);
    }
    
    pid_t pid = fork();
    if (pid == 0) {
        printf("ls\n");
        // czytelniej gdy zakomentowane
        // execlp("ls", "ls", NULL);
        exit(0);
    }
}

void runps(int sig) {
    signal(SIGCHLD, runls);
    pid_t pid = fork();
    if (pid == 0) {
        printf("ps\n");
        // execlp("ps", "ps", NULL);
        exit(0);
    }
}


int zad7(int argc, char const *argv[])
{
    runls(1);
    for(;;){}
    return 0;
}

/*
8. Zmodyfikować zadanie 7 tak, aby po otrzymaniu sygnału SIGINT polecenie ps
przestało być wykonywane.
*/
void zad8_change() {
    ignoreps = 1;
}
int zad8(int argc, char const *argv[])
{
    // should work but wsl2 terminal doesnt pass SIGINT and due to PTC no time to test under linux 
    signal(SIGINT, zad8_change);
    runls(1);
    for(;;){}
    return 0;
}

/*
9. Zmodyfikować zadanie 8 tak, aby sygnał SIGINT działał jako przełącznik – tj.
jedno naciśnięcie klawiszy Ctrl-C powodowało zakończenie polecenia ps, a
następne naciśnięcie tej kombinacji klawiszy wznawiało wykonanie tego
polecenia.
*/
void zad9_change() {
    if (ignoreps == 1)
        ignoreps = 0;
    else
        ignoreps = 1;
}
int zad9(int argc, char const *argv[])
{
    signal(SIGINT, zad9_change);
    runls(1);
    for(;;){}
    return 0;
}

int main(int argc, char const *argv[])
{
    /*char const *testargv[] =  {"main", "tekst.txt"}; return zad7(2, testargv);*/
    return zad7(argc, argv);
}