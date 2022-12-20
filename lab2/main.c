#include <sys/types.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h> 
#include <fcntl.h>


/*
1. Napisz program tworzący dwa procesy. Każdy ze stworzonych procesów
powinien utworzyć proces – potomka. Należy wyświetlać identyfikatory
procesów rodziców i potomków po każdym wywołaniu funkcji fork.
*/
int zad1(int argc, char const *argv[])
{
    printf("1. Stworzenie drugiego procesu\n");
    int pid = fork();

    printf("PID: %d, ", getpid());
    printf("PID rodzica: %d\n", getppid());

    printf("2. Utworzenie procesu potomka dla obu procesow\n");
    int pid2 = fork();

    printf("PID: %d, ", getpid());
    printf("PID rodzica: %d\n", getppid());
    return 0;
}

/*
2. Napisz program wypisujący napis „Poczatek”, następnie wywołujący funkcję
fork i wypisujący napis „Koniec”. Co jest efektem uruchomienia tego programu? 
*/
int zad2(int argc, char const *argv[])
{
    printf("Poczatek\n");
    int pid = fork();
    printf("Koniec\n");
    return 0;
}

/*
3. Napisz program wypisujący napis „Poczatek”, następnie wywołujący funkcję
exec powodującą wydruk zawartości bieżącego katalogu i następnie wypisujący
napis „Koniec”. Co jest efektem uruchomienia tego programu?
*/
int zad3(int argc, char const *argv[])
{
    printf("Poczatek\n");
    int success = execlp("ls", "ls", "-la", NULL);
    printf("Koniec\n");
    return 0;
}

/*
4. Napisz program którego rezultatem będzie wydruk zawartości bieżącego katalogu
poprzedzony napisem „Poczatek” a zakończony napisem „Koniec” 
*/
int zad4(int argc, char const *argv[])
{
    printf("Poczatek\n");
    pid_t pid = fork();
    if (pid == 0) {
        execlp("ls", "ls", "-la", NULL);
        exit(0);
    } else {
        waitpid(pid, NULL, 0);
        printf("Koniec\n");
    }
    
    return 0;
}

/*
5. Napisz program, którego wynikiem jest sformatowana lista procesów:
--------poczatek listy-------------
proces 1
proces2
.....
--------koniec listy--------------- 
*/
int zad5(int argc, char const *argv[])
{
    printf("--------poczatek listy-------------\n");
    pid_t pid = fork();
    if (pid == 0) {
        execlp("ps", "ps", "aux", NULL);
        exit(0);
    } else {
        waitpid(pid, NULL, 0);
        printf("--------koniec listy---------------\n");
    }
    
    return 0;
}



/*6. Napisz program tworzący równocześnie trzy procesy zombi. */
int zad6(int argc, char const *argv[])
{
    pid_t pid1 = fork();
    pid_t pid2, pid3;
    // 1 proces zombie
    if (pid1 != 0) {
        pid2 = fork();
        // 2 procesy zombie
    }

    if (pid1 != 0 && pid2 != 0) {
        pid3 = fork();
        // 3 procesy zombie
    }

    if (pid1 != 0 && pid2 != 0 && pid3 != 0) {
        sleep(100);
    } else {
        exit(0);
    }

    wait(&pid1);
    wait(&pid2);
    wait(&pid3);


    return 0;
}

/* 7. Zaimplementuj program wykonujący polecenie ps ax, i zapisujący jego wynik w pliku o
nazwie podanej jako parametr wywołania programu.*/
int zad7(int argc, char const *argv[])
{
    if (argc != 2) {printf("Niewlasciwa liczba parametrow!\n"); return 1;}
    printf("%s %s", argv[0], argv[1]);

    pid_t pid = fork();
    if (pid == 0) {
        // S_IRUSR | S_IWUSR so I don't have to run chmod after
        int fd = open(argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        // Redirect stdout to file
        dup2(fd, 1);
        // close the original descriptor
        close(fd);

        execlp("ps", "ps", "ax", NULL);
        exit(0);
    } else {
        waitpid(pid, NULL, 0);
    }
    

    return 0;
}

/* 8. Napisz program tworzący proces sierotę.*/
int zad8(int argc, char const *argv[])
{
    pid_t pid1 = fork();
    // zabijamy rodzica...
    if (pid1 != 0) {
        return 0;
    } else {
         printf("ID mojego rodzica: %d\n", getppid());
    }

    // ...i powstaje sierota :)
    printf("Moj rodzic zostal zabity!\n");
    sleep(1);
    printf("ID mojego nowego rodzica: %d\n", getppid());

    return 0;
}

/* 9. Stwórz grupę procesów (3 procesy – jeden z nich jest liderem). Niech lider czeka na
zakończenie dwóch procesów z jego grupy. Mogą się przydać funkcje: setgrp() oraz setpgid()*/
int zad9(int argc, char const *argv[])
{
    pid_t pid1 = fork();
    pid_t pid2;
    // 2 processes
    if (pid1 != 0) {
        pid2 = fork();
        // 3 processes
    }

    if (pid1 == 0 || pid2 == 0) {
        printf("A child is now awake!\n");
        sleep(1);
        printf("A child is now asleep.\n");
        exit(0);
    } else {
        // Using pid1 PID as the PGID
        setpgid(0, pid1);
        setpgid(pid1, pid1);
        setpgid(pid2, pid1);

        printf("Waiting for two group processes\n");
        pid_t pgid = -pid1;
        wait(&pgid);
        wait(&pgid);
        printf("Everyone is asleep\n");
    }

    
    return 0;
}

int main(int argc, char const *argv[])
{
    /*char const *testargv[] =  {"main", "tekst.txt"}; return zad7(2, testargv);*/
    
    return zad9(argc, argv);
}