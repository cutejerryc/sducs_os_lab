#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>


int pipex[2], pipey[2]; //fx, fy两个管道
pthread_t thrd1, thrd2, thrd3; //线程号fxy, fx, fy

void f_xy() {
    int fx, fy;
    //读运算结果
    read(pipex[0], &fx, sizeof(int));
    read(pipey[0], &fy, sizeof(int));
    int res = fx + fy;
    printf("Result: %d\n", res);
    close(pipex[0]);
    close(pipey[0]);
}
void f_x(int *x) {
    //执行运算
    int tmp = *x;
    int res = 1;
    while (tmp >= 1) {
        res *= tmp;
        tmp --;
    }
    //结果写管道x
    write(pipex[1], &res, sizeof(int));
    close(pipex[1]);
}
void f_y(int *y) {
    int dp1 = 1, dp2 = 1;
    if (*y <= 2) {
        write(pipey[1], &dp1, sizeof(int));
        close(pipey[1]);
        return;
    }
    for (int i = 3; i <= *y; i++) {
        int tmp = dp2;
        dp2 += dp1;
        dp1 = tmp;
    }
    write(pipey[1], &dp2, sizeof(int));
    close(pipey[1]);
}

int main(int argc, char *argv[]) {
    int state;
    int x, y;

    if (pipe(pipex) < 0) {
        perror("失败了");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipey) < 0) {
        perror("失败了");
        exit(EXIT_FAILURE);
    }

    printf("x: ");
    scanf("%d", &x);
    printf("y: ");
    scanf("%d", &y);

    state = pthread_create(&thrd1, NULL, (void*)f_xy, NULL);
    if (state) {
        perror("f_xy failed");
        exit(EXIT_FAILURE);
    }
    state = pthread_create(&thrd2, NULL, (void*)f_x, &x);
    if (state) {
        perror("f_x failed");
        exit(EXIT_FAILURE);
    }
    state = pthread_create(&thrd3, NULL, (void*)f_y, &y);
    if (state) {
        perror("f_y failed");
        exit(EXIT_FAILURE);
    }

    pthread_join(thrd2, NULL);
    pthread_join(thrd3, NULL);
    pthread_join(thrd1, NULL);

}
