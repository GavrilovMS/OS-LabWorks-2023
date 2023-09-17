#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <grp.h>


struct targs{
    int flag;
    int file_ds;
};

void* proc_write(void *arg){
    std::cout << "[write]: Writing thread was started." << std::endl;
    targs *args = (targs*) arg;

    void* buf;
    char gr_name[] = "man";
    while (!args->flag){
        std::cout << "[write]: Getting data..." << std::endl << std::endl;
        buf = (void*)getgrnam(gr_name);
        group* gr = (group*)buf;
        std::cout << "[write]: Group name: " << gr->gr_name << std::endl;
        std::cout << "[write]: Group password: " << gr->gr_passwd << std::endl;
        std::cout << "[write]: Group ID: " << gr->gr_gid << std::endl << std::endl;

        ssize_t wr_flag = write(args->file_ds, buf, sizeof(group));
        if (wr_flag == -1) {
            perror("[write]");
        }
        sleep(1);
    }

    std::cout << "[write]: Writing thread was finished." << std::endl;
    pthread_exit((void*) 0);
}

void* proc_read(void *arg){
    std::cout << "[read]: Reading thread was started." << std::endl;
    targs *args = (targs*) arg;
    
    char* buf;
    while (!args->flag){
        buf = new char[sizeof(group)];
        std::cout << "[read]: Reading data..." << std::endl << std::endl;
        ssize_t rd_flag = read(args->file_ds, buf, sizeof(group));

        if (rd_flag == -1) {
            perror("[read]");
        } else {
            std::cout << "[read]: Group name: " << ((group*)buf)->gr_name << std::endl;
            std::cout << "[read]: Group password: " << ((group*)buf)->gr_passwd << std::endl;
            std::cout << "[read]: Group ID: " << ((group*)buf)->gr_gid << std::endl << std::endl;
        }
        delete[] buf;
        sleep(1);
    }

    std::cout  << "[read]: Reading thread was finished." << std::endl;
    pthread_exit((void*) 0);
}


int main(){

    std::cout << "[main]: Program was started." << std::endl;

    pthread_t id1;
    pthread_t id2;

    
    int filedes[2];
    int pipe_id = pipe(filedes); 
    
    //int pipe_id = pipe2(filedes, O_NONBLOCK);
    
    //fcntl(filedes[0], F_SETFL, O_NONBLOCK);
    //fcntl(filedes[1], F_SETFL, O_NONBLOCK);

    targs args_write;
    args_write.flag = 0;
    args_write.file_ds = filedes[1];
    targs args_read;
    args_read.flag = 0;
    args_read.file_ds = filedes[0];

    pthread_create(&id1, NULL, proc_write, &args_write);
    pthread_create(&id2, NULL, proc_read, &args_read);


    std::cout << "[main]: Program is waiting for the Enter to be pressed." << std::endl << std::endl;
    getchar();
    std::cout << "[main]: Enter was pressed." << std::endl << std::endl;

    args_write.flag = 1;
    args_read.flag = 1;

    int *exitcode1;
    int *exitcode2;

    pthread_join(id1, (void**) exitcode1);
    pthread_join(id2, (void**) exitcode2);

    close(filedes[0]);
    close(filedes[1]);

    std::cout << "[main]: Writing thread finished work with exitcode: " << exitcode1 << '.' << std::endl;
    std::cout << "[main]: Reading thread finished work with exitcode: " << exitcode2 << '.' << std::endl;
    std::cout << "[main]: Program was finished." << std::endl;
    return 0;
}