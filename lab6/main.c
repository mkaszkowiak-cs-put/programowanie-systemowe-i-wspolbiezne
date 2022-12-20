#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h> 
#include <fcntl.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <ctype.h>

struct msg_struct {
    long msg_type;
    char msg_text[100];
};


void zad1_server() {
    // generate unique key
    key_t key = ftok("/home/maciej/piloty.txt", 65);
  
    int msgid = msgget(key, 0666 | IPC_CREAT);
    struct msg_struct message;
    message.msg_type = 1;
  
    strcpy(message.msg_text, "Hello!");
    msgsnd(msgid, &message, sizeof(message), 0);
    printf("Sent: %s \n", message.msg_text);
}

void zad1_client() {
    key_t key = ftok("/home/maciej/piloty.txt", 65);
    int msgid = msgget(key, 0666 | IPC_CREAT);
    struct msg_struct message;
  
    msgrcv(msgid, &message, sizeof(message), 1, 0);
    printf("Received: %s \n", message.msg_text);
  
    msgctl(msgid, IPC_RMID, NULL);
}

/* 
1. Napisz program tworzący dwa procesy komunikujące się poprzez kolejkę
komunikatów. Komunikacja polega na przesłaniu komunikatu „Hello”.
*/
int zad1(int argc, char const *argv[])
{    
    pid_t pid = fork();
    if (pid == 0) {zad1_client();} else {zad1_server();}
    return 0;
}

void zad2_server() {
    // generate unique key
    key_t key = ftok("/home/maciej/piloty.txt", 65);
  
    int msgid = msgget(key, 0666 | IPC_CREAT);

    int i;
    for (i=1; i <= 10; i++) {
        struct msg_struct message;
        message.msg_type = i;
        strcpy(message.msg_text, "Hello!");

        msgsnd(msgid, &message, sizeof(message), 0);
        printf("[%d] Sent: %s\n", i, message.msg_text);
    }
    
}

void zad2_client(int receive_type) {
    key_t key = ftok("/home/maciej/piloty.txt", 65);
    int msgid = msgget(key, 0666 | IPC_CREAT);
    struct msg_struct message;
  
    msgrcv(msgid, &message, sizeof(message), receive_type, 0);
    printf("Received: %s \n", message.msg_text);
  
    msgctl(msgid, IPC_RMID, NULL);
}

/*
2. Napisz program tworzący dwa procesy komunikujące się poprzez kolejkę
komunikatów. Komunikacja polega na przesłaniu komunikatów różnego typu.
Proces odbierający powinien odbierać tylko komunikaty typu podanego jako
argument wejściowy. ( wskazówka: zakładamy że typ komunikatu jest liczbą z przedziału
<1,10>.)
*/
int zad2(int argc, char const *argv[])
{    
    pid_t pid = fork();
    if (pid == 0) {zad2_client(6);} else {zad2_server();}
    return 0;
}

void zad3_server() {
    // generate unique key
    key_t key = ftok("/home/maciej/piloty.txt", 65);
  
    int msgid = msgget(key, 0666 | IPC_CREAT);
    struct msg_struct message;
    message.msg_type = 10;
  
    strcpy(message.msg_text, "Hello!");
    msgsnd(msgid, &message, sizeof(message), 0);

    msgrcv(msgid, &message, sizeof(message), 20, 0);
    printf("Received translated: %s \n", message.msg_text);
      
    msgctl(msgid, IPC_RMID, NULL);
    
}

void zad3_client() {
    key_t key = ftok("/home/maciej/piloty.txt", 65);
    int msgid = msgget(key, 0666 | IPC_CREAT);
    struct msg_struct message;
  
    msgrcv(msgid, &message, sizeof(message), 10, 0);
    printf("Received: %s \n", message.msg_text);

    for(char* c=message.msg_text; *c=toupper(*c); ++c) ;
    message.msg_type = 20;

    msgsnd(msgid, &message, sizeof(message), 0);
    printf("Resending translated: %s \n", message.msg_text);

}



/*
3. Napisz programy klienta i serwera realizujące następujący scenariusz:
Klient przesyła dowolny ciąg znaków do serwera, serwer w odpowiedzi odsyła ten
sam ciąg po zamianie wszystkich małych liter na duże.
*/
int zad3(int argc, char const *argv[])
{    
    pid_t pid = fork();
    if (pid == 0) {zad3_client();} else {zad3_server();}
    return 0;
}

struct M_DANE {
    long msg_type;
    int msg_int;
};

#define DANE_LICZBA 100
#define DANE_END 101
#define DANE_WYNIK 102

void zad4_server() {
    // generate unique key
    key_t key = ftok("/home/maciej/piloty.txt", 65);
  
    int msgid = msgget(key, 0666 | IPC_CREAT);
    struct M_DANE message;

    message.msg_type = DANE_LICZBA;
    message.msg_int = 10;
    msgsnd(msgid, &message, sizeof(message), 0);

    message.msg_type = DANE_LICZBA;
    message.msg_int = 40;
    msgsnd(msgid, &message, sizeof(message), 0);

    message.msg_type = DANE_END;
    message.msg_int = 73;
    msgsnd(msgid, &message, sizeof(message), 0);

    msgrcv(msgid, &message, sizeof(message), DANE_WYNIK, 0);
    printf("Received result: %d \n", message.msg_int);
      
    msgctl(msgid, IPC_RMID, NULL);
    
}

void zad4_client() {
    key_t key = ftok("/home/maciej/piloty.txt", 65);
    int msgid = msgget(key, 0666 | IPC_CREAT);
    struct msg_struct message;
  
    msgrcv(msgid, &message, sizeof(message), 10, 0);
    printf("Received: %s \n", message.msg_text);

    for(char* c=message.msg_text; *c=toupper(*c); ++c) ;
    message.msg_type = 20;

    msgsnd(msgid, &message, sizeof(message), 0);
    printf("Resending translated: %s \n", message.msg_text);

}

// M_END oraz M_WYNIK zrealizuję jako M_DANE ze zmodyfikowanym msg_type
// nie ma sensu tworzyć trzech structów z identyczną strukturą

/*
4. Napisz programy klienta i serwera realizujące następujący scenariusz:
Serwer sumuje liczby wysyłane przez klienta. Liczby są przesyłane jako komunikat
typu M_DANE, a ostatnia z liczb jest wysyłana jako komunikat M_END. Po
otrzymaniu tego komunikatu serwer odsyła wynik sumowania komunikatem typu
M_WYNIK.
*/
int zad4(int argc, char const *argv[])
{    
    return 0;
}

/*
5. Napisz program (programy) komunikujące się za pomocą kolejki komunikatów.
Każdy z programów wysyła wiadomości wpisywane z klawiatury, a po odbiorze
komunikatu z kolejki wypisuje jego treść na ekran. Operacje odczytu i zapisu
powinny być wykonywane w sposób asynchroniczny. Program kończy się po
otrzymaniu wiadomości o treści ”exit”.
*/
int zad5(int argc, char const *argv[])
{    
    return 0;
}

int main(int argc, char const *argv[])
{
    /*char const *testargv[] =  {"main", "tekst.txt"}; return zad7(2, testargv);*/

    return zad4(argc, argv);
}