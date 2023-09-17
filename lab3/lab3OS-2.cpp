#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>

int main(){
    std::cout << "Program 2 was started.\n [prog2] Id: " << getpid() << " ParentId: " << getppid() << std::endl << std::endl;

    char args[4];
    args[0] = '0';
    args[1] = '1';
    args[2] = '2';
    args[3] = '3';

    int status;
    int options = WNOHANG;

    pid_t pid = fork();

    if (!pid){
        execl("lab3OS-1", &args[0], &args[1], &args[2], &args[3], NULL);
    } else if (pid > 0){
        sleep(1);    
        while (!waitpid(pid, &status, options)) {
            std::cout << "[prog2] Waiting for prog1 to be finished." << std::endl;
            sleep(1);
        }
        std::cout << "[prog2] Program 1 was finished with exitcode: " << WEXITSTATUS(status) << '.' << std::endl;
    } else if (pid == -1){
        perror("fork");
    }
    
    std::cout << "Program 2 was finished with exitcode: 0." << std::endl;
    return 0;
}