#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <vector>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>


int listening_socket_id;
int client_socket_id;
std::vector<std::string> handle_que;

int waitingConnection_flag = 0;
int handle_flag = 0;
int rcv_flag = 0;

pthread_t waitingConnection_id;
pthread_t handle_id;
pthread_t rcv_id;
pthread_mutex_t mutex_id;

void* rcv(void *arg){
    std::cout << "[rcv]: Thread was started." << std::endl;
    char* rcvbuf = new char[16];

    while (!rcv_flag){
        delete[] rcvbuf;
        char* rcvbuf = new char[16];

        int reccount = recv(client_socket_id, rcvbuf, sizeof(rcvbuf), 0);
        if (reccount == -1){
            perror("[rcv]");
            sleep(1);
        } else {
            if(!pthread_mutex_trylock(&mutex_id)) {;
                std::string str(rcvbuf);
                handle_que.push_back(str);
                std::cout << "[rcv]: " << str << std::endl;
                pthread_mutex_unlock(&mutex_id);
                sleep(1);
            }
        }   
    }
    
    delete[] rcvbuf;
    std::cout << "[rcv]: Thread was finished." << std::endl;
    pthread_exit((void*) 0);
}

void* handle(void *arg){
    std::cout << "[handle]: Thread was started." << std::endl;
    char gr_name[] = "man";
    int num_of_snd = 1;
    while (!handle_flag){
        if(!pthread_mutex_trylock(&mutex_id)){;
            while (!handle_que.empty() && !handle_flag){
                std::string str = handle_que.front();
                handle_que.erase(handle_que.begin());
                group* gr = getgrnam(gr_name);
                std::string snd = str + " snd " + std::to_string(num_of_snd++) + ": " + (char*)gr->gr_name; 
                const char* sndbuf = snd.c_str();
                int sentcount = send(client_socket_id, sndbuf, snd.length(), 0);
                if (sentcount == -1) {
                    perror("[handle]");
                } else {
                    std::cout << "[handle]: " << snd << std::endl;
                }
            } 
            pthread_mutex_unlock(&mutex_id);
            sleep(1);
        }
    }

    std::cout << "[handle]: Thread was finished." << std::endl;
    pthread_exit((void*) 0);
}

void* waitingConnection(void *arg){
    std::cout << "[waitingConnection]: Thread was started." << std::endl;
    struct sockaddr* client_addr;

    while (!waitingConnection_flag){
        client_socket_id = accept(listening_socket_id, client_addr, (socklen_t*)sizeof(client_addr));
        if (client_socket_id == -1){
            perror("[waitingConnection]");
            sleep(1);
        } else {
            pthread_create(&rcv_id, NULL, rcv, NULL);
            pthread_create(&handle_id, NULL, handle, NULL);
            break;
        }
    }

    std::cout << "[waitingConnection]: Thread was finished." << std::endl;
    pthread_exit((void*) 0);
}

int main(){
    std::cout << "[server]: Program was started." << std::endl;
    struct sockaddr_in serverAddr;
    serverAddr.sin_family =      AF_INET;
    serverAddr.sin_port =        htons(7001);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    pthread_mutex_init(&mutex_id, NULL);

    listening_socket_id = socket(AF_INET, SOCK_STREAM, 0);
    bind(listening_socket_id, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    int optval = 1;
    setsockopt(listening_socket_id, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    fcntl(listening_socket_id, F_SETFL, O_NONBLOCK);
    listen(listening_socket_id, 1);
 
    pthread_create(&waitingConnection_id, NULL, waitingConnection, NULL);

    std::cout << "[server]: Program is waiting for the Enter to be pressed." << std::endl << std::endl;
    getchar();
    std::cout << "[server]: Enter was pressed." << std::endl << std::endl;
    
    rcv_flag = 1;
    handle_id = 1;
    waitingConnection_flag = 1;

    int *exitcode;
    pthread_join(rcv_id, (void**) exitcode);
    std::cout << "[server]: rcv finished work with exitcode: " << exitcode << '.' << std::endl;
    pthread_join(handle_id, (void**) exitcode);
    std::cout << "[server]: handle finished work with exitcode: " << exitcode << '.' << std::endl;
    pthread_join(waitingConnection_id, (void**) exitcode);
    std::cout << "[server]: waitingConnection finished work with exitcode: " << exitcode << '.' << std::endl;

    shutdown(client_socket_id, 2);
    close(listening_socket_id);
    close(client_socket_id);

    pthread_mutex_destroy(&mutex_id);

    std::cout << "[server]: Program was finished." << std::endl;
    return 0;
}