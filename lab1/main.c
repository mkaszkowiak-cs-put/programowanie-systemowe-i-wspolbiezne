#include <sys/types.h>
#include <sys/stat.h>
// See: https://stackoverflow.com/questions/19339110/why-does-lseek-allow-me-to-set-a-negative-file-position

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <error.h>


/* 1. Napisz program kopiujący zawartość pliku o nazwie podanej jako pierwszy
parametr do pliku którego nazwa podana jest jako drugi parametr */
int zad1(int argc, char const *argv[])
{
    if (argc != 3)
    {
        printf("Podaj 2 parametry!\n");
        return 1;
    }
    int fdRead = open(argv[1], O_RDONLY);
    int fdWrite = open(argv[2], O_CREAT | O_WRONLY);

    if (fdRead == -1 | fdWrite == -1)
    {
        printf("Blad otwierania plikow\n");
        return 1;
    }

    char buf[1024];
    int len;
    while (1)
    {
        len = read(fdRead, buf, 1024);
        if (len <= 0)
        {
            printf("Przekopiowano caly plik!\n");
            break;
        }

        write(fdWrite, buf, len);
    }

    close(fdRead);
    close(fdWrite);

    return 0;
}

/* 2. Napisz program zmieniający kolejność znaków w każdej linii pliku o nazwie podanej jako parametr. */
int zad2(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("Podaj 1 parametr!\n");
        return 1;
    }

    int fileHandle = open(argv[1], O_RDWR);
    if (fileHandle == -1)
    {
        printf("Blad otwierania pliku\n");
        return 1;
    }

    char fileContents[1024];
    char buf[1];
    int len;
    int pos = 0;
    int bufferSize = 0;
    while (1)
    {
        len = read(fileHandle, buf, 1);
        pos += len;

        if (len <= 0 || buf[0] == '\n') {
            // Jump to the beginning of line
            lseek(fileHandle, pos - bufferSize - 1, SEEK_SET);
            for (int i = bufferSize - 1; i >= 0; i--) {
                buf[0] = fileContents[i];
                write(fileHandle, buf, 1);
                printf("%c", buf[0]);
            }
            printf("\n");
            // Omit new line so we dont parse it twice 
            lseek(fileHandle, 1, SEEK_CUR); 
            
            bufferSize = 0;
        } else {
            fileContents[bufferSize] = buf[0];
            bufferSize += 1;
        }

        if (len <= 0)
        {
            printf("Odczytano caly plik!\n");
            break;
        }
    }

    return 0;
}

/* 3. Napisz program wyszukujący najdłuższą linię w pliku i podający ilość znaków w tej linii. */
int zad3(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("Podaj 1 parametr!\n");
        return 1;
    }

    int fileHandle = open(argv[1], O_RDWR);
    if (fileHandle == -1)
    {
        printf("Blad otwierania pliku\n");
        return 1;
    }

    char fileContents[1024];
    char buf[1];
    int len;
    int pos = 0;
    int bufferSize = 0;
    int longestLine = 0;
    while (1)
    {
        len = read(fileHandle, buf, 1);
        pos += len;

        if (len <= 0 || buf[0] == '\n') {
            // Jump to the beginning of line
            if (bufferSize > longestLine)
                longestLine = bufferSize;
            
            bufferSize = 0;
        } else {
            fileContents[bufferSize] = buf[0];
            bufferSize += 1;
        }

        if (len <= 0)
        {
            printf("Najdluzsza linia ma %d znakow.\n", longestLine);
            break;
        }
    }

    return 0;
}

/* 4. Napisz program określający rozmiar pliku ( kilku plików o nazwach podanych jako parametry wejściowy) */
int zad4(int argc, char const *argv[])
{
    // TODO: "//kilku// plikow"
    if (argc != 2)
    {
        printf("Podaj 1 parametr!\n");
        return 1;
    }

    int fileHandle = open(argv[1], O_RDWR);
    if (fileHandle == -1)
    {
        printf("Blad otwierania pliku\n");
        return 1;
    }

    char fileContents[1024];
    int len;
    int pos = 0;
    while (1)
    {
        len = read(fileHandle, fileContents, 1024);
        pos += len;

        if (len <= 0)
        {
            break;
        }
    }

    printf("Plik ma %d bajtów.\n", pos);

    return 0;
}

/* 5a. Napisz program wypisujący od końca zawartość pliku
a) znakami b) liniami  */
int zad5a(int argc, char const *argv[])
{
    // Two different approaches to this task, either I could:
    // 1. Read the entire file into memory and reverse it
    // 2. Iterate backwards with lseek and read 

    // #2 is slower but I assume it is the required approach 
    if (argc != 2)
    {
        printf("Podaj 1 parametr!\n");
        return 1;
    }

    int fileHandle = open(argv[1], O_RDWR);
    if (fileHandle == -1)
    {
        printf("Blad otwierania pliku\n");
        return 1;
    }

    char fileContents[1024];
    int len;
    int pos = 0;
    while (1)
    {
        len = read(fileHandle, fileContents, 1024);
        pos += len;

        if (len <= 0)
        {
            break;
        }
    }

    char buf[1];
    for (int i = pos; i >= 0; i--) {
        lseek(fileHandle, i, SEEK_SET);
        read(fileHandle, buf, 1);
        printf("%c", buf[0]);
    }

    return 0;
}

/* 5. Napisz program wypisujący od końca zawartość pliku
a) znakami b) liniami  */
int zad5b(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("Podaj 1 parametr!\n");
        return 1;
    }

    int fileHandle = open(argv[1], O_RDWR);
    if (fileHandle == -1)
    {
        printf("Blad otwierania pliku\n");
        return 1;
    }

    char fileContents[1024];
    char buf[1];
    int len;
    int pos = 0;
    int bufferSize = 0;
    while (1)
    {
        len = read(fileHandle, buf, 1);
        pos += len;

        if (len <= 0 || buf[0] == '\n') {
            for (int i = bufferSize - 1; i >= 0; i--) {
                printf("%c", fileContents[i]);
            }

            if (len > 0)
                printf("\n");
            
            bufferSize = 0;
        } else {
            fileContents[bufferSize] = buf[0];
            bufferSize += 1;
        }

        if (len <= 0)
        {
            break;
        }
    }

    return 0;
}

/* 6. Napisz procedurę kopiowania ostatnich 10 znaków, słów i ostatnich 10 linii jednego pliku do innego.  */
int zad6(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("Podaj 1 parametr!\n");
        return 1;
    }

    int fileHandle = open(argv[1], O_RDWR);
    if (fileHandle == -1)
    {
        printf("Blad otwierania pliku\n");
        return 1;
    }

    char fileContents[1024];
    char buf[1];
    int len;
    int pos = 0;
    int bufferSize = 0;
    int cursorPos;

    cursorPos = lseek(fileHandle, 0, SEEK_END);
    for (int i = 0; i < 10; i++) {
        cursorPos = lseek(fileHandle, -2, SEEK_CUR);

        if (cursorPos < 0) {
            break;
        }

        len = read(fileHandle, buf, 1);
        pos += len;

        if (len <= 0) {
            break;
        }

        fileContents[bufferSize] = buf[0];
        bufferSize += 1;
    }

    printf("10 znakow od tylu: ");
    for (int i = bufferSize; i >= 0; i--) {
        printf("%c", fileContents[i]);
    }

    printf("\n");

    return 0;
}

int main(int argc, char const *argv[])
{
    return zad6(argc, argv);
}