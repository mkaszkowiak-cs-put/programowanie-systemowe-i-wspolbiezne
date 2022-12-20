#include <sys/types.h>
#include <sys/stat.h> 
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
1. Napisz program który tworzy dwa procesy – pierwszy z procesów powinien zapisać
do potoku napis „Hallo”, a drugi proces powinien ten napis odczytać.
*/
int zad1(int argc, char const *argv[])
{
    int p[2];
    pipe(p);
    pid_t pid = fork();
    if (pid == 0) {
        // child
        close(1);
        dup(p[1]);
        close(p[0]);
        execlp("echo", "echo", "Hallo", NULL);
    } else {
        // parent
        close(0); // close stdin
        dup(p[0]);
        close(p[1]);
        execlp("cat", "cat", NULL);
    }

    return 0;
}

/*
2. Zmodyfikuj program 1, tak by przez potok stworzony przez rodzica komunikowały
się dwa procesy potomne.
*/
int zad2(int argc, char const *argv[])
{
    int p[2];
    pipe(p);
    pid_t pid = fork();
    pid_t pid2 = 0;
    if (pid == 0) {
        // parent creates a fork 
        pid2 = fork();
    }

    if (pid == 0 && pid2 != 0) { // second child
        // child
        close(1);
        dup(p[1]);
        close(p[0]);
        execlp("echo", "echo", "Hallo", NULL);
    } else if (pid == 0 && pid2 == 0) { // first child
        close(0);
        dup(p[0]);
        close(p[1]);
        execlp("cat", "cat", NULL);
    }

    printf("Hello from the parent\n");

    return 0;
}

/*
3. Napisz program który tworzy trzy procesy, z których dwa zapisują do potoku, a
trzeci odczytuje z niego i drukuje otrzymane komunikaty.
*/
int zad3(int argc, char const *argv[])
{
        int p[2];
    pipe(p);
    pid_t pid = fork();
    pid_t pid2 = 0;
    if (pid == 0) {
        // parent creates a fork 
        pid2 = fork();
    }

    if (pid == 0 && pid2 != 0) { // second child
        // child
        close(1);
        dup(p[1]);
        close(p[0]);
        execlp("echo", "echo", "Second child", NULL);
    } else if (pid == 0 && pid2 == 0) { // first child
        close(1);
        dup(p[1]);
        close(p[0]);
        execlp("echo", "echo", "first child", NULL);
    } else if (pid != 0) {
        printf("Hello from the parent\n");
        close(0); // close stdin
        dup(p[0]);
        close(p[1]);
    }
    return 0;
}

/*
4. Napisz program realizujący potok ls | wc
*/
int zad4(int argc, char const *argv[])
{
    int p[2];
    pipe(p);
    pid_t pid = fork();
    if (pid == 0) {
        // child
        close(1);
        dup(p[1]);
        close(p[0]);
        execlp("ls", "ls", NULL);
    } else {
        // parent
        close(0); // close stdin
        dup(p[0]);
        close(p[1]);
        execlp("wc", "wc", NULL);
    }

    return 0;
}

/*
5. Napisać program konwertujący wynik polecenia ls z małych liter na duże. 
*/
int zad5(int argc, char const *argv[])
{
    int p[2];
    pipe(p);
    pid_t pid = fork();
    if (pid == 0) {
        // child
        close(1);
        dup(p[1]);
        close(p[0]);
        execlp("ls", "ls", NULL);
    } else {
        // parent
        close(0); // close stdin
        dup(p[0]);
        close(p[1]);
        execlp("awk", "awk", "{print tolower($0)}", NULL);
        
    }

    return 0;
}

/*
6. Napisać program będący odpowiednikiem programu 5, realizujący programowo
potok ls |tr a-z A-Z
*/
int zad6(int argc, char const *argv[])
{
    return 0;
}

/*
7. Zrealizować następujące potoki:
• finger | cut -d' ' -f1
• ls -l | grep ^d | more
• ps –ef| tr -s ' ' :| cut -d: -f1 |sort| uniq -c |sort n
• cat /etc/group | head -5 > grupy.txt 
*/
int zad7(int argc, char const *argv[])
{
    return 0;
}

int main(int argc, char const *argv[])
{
    /*char const *testargv[] =  {"main", "tekst.txt"}; return zad7(2, testargv);*/
    return zad3(argc, argv);
}