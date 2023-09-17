#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <grp.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>


struct sockaddr_in serverAddr;
int socket_id;
int connection_flag = 0;
int snd_flag = 0;
int rcv_flag = 0;

pthread_t connection_id;
pthread_t snd_id;
pthread_t rcv_id;


void* snd(void *arg){
    std::cout << "[snd]: Thread was started." << std::endl;
    int num_of_req = 1;
    
    while (!snd_flag){
        std::string req = "req " + std::to_string(num_of_req++);
        const char* reqbuf = req.c_str();
        int sentcount = send(socket_id, reqbuf, req.length(), 0);
        if (sentcount == -1) {
            perror("[snd]");
        } else {
            std::cout << "[snd]: " << req << std::endl; 
        }
        sleep(1);
    }

    std::cout << "[snd]: Thread was finished." << std::endl;
    pthread_exit((void*) 0);
}

void* rcv(void *arg){
    std::cout << "[rcv]: Thread was started." << std::endl;
    char* rcvbuf = new char[16];

    while (!rcv_flag){
        delete[] rcvbuf;
        char* rcvbuf = new char[16];
        int reccount = recv(socket_id, rcvbuf, 2*sizeof(rcvbuf), 0);

        if (reccount == -1){
            perror("[rcv]");
            sleep(1);
        } else {
            std::cout << "[rcv]: " << std::string(rcvbuf) << std::endl;
        }
    }

    delete[] rcvbuf;
    std::cout << "[rcv]: Thread was finished." << std::endl;
    pthread_exit((void*) 0);
}

void* connection(void *arg){
    std::cout << "[connection]: Thread was started." << std::endl;
    
    while (!connection_flag){
        int result = connect(socket_id, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        if (result == -1){
            perror("[connection]");
            sleep(1);
        } else {
            std::cout << "[connection]: Sucess." << std::endl;
            pthread_create(&snd_id, NULL, snd, NULL);
            pthread_create(&rcv_id, NULL, rcv, NULL);
            break;
        }
    }

    std::cout << "[connection]: Thread was finished." << std::endl;
    pthread_exit((void*) 0);
}

int main(){
    std::cout << "[client]: Program was started." << std::endl;
    serverAddr.sin_family =      AF_INET;
    serverAddr.sin_port =        htons(7001);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    socket_id = socket(AF_INET, SOCK_STREAM, 0);;
    
    pthread_create(&connection_id, NULL, connection, NULL);

    std::cout << "[client]: Program is waiting for the Enter to be pressed." << std::endl << std::endl;
    getchar();
    std::cout << "[client]: Enter was pressed." << std::endl << std::endl;
    
    
    snd_flag = 1;
    rcv_flag = 1;
    connection_flag = 1;
    int *exitcode;
    pthread_join(connection_id, (void**) exitcode);
    std::cout << "[client]: connection finished work with exitcode: " << exitcode << '.' << std::endl;
    pthread_join(snd_id, (void**) exitcode);
    std::cout << "[client]: snd finished work with exitcode: " << exitcode << '.' << std::endl;
    pthread_join(rcv_id, (void**) exitcode);
    std::cout << "[client]: rcv finished work with exitcode: " << exitcode << '.' << std::endl;

    shutdown(socket_id, 2);
    close(socket_id);

    std::cout << "[client]: Program was finished." << std::endl;
    return 0;
}