#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

typedef void (*sighandler_t) (int);
void sigcat(){
    printf("%d Process continue\n", getpid());
}