### PSiW

Created by Maciej Kaszkowiak (maciej@kaszkowiak.org)

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