#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <unistd.h>
#define BUFFERSIZE 256
#define MAXVAL 100
#define STRSIZE 8
#define CHAIRQUEST 1        // 理发椅请求
#define SOFAQUEST 2         // 沙发请求
#define ROOMQUEST 3         // 等待室请求
#define CUT_FINISHED 4      // 理发结束消息
#define ACC_FINISHED 5      // 结账结束消息
#define HAIRQUEST 5         // 理发请求
#define ACCOUNTQUEST 6      // 结账请求
#define WRITERQUEST 1       // 写标志
#define READERQUEST 2       // 读标志
#define FINISHED 3          // 完成标志
#define SOFA 0
#define ROOM 1
#define CHAIR 2

/* 信号量控制用的共同体 */
typedef union semuns {
    int val;
} Sem_uns;

/* 消息结构体 */
typedef struct msgbuf {
    long mtype;
    int mid;
} Msg_buf;



int get_ipc_id(char *proc_file, key_t key);
char *set_shm(key_t shm_key, int shm_num, int shm_flag);
int set_msq(key_t msq_key, int msq_flag);
int set_sem(key_t sem_key, int sem_val, int sem_flag);
int sem_wait(int sem_id);
int sem_signal(int sem_id);