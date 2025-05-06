#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstring>

using namespace std;

// 函数 f(x)
int f_x(int x) {
    if (x == 1) return 1;
    return f_x(x - 1) * x;
}

// 函数 f(y)
int f_y(int y) {
    if (y == 1 || y == 2) return 1;
    return f_y(y - 1) + f_y(y - 2);
}

// 线程函数计算 f(x)
void* calculate_fx(void* arg) {
    int x = *((int*)arg);
    int* result = new int(f_x(x));
    pthread_exit(result);
}

// 线程函数计算 f(y)
void* calculate_fy(void* arg) {
    int y = *((int*)arg);
    int* result = new int(f_y(y));
    pthread_exit(result);
}

// 线程函数计算 f(x,y)
void* calculate_fxy(void* arg) {
    int* pipe_fd = (int*)arg;
    close(pipe_fd[1]); // 关闭写端

    int fx, fy;
    read(pipe_fd[0], &fx, sizeof(fx));
    read(pipe_fd[0], &fy, sizeof(fy));

    int* result = new int(fx + fy);
    cout << *result << endl;
    close(pipe_fd[0]); // 关闭读端

    pthread_exit(result);
}

int main() {
    int x = 5, y = 5;
    pthread_t thread_fx, thread_fy, thread_fxy;
    int pipe_fd[2];

    // if (pipe(pipe_fd) == -1) {
    //     cerr << "Pipe creation failed" << endl;
    //     return 1;
    // }

    // 创建 f(x) 线程
    pthread_create(&thread_fx, nullptr, calculate_fx, &x);

    // 创建 f(y) 线程
    pthread_create(&thread_fy, nullptr, calculate_fy, &y);

    int *result_fx, *result_fy;

    // 等待 f(x) 线程结束
    pthread_join(thread_fx, (void**)&result_fx);
    
    // 等待 f(y) 线程结束
    pthread_join(thread_fy, (void**)&result_fy);

    // 写入计算结果到管道
    close(pipe_fd[0]); // 关闭读端
    write(pipe_fd[1], result_fx, sizeof(int)); // 确保写入数据大小是 int
    write(pipe_fd[1], result_fy, sizeof(int)); // 确保写入数据大小是 int
    close(pipe_fd[1]); // 关闭写端

    // 创建 f(x,y) 线程
    pthread_create(&thread_fxy, nullptr, calculate_fxy, pipe_fd);

    int *result_fxy;
    // 等待 f(x,y) 线程结束
    pthread_join(thread_fxy, (void**)&result_fxy);

    cout << "f(x) = " << *result_fx << endl;
    cout << "f(y) = " << *result_fy << endl;
    cout << "f(x, y) = " << *result_fxy << endl;

    delete result_fx;
    delete result_fy;
    delete result_fxy;

    return 0;
}