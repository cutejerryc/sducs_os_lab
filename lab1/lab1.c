#include "lab1.h"

int main(int argc, char *argv[]) {
    signal(SIGINT,(sighandler_t)sigcat);
    int times = 3; //循环次数
    int state;

    while (times --) {
        pid_t ls_id = fork(); //第一个子进程
        
        if (ls_id < 0) {
            perror("出错了");
            exit(EXIT_FAILURE);
        }
        else if (ls_id == 0) {
            pause(); //先挂起
            
            state = execlp("ls", "ls", "-l", NULL);
        }
        else {
            sleep(0.5); //确保先挂起，再唤醒
            printf("ls created\n");
            pid_t ps_id = fork(); //第二个子进程
            
            if (ps_id < 0) {
                perror("出错了");
                exit(EXIT_FAILURE);
            }
            else if (ps_id == 0) {
                pause(); //同样先挂起
                
                state = execlp("ps", "ps", NULL); 
            }
            else {
                sleep(0.5); //等待子进程
                printf("ps created\n");
                kill(ps_id, SIGINT); //唤醒第二个，执行ps
                waitpid(ps_id, &state, 0);
                printf("ps success\n");
                
            }
            kill(ls_id, SIGINT); //唤醒第一个，执行ls
            waitpid(ls_id, &state, 0);
            printf("ls success\n");
            
        }
        printf("\nNext ...\n");
        sleep(3);
    }

    return 0;
}