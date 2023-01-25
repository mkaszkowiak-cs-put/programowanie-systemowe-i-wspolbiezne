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
#include "parser.h"

struct command_msg
{
    long mtype;
    char command[1024];
    int response_id;
};

struct response_msg
{
    long mtype;
    char response[2048];
};

char *read_from_config(char *search_key)
{
    config_option_t co;
    if ((co = read_config_file("config.xd")) == NULL)
    {
        printf("[!] Config.xd not found!\n");
        return 0;
    }
    while (1)
    {
        if (strcmp(search_key, co->key) == 0)
            return co->value;

        if (co->prev != NULL)
        {
            co = co->prev;
        }
        else
        {
            break;
        }
    }
    printf("[!] Search key %s not found in the configuration file!\n", search_key);
    return 0;
}

int handler_thread(int receive_queue)
{
    printf("[Handler] Now listening under #%d message queue...\n", receive_queue);
    for (;;)
    {
        struct command_msg command_struct;
        msgrcv(receive_queue, &command_struct, sizeof(command_struct), 1, 0);
        printf("[Handler] Received a command \"%s\"! Executing it.\n", command_struct.command);

        static char buff[2048];
        memset(buff, 0, sizeof buff);

        int p[2];
        pipe(p);
        pid_t pid = fork();
        if (pid == 0) {
            close(1);
            dup(p[1]);
            close(p[0]);
            execlp("/bin/bash", "/bin/bash", "-c", command_struct.command, (char *)0);
            exit(0);
        } else {
            read(p[0], buff, 2048);
            close(p[0]);
        }

        struct response_msg response = {
            .mtype = 1, .response = ""
        };

        strncpy(response.response, buff, sizeof(response.response));

        printf("[Handler] Command executed - sending response.\n");
        msgsnd(command_struct.response_id, &response, sizeof(response), 0);
    }
    printf("[Handler] Closing #%d message queue...\n", receive_queue);
    msgctl(receive_queue, IPC_RMID, NULL);

    return 0;
}

int cli_thread(int receive_queue, pid_t handler_pid)
{
    printf("[CLI] Waiting for commands!\n");
    char input[1024];

    char receiver_id[1024];
    char command[1024];
    char send_id[1024];
    int length;

    for (;;)
    {
        // Read a command
        printf("> ");
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("[CLI] Fgets failed!\n");
            continue;
        }

        // Remove trailing newline
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n')
        {
            input[--len] = '\0';
            length -= 1;
        }

        // Exit on "exit"
        if (strcmp(input, "exit") == 0)
        {
            printf("[CLI] Exiting the program.\n");
            kill(handler_pid, SIGKILL);
            msgctl(receive_queue, IPC_RMID, NULL);
            exit(0);
        }

        printf("[CLI] Parsing %s\n", input);

        char *token = multi_tok(input, " ||| ");
        int tokens_count = 0;
        while (token != NULL)
        {
            tokens_count += 1;
            if (tokens_count == 1)
            {
                strcpy(receiver_id, token);
            }
            else if (tokens_count == 2)
            {
                strcpy(command, token);
            }
            else if (tokens_count == 3)
            {
                strcpy(send_id, token);
            }
            token = multi_tok(NULL, " ||| ");
        }

        if (tokens_count != 3)
        {
            printf("[CLI] Tokens count %d != 3: <receiver_id> ||| <command> ||| <response_queue_id>\n", tokens_count);
            continue;
        }

        printf("[CLI] Will proceed to send command \"%s\" to %s and provide %s as response queue\n", command, receiver_id, send_id);

        char *config_id_txt = read_from_config(receiver_id);
        if (config_id_txt == 0)
        {
            printf("[CLI] Couldn't find %s in the configuration file!\n", receiver_id);
            continue;
        }

        printf("[CLI] Found configuration for %s: %s\n", receiver_id, config_id_txt);

        int client_q_id = atoi(config_id_txt);
        int client_q = msgget(client_q_id, 0666);
        if (client_q == -1)
        {
            printf("[CLI] Couldn't open the #%d client message queue!\n", client_q_id);
            continue;
        }

        printf("[CLI] Opened the client message queue #%d.\n", client_q_id);

        int send_id_int = atoi(send_id);

        if (client_q_id == send_id_int) {
            printf("[CLI] Error! Response queue ID must not match the client queue ID!\n");
            continue;
        }

        int response_q = msgget(send_id_int, 0666 | IPC_CREAT | IPC_EXCL);
        if (response_q == -1)
        {
            printf("[CLI] Couldn't create a message queue!\n");
            continue;
        }
        printf("[CLI] Created #%d message queue.\n", response_q);

        struct command_msg command_struct = {
            .mtype = 1,
            .response_id = response_q};

        strncpy(command_struct.command, command, sizeof(command_struct.command));

        printf("[CLI] Sending the command \"%s\" to the client using #%d message queue...\n", command, client_q);
        msgsnd(client_q, &command_struct, sizeof(command_struct), 0);

        printf("[CLI] Waiting for the response.\n");
        struct response_msg response;
        msgrcv(response_q, &response, sizeof(response), 1, 0);

        printf("[CLI] Received a response:\n\n---\n%s\n---\n\n", response.response);
        printf("[CLI] Closing the response message queue.\n");
        msgctl(response_q, IPC_RMID, NULL);
    }
    return 0;
}

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        printf("[!] Not enough arguments passed.\n");
        return 1;
    }

    char *config_id_txt = read_from_config((char *)argv[1]);
    if (config_id_txt == 0)
        return 1;

    printf("[*] Found configuration for %s: %s\n", argv[1], config_id_txt);

    int config_id = atoi(config_id_txt);
    printf("[*] Creating message queue under ID %d...\n", config_id);

    int msgid = msgget(config_id, 0666 | IPC_CREAT | IPC_EXCL);
    if (msgid == -1)
    {
        printf("[!] Couldn't create a message queue!\n");
        return -1;
    }

    printf("[*] Message queue created - ID %d! Forking the process\n", msgid);

    pid_t child_pid = fork();
    if (child_pid == 0)
    {
        handler_thread(msgid);
    }
    else
    {
        cli_thread(msgid, child_pid);
    }

    return 0;
}
