### PSiW

Created by Maciej Kaszkowiak (maciej@kaszkowiak.org)

#### How it works

The program scans a configuration file named "config.xd", which specifies message queue IDs for given names. Upon launch a name is read from argv and the corresponding message queue is created.

Upon succesful message queue creation, the program launches two threads:

1. **CLI thread** - on a loop:
- reads user input in the form of "target name ||| command ||| ID of response mesage queue"
- creates a response message queue
- connects to the target message queue
- sends a {command, ID of response message queue} struct to the target
- waits for the response and prints it

2. **Handler thread** - on a loop:
- waits for a {command, ID of response message queue} struct on the program's message queue
- executes the given command
- sends a {response} struct to the response message queue

"exit" is a special command used to terminate both threads and destroy the message queue.

#### How to run:
```
gcc -o test main.c
./test usr1
```

#### Example commands:
```
> usr1 ||| ls -la ||| 1000
> usr2 ||| echo "foobar" ||| 1000
> exit
```

Please note that if you don't exit using the "exit" command, the message queue won't be destroyed until the next reboot. The program will intentionally fail if a message queue exists prior to the create call.   

#### Caveats

The project is only briefly tested and might contain errors. The file "parser.h" contains code from the Internet used for parsing, with proper source attribution. 