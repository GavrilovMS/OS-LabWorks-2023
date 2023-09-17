#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <grp.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <cstring>

struct targs{
    int flag;
    int msgid;
};

typedef struct {
    long mtype;
    char buff[256];
}TMessage;

void* proc(void *arg){
    std::cout << "[thread2]: Thread was started." << std::endl;
    targs *args = (targs*) arg;

    while (!args->flag){
        TMessage message;
        message.mtype = 1;
        memset(message.buff, 0, sizeof(message.buff));
        int result = msgrcv(args->msgid, &message, sizeof(message.buff), message.mtype, IPC_NOWAIT);
        if (result == -1){
            perror("[thread1]");
        } else {
            std::cout << "[thread2]: Group name: " << message.buff << std::endl;
        }
        sleep(1);
    }

    std::cout << "[thread2]: Thread was finished." << std::endl;
    pthread_exit((void*) 0);
}

int main(){
    std::cout << "[prog2]: Program was started." << std::endl;

    int key = ftok("lab7OS", 'A');
    int msgid = msgget(key, 0);
    if (msgid < 0){
        msgid = msgget(key, IPC_CREAT | 0644);
    }
    pthread_t thread_id;
    targs args;
    args.msgid = msgid;
    args.flag = 0;
    pthread_create(&thread_id, NULL, proc, &args);

    std::cout << "[prog2]: Program is waiting for the Enter to be pressed." << std::endl << std::endl;
    getchar();
    std::cout << "[prog2]: Enter was pressed." << std::endl << std::endl;
    args.flag = 1;
    
    msgctl(msgid, IPC_RMID, NULL);
    int *exitcode;
    pthread_join(thread_id, (void**) exitcode);

    std::cout << "[prog2]: Thread finished work with exitcode: " << exitcode << '.' << std::endl;
    std::cout << "[prog2]: Program was finished." << std::endl;
    return 0;
}