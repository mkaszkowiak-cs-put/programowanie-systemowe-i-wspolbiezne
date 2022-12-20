#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h> 
#include <fcntl.h>
#include <signal.h>

#define FIFO_PATH "/tmp/fifo_psiw"
#define FIFO_PATH2 "/tmp/fifo_psiw2"
#define BUFFER_SIZE 128

#define READ_FIFO(path, var) int var = open(path, O_RDONLY); \
    if (var < 0) { \
        printf("Couldn't open the named pipe!\n"); \
        return 1; \
    } \

#define WRITE_FIFO(path, var) int var = open(path, O_WRONLY); \
    if (var < 0) { \
        printf("Couldn't open the named pipe!\n"); \
        return 1; \
    } \

#define CREATE_FIFO(path) mkfifo(path, 0666);

#define READ_FROM_FIFO(file_desc, buf, read_bytes, code) int read_bytes; char buf[BUFFER_SIZE]; for (;;) { \
        read_bytes = read(file_desc, buf, BUFFER_SIZE); \
        if (read_bytes <= 0) \
            break; \
        {code}}

#define READ_BUFFER(buf, read_bytes) fwrite(buf, read_bytes, 1, stdout);
/*
1. Napisać program który tworzy dwa procesy – zapisujący do kolejki FIFO i
odczytujący z niej napis „HALLO!”.
*/
int zad1_consumer(int argc, char const *argv[])
{
    CREATE_FIFO(FIFO_PATH);
    READ_FIFO(FIFO_PATH, file_desc);
    READ_FROM_FIFO(file_desc, buf, read_bytes, {
        READ_BUFFER(buf, read_bytes);
    });
    return 0;
}

int zad1_producer(int argc, char const *argv[])
{
    WRITE_FIFO(FIFO_PATH, file_desc);
    write(file_desc, "HALLO!\n", 8);
    return 0;
}

/*
2. Zrealizować następujące potoki:
• ls | wc
• ps –ef | tr –s ‘ ‘ :| cut –d: -f1 |sort| uniq –c |sort –n 
*/
int zad2(int argc, char const *argv[])
{
   
    return 0;
}

/*
3. Napisać program tworzący dwa procesy: klienta i serwera. Serwer tworzy
ogólnodostępną kolejkę FIFO, i czeka na zgłoszenia klientów. Każdy klient tworzy
własną kolejkę, poprzez którą będą przychodzić odpowiedzi serwera. Zadaniem
klienta jest przesłanie nazwy stworzonej przez niego kolejki, a serwera odesłaniem
poprzez kolejkę stworzoną przez klienta wyniku polecenia ls.
*/

int zad3_client() {

    CREATE_FIFO(FIFO_PATH2);

    // talk to server
    WRITE_FIFO(FIFO_PATH, server_desc);
    write(server_desc, FIFO_PATH2, sizeof(FIFO_PATH2));
    close(server_desc);

    // send some data through the specified fifo
    /*WRITE_FIFO(FIFO_PATH2, fifo_desc);
    write(fifo_desc, "Works!", 7);
    close(fifo_desc);*/

    return 0;
}

int zad3_server() {
    CREATE_FIFO(FIFO_PATH);
    READ_FIFO(FIFO_PATH, file_desc);
    READ_FROM_FIFO(file_desc, buf, read_bytes, {
        READ_BUFFER(buf, read_bytes);
    });
    
    /*
    READ_FIFO(buf, received_fifo);
        READ_FROM_FIFO(received_fifo, buf2, read_bytes2, {
            READ_BUFFER(buf2, read_bytes2);
        });
    */
    return 0;
}

int zad3(int argc, char const *argv[])
{
   // for testing purposes i'll use fork
    pid_t pid = fork();
    if (pid != 0) {
        return zad3_client();
    } else {
        return zad3_server();
    }
    return 0;
}

/*
4. Zmodyfikować program 3, tak, by kolejka utworzona przez klienta była
dwukierunkowa, klient publiczną kolejką powinien przesyłać nazwę stworzonej przez
siebie kolejki. Dalsza wymiana komunikatów powinna odbywać się poprzez kolejkę
stworzoną przez klienta. Klient kolejką tą powinien wysyłać polecenia, zadaniem
serwera jest wykonywanie tych poleceń i odsyłanie wyników. 
*/
int zad4(int argc, char const *argv[])
{
 
    return 0;
}

/*
5. Napisać program tworzący pierścień procesów połączonych za pomocą kolejki FIFO.
Liczbę procesów w pierścieniu podaje użytkownik. Jeden z procesów ma
zainicjalizować wysłanie komunikatu, a pozostałe odczytać go, wyświetlić na ekranie
i wysłać dalej, dopisując do komunikatu własny identyfikator.. 
*/
int zad5(int argc, char const *argv[])
{
    
    return 0;
}

int main(int argc, char const *argv[])
{
    /*char const *testargv[] =  {"main", "tekst.txt"}; return zad7(2, testargv);*/

    return zad3(argc, argv);
}