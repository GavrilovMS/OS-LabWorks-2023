#include <iostream>
#include <cstring>
#include <unistd.h>

int main(int argc, char* argv[]){
    std::cout << "Program 1 was started.\n [prog1] Id: " << getpid() << " ParentId: " << getppid() << std::endl << std::endl;
    
    for (size_t i = 0; i < argc; i++){
        std::cout << "[prog1] " << *argv[i] << std::endl;
        sleep(1);
    }

    std::cout << "[prog1] Program 1 was finished with exitcode: 10." << std::endl;
    exit(10);
}