#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#define HISTORY_SIZE 2

#define MAX_LINE 100

// Function to execute commands
void execute_command(char *cmd) {
    char *args[100];   // 保存命令中的所有参数
    char *args2[100];  // 当有管道时，第二条命令保存在args2中
    int isPipe = 0;
    int i = 0;                      // 保存参数个数
    int i2 = 0;
    int background = 0;             // 如果命令行的最后字符是&，则background=1
    int redirect_output = -1, redirect_input = -1;

    // Split command into arguments
    char *token = strtok(cmd, " ");
    while (token != NULL)
    {
        if (strcmp(token, ">") == 0)
        {
            token = strtok(NULL, " ");
            redirect_output = open(token, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (redirect_output < 0)
            {
                perror("Failed to open file");
                return;
            }
        }
        else if (strcmp(token, "<") == 0)
        {
            token = strtok(NULL, " ");
            redirect_input = open(token, O_RDONLY);
            if (redirect_input < 0)
            {
                perror("Failed to open file");
                return;
            }
        }
        else if (strcmp(token, "&") == 0)
        {
            background = 1; // Background execution
        }
        else if(strcmp(token, "|") == 0)
        {
            isPipe = 1;
        }
        else if(isPipe == 1)
        {
            args2[i2++] = token;
        }
        else
        {
            args[i++] = token;
        }
        token = strtok(NULL, " ");
    }
    args[i] = NULL; // End of arguments
    args2[i2] = NULL;

    // 此时我们获得了命令的各个参数，并保存在args中，当使用管道时，isPipe不为-1，且args和args2分别保存两条命令
    if(isPipe== 0)   // 没有管道
    { 
        pid_t pid = fork();
        if (pid == 0) { // Child process
            if (redirect_output != -1)
            {
                dup2(redirect_output, STDOUT_FILENO);
                close(redirect_output);
            }
            if (redirect_input != -1)
            {
                dup2(redirect_input, STDIN_FILENO);
                close(redirect_input);
            }
            execvp(args[0], args); // Execute command
            perror("Execution failed");
            exit(1);
        }
        else if (pid > 0)// Parent process
        { 
            if (!background)
            {
                wait(NULL); // 等待子进程
            }
        } else {
            perror("Fork failed");
        }
    }
    else    // 有管道
    {
        int pipefd[2];
        pipe(pipefd);
        pid_t pid1 = fork();
        if (pid1 == 0) {
            // 第一个子进程，执行 echo
            dup2(pipefd[1], STDOUT_FILENO); // 将标准输出重定向到管道
            close(pipefd[0]);
            close(pipefd[1]);
            
            execvp(args[0], args);
            perror("execvp failed");
            exit(1);
        }

        pid_t pid2 = fork();
        if (pid2 == 0) {
            // 第二个子进程，执行 wc
            dup2(pipefd[0], STDIN_FILENO); // 将标准输入重定向到管道
            close(pipefd[0]);
            close(pipefd[1]);
            
            execvp(args2[0], args2);
            perror("execvp failed");
            exit(1);
        }
        if (!background)
        {
            close(pipefd[0]);
            close(pipefd[1]);
            wait(NULL); // 等待第一个子进程
            wait(NULL); // 等待第二个子进程
        }

    }

}

// Main shell loop
int main() {
    char *input;
    while (1)
    {
        fflush(stdout);
        input = readline("# ");   // 获得输入命令

        input[strcspn(input, "\n")] = 0;    // 删除尾随换行符
        if (strcmp(input, "exit") == 0)   // 如果输入exit命令，退出程序
        { 
            break;
        }

        if (input[0] != '\0') {       // 只要有命令，就保存命令，并执行命令
            // 如果输入不为空，则将其添加到历史记录
            add_history(input);
            execute_command(input);   // 执行命令
        }
        free(input);
    }

    return 0;
}