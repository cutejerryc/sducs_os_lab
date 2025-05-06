#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

//执行单个命令
void execute(char *command) {
    char *args[99];   // 保存命令中的所有参数
    char *args_p[99];  // 当有管道时，第二条命令保存在args2中
    int has_pipe = 0; //有管道为1
    int paras = 0, para_p = 0;    //参数个数
    int background = 0;    //background=1时为后台执行
    int re_out = -1, re_in = -1; //重定向
    int state, state1; //命令运行的状态

    //分割字符串
    char *word = strtok(command, " ");
    while (word != NULL)
    {
        if (strcmp(word, ">") == 0)
        {
            word = strtok(NULL, " ");
            re_out = open(word, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (re_out < 0)
            {
                perror("文件打开失败");
                return;
            }
        }
        else if (strcmp(word, "<") == 0)
        {
            word = strtok(NULL, " ");
            re_in = open(word, O_RDONLY);
            if (re_in < 0)
            {
                perror("找不到文件");
                return;
            }
        }
        else if (strcmp(word, "&") == 0)
        {
            background = 1; //后台执行
        }
        else if(strcmp(word, "|") == 0)
        {
            has_pipe = 1;
        }
        else if(has_pipe == 1)
        {
            args_p[para_p++] = word;
        }
        else
        {
            args[paras++] = word;
        }
        word = strtok(NULL, " ");
    }
    //参数表末尾用NULL
    args[paras] = NULL; 
    args_p[para_p] = NULL;

    // 此时我们获得了命令的各个参数，并保存在args中，当使用管道时，isPipe不为-1，且args和args2分别保存两条命令
    if(has_pipe == 0)   // 没有管道
    { 
        pid_t pid = fork();
        //子进程指令命令
        if (pid == 0) {
            if (re_out != -1)
            {
                dup2(re_out, STDOUT_FILENO);
                close(re_out);
            }
            if (re_in != -1)
            {
                dup2(re_in, STDIN_FILENO);
                close(re_in);
            }
            state = execvp(args[0], args); // Execute command
            perror("找不到命令");
            exit(1);
        }
        else if (pid < 0) {
            perror("进程创建失败");
            exit(EXIT_FAILURE);
        }
        else { 
            if (!background) {
                // sleep(1);
                waitpid(pid, &state, 0); // 等待子进程
            }
        } 
    }
    //处理管道
    else    
    {
        //创建管道
        int pipec[2];
        pipe(pipec);
        //定义进程号
        pid_t pid1 = fork(), pid2;
        if (pid1 == 0) {
            
            dup2(pipec[1], STDOUT_FILENO); // 将标准输出重定向到管道
            close(pipec[0]);
            close(pipec[1]);
            
            state = execvp(args[0], args);
            perror("找不到命令");
            exit(1);
        }
        else {
            pid2 = fork();
            if (pid2 == 0) {
                dup2(pipec[0], STDIN_FILENO); //重定向到管道
                close(pipec[0]);
                close(pipec[1]);
                
                state1 = execvp(args_p[0], args_p);
                perror("找不到命令");
                exit(EXIT_FAILURE);
            }
            if (!background)
            {
                close(pipec[0]);
                close(pipec[1]);
                waitpid(pid1, &state, 0); //等待第一个子进程
                waitpid(pid2, &state1, 0); //等待第二个子进程
            }
        }
    }
}

int main() {
    char *input;
    while (1)
    {
        fflush(stdout);
        input = readline("# ");   //输入
        //删除末尾换行符
        input[strcspn(input, "\n")] = 0;   
        //结束标志 
        if (strcmp(input, "q") == 0)   
        { 
            break;
        }
        //输入为空时不执行任何操作，进入下一行
        if (input[0] == '\0') {
            free(input);
            continue;
        }
             
        //输入不为空，记录输入
        add_history(input);
        //开始执行
        execute(input);   
        free(input);
    }

    return 0;
}