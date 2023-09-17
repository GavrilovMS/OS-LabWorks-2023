#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstring>
#include <semaphore.h>

struct targs{
    int flag;
    char symb;
    sem_t* sem;
};

void* proc1(void *arg){
    std::cout << "Thread 1 was started." << std::endl;
    sleep(1);
    
    targs *args = (targs*) arg;
    while (!args->flag){
        sem_wait(args->sem);

        for(int i = 0; i < 10; i++){
            if (args->flag) break;
            putchar(args->symb);
            fflush(stdout);
            sleep(1);
        }

        sem_post(args->sem);
        sleep(1);
    }

    std::cout << "Thread 1 was finished." << std::endl;
    pthread_exit((void*) 0);
}

void* proc2(void *arg){
    std::cout << "Thread 2 was started." << std::endl;
    sleep(1);

    targs *args = (targs*) arg;
    while (!args->flag){
        sem_wait(args->sem);

        for(int i = 0; i < 10; i++){
            if (args->flag) break;
            putchar(args->symb);
            fflush(stdout);
            sleep(1);
        }

        sem_post(args->sem);
        sleep(1);
    }


    std::cout  << "Thread 2 was finished." << std::endl;
    pthread_exit((void*) 0);
}


int main(){

    std::cout << "Program was started." << std::endl;

    pthread_t id1;
    pthread_t id2;

    sem_t sem;
    int shared = 0;
    unsigned int value = 1;
    
    sem_init(&sem, shared, value);

    targs args1;
    args1.flag = 0;
    args1.symb = '1';
    args1.sem = &sem;

    targs args2;
    args2.flag = 0;
    args2.symb = '2';
    args2.sem = &sem;

    pthread_create(&id1, NULL, proc1, &args1);
    pthread_create(&id2, NULL, proc2, &args2);

    std::cout << "Program is waiting for the Enter to be pressed." << std::endl;
    getchar();
    std::cout << "Enter was pressed." << std::endl << std::endl;

    args1.flag = 1;
    args2.flag = 1;

    int *exitcode1;
    int *exitcode2;

    pthread_join(id1, (void**) exitcode1);
    pthread_join(id2, (void**) exitcode2);
    sem_destroy(&sem);

    std::cout << "Thread 1 finished work with exitcode: " << exitcode1 << '.' << std::endl;
    std::cout << "Thread 2 finished work with exitcode: " << exitcode2 << '.' << std::endl;
    std::cout << "Program was finished." << std::endl;
    return 0;
}