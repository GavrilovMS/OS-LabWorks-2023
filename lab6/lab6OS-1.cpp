#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <grp.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <cstring>

struct targs{
    int flag;
    sem_t* sem_write;
    sem_t* sem_read;
    group* shm;
};

group* local_adress;



void* proc(void *arg){
    std::cout << "[thread1]: Thread was started." << std::endl;
    targs *args = (targs*) arg;

    char gr_name[] = "man";
    char  buf[256];

    while (!args->flag){
       // sem_wait(args->sem_write);
        std::cout << "[thread1]: Getting data..." << std::endl << std::endl;
        group* gr = getgrnam(gr_name);
        std::cout << "[thread1]: Group name: " << gr->gr_name << std::endl;
        std::cout << "[thread1]: Group password: " << gr->gr_passwd << std::endl;
        std::cout << "[thread1]: Group ID: " << gr->gr_gid << std::endl << std::endl;

        memset(buf,0,256);
        strcpy(buf, gr->gr_name);

        memcpy(args->shm, buf, 256);

      // memcpy(local_adress, gr, sizeof(group));

        fflush(stdout);
        //sem_post(args->sem_read);
        sem_post(args->sem_write);
        sem_wait(args->sem_read);
        sleep(1);
    }

    std::cout << "[thread1]: Thread was finished." << std::endl;
    pthread_exit((void*) 0);
}

int main(){
    std::cout << "[prog1]: Program was started." << std::endl;
    char shm_name[] = "ShmForLab6";
    int shm_id = shm_open(shm_name, O_CREAT | O_RDWR, 0644);
    size_t shm_length = sizeof(group);
    ftruncate(shm_id, 256);//shm_length); 
    local_adress = (group*) mmap(0, 256, PROT_WRITE | PROT_READ, MAP_SHARED, shm_id, 0);

    sem_t* sem_write_id, *sem_read_id;
    char sem_write_name[] = "WriteSemForLab6";
    char sem_read_name[] = "ReadSemForLab6";
    sem_write_id = sem_open(sem_write_name, O_CREAT, 0644, 0);
    sem_read_id = sem_open(sem_read_name, O_CREAT, 0644,0);
    
  //  int semStatus;
  //  sem_getvalue(sem_read_id, &semStatus);
  //  if (semStatus == 1){
  //      sem_wait(sem_read_id);
  //  }

    pthread_t thread_id;
    targs args;
    args.flag = 0;
    args.sem_read = sem_read_id;
    args.sem_write = sem_write_id;
    args.shm = (group*) local_adress;
    pthread_create(&thread_id, NULL, proc, &args);

    std::cout << "[prog1]: Program is waiting for the Enter to be pressed." << std::endl << std::endl;
    getchar();
    std::cout << "[prog1]: Enter was pressed." << std::endl << std::endl;
    args.flag = 1;
    
    int *exitcode;
    pthread_join(thread_id, (void**) exitcode);

    sem_close(sem_write_id);
    sem_unlink(sem_write_name);
    sem_close(sem_read_id);
    sem_unlink(sem_read_name);

    munmap(local_adress, shm_length);
    close(shm_id);
    shm_unlink(shm_name);
    std::cout << "[prog1]: Reading thread finished work with exitcode: " << exitcode << '.' << std::endl;
    std::cout << "[prog1]: Program was finished." << std::endl;
    return 0;
}