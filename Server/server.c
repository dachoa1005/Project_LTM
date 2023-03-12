#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/types.h>
#include "user.h"
#include "authenticate.h"
#include "room.h"
#include "handler.h"

extern User *users;
extern Room *rooms;
#define IP_ADDRESS "127.0.0.1"
#define PORT 8888
#define MAX_CLIENTS 10

int main(int argc, char *argv[])
{  
    // Init user array from file to array: (/File/user.txt)
    readUsersFromFile(&users, "../File/user.txt");
    printAllUsers(users);
    init_rooms();

    // Create socket
    int server_sockfd, client_sockfd;
    int addrlen = sizeof(struct sockaddr_in);
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd == -1)
    {
        printf("Could not create socket");
        exit(EXIT_FAILURE);
    }
    printf ("Socket created successfully\n");

    // configure socket
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    server_addr.sin_port = htons(PORT);

    // Bind socket to port
    int bind_status = bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (bind_status < 0)
    {
        printf("Bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket bind successfully\n");

    // Listen for incoming connections
    int listen_status = listen(server_sockfd, MAX_CLIENTS);
    if (listen_status < 0)
    {
        printf("Listen failed");
        exit(EXIT_FAILURE);
    }

    int thread_count = 0;
    // Accept incoming connections
    while (1)
    {
        printf("Listening for incoming connections on port %d\n", PORT);
        client_sockfd = accept(server_sockfd, (struct sockaddr *)&server_addr, (socklen_t*)&addrlen);
        if (client_sockfd < 0)
        {
            printf("Accept failed");
            exit(EXIT_FAILURE);
        }
        printf("Connection has been established with client on socket %d\n", client_sockfd);

        // create thread to handle connection - register or login
        pthread_t thread[10];
        if (pthread_create(&thread[thread_count], NULL, connection_handle, (void *)&client_sockfd) < 0)
        {
            printf("Could not create thread");
            exit(EXIT_FAILURE);
        }
        thread_count++;
    }
}