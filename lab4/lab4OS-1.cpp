#include <iostream>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

int kbhit()
{   
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}




int main(){
    std::cout << "Program 1 was started." << std::endl << "Program 1 is waiting for any button to be pressed." << std::endl;
    
    sem_t* sem;
    char sem_name[] = "SemForLab4";
    int file_ds;

    sem = sem_open(sem_name, O_CREAT, 0644, 1);
    file_ds = open("file.txt", O_CREAT | O_WRONLY | O_APPEND | O_NONBLOCK);

    while(!kbhit()){
        std::cout << "Program 1 is waiting for the semaphore to be free." << std::endl;
        sem_wait(sem);
        std::cout << "Program 1 has blocked the semaphore." << std::endl;
        for (int i = 0; i < 10; i++){
            if (kbhit()) break;
            char symb = '1';
            putchar(symb);
            fflush(stdout);
            write(file_ds, &symb, 1);
            sleep(1);
        }
        std::cout << std::endl;
        sem_post(sem);
        sleep(1);
    }

    std::cout << std::endl;

    close(file_ds);
    sem_close(sem);
    sem_unlink(sem_name);
    std::cout << "Program 1 was finished with exitcode: 0" << std::endl;
    return 0;
}
