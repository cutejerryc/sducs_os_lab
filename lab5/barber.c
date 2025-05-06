#include "ipc.h"

key_t buff_key;
int buff_num;
char *buff_ptr;
int shm_flg;

int quest_flg;
key_t quest_key;
int quest_id;

int respond_flg;
key_t respond_key;
int respond_id;

// room请求sofa相关变量
int sofa_quest_flg;
key_t sofa_quest_key;
int sofa_quest_id;
// sofa请求barber相关变量
int barber_quest_flg;
key_t barber_quest_key;
int barber_quest_id;
// barber响应sofa相关变量
int barber_respond_flg;
key_t barber_respond_key;
int barber_respond_id;
// customer请求account相关变量
int account_quest_flg;
key_t account_quest_key;
int account_quest_id;
// barber响应account相关变量
int account_respond_flg;
key_t account_respond_key;
int account_respond_id;
// 账本相关信号量
key_t account_key;
int account_sem;
// 信号量赋值/flag
int sem_flg;
int sem_val;

int main(int argc, char *argv[]) {
    int rate;    
    int barber_num = atoi(argv[1]);
    if(argv[2] != NULL)
        rate = atoi(argv[2]);
    else 
        rate = 3;
    
    Msg_buf msg_arg;
    // 建立room队列, 可容纳13人
    sofa_quest_flg = IPC_CREAT | 0644;
    sofa_quest_key = 111;
    sofa_quest_id = set_msq(sofa_quest_key, sofa_quest_flg);
    // 建立barber队列, 可容纳4人
    barber_quest_flg = IPC_CREAT | 0644;
    barber_quest_key = 211;    
    barber_quest_id = set_msq(barber_quest_key, barber_quest_flg);    
    barber_respond_flg = IPC_CREAT | 0644;
    barber_respond_key = 212;    
    barber_respond_id = set_msq(barber_respond_key, barber_respond_flg);
    // 建立account请求队列, 可容纳3个
    account_quest_flg = IPC_CREAT | 0644;
    account_quest_key = 213;
    account_quest_id = set_msq(account_quest_key, account_quest_flg);
    account_respond_flg = IPC_CREAT | 0644;
    account_respond_key = 214;    
    account_respond_id = set_msq(account_respond_key, account_respond_flg);

    // 建立账本信号量
    account_key = 312;    
    sem_flg = IPC_CREAT | 0644;
    sem_val = 1;    
    account_sem = set_sem(account_key, sem_val, sem_flg);

    printf("理发师 %d 睡眠\n", barber_num);
    // barber
    while (1) {
        if (msgrcv(barber_quest_id, &msg_arg, sizeof(msg_arg), HAIRQUEST, IPC_NOWAIT) >= 0) {     
            printf("理发师 %d 为顾客 %d 理发\n", barber_num, msg_arg.mid);
            sleep(rate);
            printf("顾客 %d 理发完毕\n", msg_arg.mid);
            // 理发师通知顾客理发完成, 让顾客找理发师收费
            msg_arg.mtype = CUT_FINISHED;
            msgsnd(barber_respond_id, &msg_arg, sizeof(msg_arg), 0);
            printf("理发师 %d 睡眠\n", barber_num);
        }
        if (msgrcv(account_quest_id, &msg_arg, sizeof(msg_arg), ACCOUNTQUEST, IPC_NOWAIT) >= 0) {
            sem_wait(account_sem);
            printf("理发师 %d 给顾客 %d 结账\n", barber_num, msg_arg.mid);
            sleep(rate);
            printf("顾客 %d 结账完毕\n", msg_arg.mid);
            msg_arg.mtype = ACC_FINISHED;
            msgsnd(account_respond_id, &msg_arg, sizeof(msg_arg), 0);
            sem_signal(account_sem);
            printf("理发师 %d 睡眠\n", barber_num);
        }
    }

    return EXIT_SUCCESS;
}