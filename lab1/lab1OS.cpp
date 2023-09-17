#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstring>

struct targs{
    int flag;
    char symb;
};

void* proc1(void *arg){
    std::cout << "Thread 1 was started." << std::endl;
    sleep(1);

    targs *args = (targs*) arg;
    while (!args->flag){
        putchar(args->symb);
        fflush(stdout);
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
        putchar(args->symb);
        fflush(stdout);
        sleep(1);
    }

    std::cout  << "Thread 2 was finished." << std::endl;
    pthread_exit((void*) 0);
}


int main(){

    std::cout << "Program was started." << std::endl;

    pthread_t id1;
    pthread_t id2;

    targs args1;
    args1.flag = 0;
    args1.symb = '1';

    targs args2;
    args2.flag = 0;
    args2.symb = '2';

    pthread_create(&id1, NULL, proc1, &args1);
    pthread_create(&id2, NULL, proc2, &args2);

    char* name1 = new char[31];
    int rc;
     
    rc = pthread_getname_np(id1, name1, 31);
    std::cout << "Thread 1 current name: " << name1 << '.' << std::endl;
    strcpy(name1, "THREAD1");
    rc = pthread_setname_np(id1, name1);
    rc = pthread_getname_np(id1, name1, 31);
    std::cout << "Thread 1 new name: " << name1 << '.' << std::endl << std::endl;
    delete[] name1;

    std::cout << "Program is waiting for the Enter to be pressed." << std::endl;
    getchar();
    std::cout << "Enter was pressed." << std::endl << std::endl;

    args1.flag = 1;
    args2.flag = 1;

    int *exitcode1;
    int *exitcode2;

    pthread_join(id1, (void**) exitcode1);
    pthread_join(id2, (void**) exitcode2);

    std::cout << "Thread 1 finished work with exitcode: " << exitcode1 << '.' << std::endl;
    std::cout << "Thread 2 finished work with exitcode: " << exitcode2 << '.' << std::endl;
    std::cout << "Program was finished." << std::endl;
    return 0;
}
