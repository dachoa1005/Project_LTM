#include "user.h"
#include "authenticate.h"
#include "room.h"
#include "handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>

extern User *users;
extern Room *rooms;

char response[5];


void *connection_handle(void *client_socket)
{
    int client_sockfd = *(int *)client_socket;
    char buffer[1024];
    int read_len;
    // first, handle login process
    do
    {
        read_len = recv(client_sockfd, buffer, 1024, 0);
        if (read_len == 0)
        {
            printf("Client on socket %d disconnected\n", client_sockfd);
            break;
        }
        printf("buffer: %s\n", buffer);
        buffer[read_len] = '\0'; // add null terminator
        // receive format: REQUEST_TYPE|USERNAME|PASSWORD
        // send format: RESPONSE_TYPE
        char *request_type = strtok(buffer, "|");
        char *username = strtok(NULL, "|");
        char *password = strtok(NULL, "|");

        if (strcmp(request_type, "LOGINREQ") == 0)
        {
            login_handle(username, password, client_sockfd);
        }

        // register new user
        if (strcmp(request_type, "REGISREQ") == 0)
        {
            register_handle(username, password, client_sockfd);
        }

        // logout
        if (strcmp(request_type, "LOGOUTREQ") == 0)
        {
            logout_handle(username, password, client_sockfd);
        }

        // handle GETROOMREQ|username
        if (strcmp(request_type, "GETROOMREQ") == 0)
        {
            // send all room id + difficulty + current numbers of players in each room to client
            get_room_handle(username, client_sockfd);
        }


    } while (read_len > 0);

    // close socket
    close(client_sockfd);
    // update loggin status for current disconnected user using socketfd
    User *current = users;
    while (current != NULL)
    {
        if (current->socketfd == client_sockfd)
        {
            current->login_status = 0;
            current->socketfd = -1;
            break;
        }
        current = current->next;
    }

    // print all Current logged in users
    printf("Current logged in users: ");
    current = users;
    while (current != NULL)
    {
        if (current->login_status == 1)
        {
            printf("%s ", current->username);
        }
        current = current->next;
    }
    printf("\n");
}

void login_handle(char *username, char *password, int client_sockfd)
{
    int response_type = authenticate(username, password);
    switch (response_type)
    /*
    authen response:
    0: user not exist
    1: locked
    2: already logged in
    3: wrong password
    4: login successfully
    */
    {
    case 0:
    {
        // user not exist
        strcpy(response, "NOEX");
        send(client_sockfd, response, sizeof(response), 0);
        break;
    }
    case 1:
    {
        // locked
        strcpy(response, "LOCK");
        send(client_sockfd, response, sizeof(response), 0);
        break;
    }
    case 2:
    {
        // already logged in
        strcpy(response, "ALOG");
        send(client_sockfd, response, sizeof(response), 0);
        break;
    }
    case 3:
    {
        // wrong password
        strcpy(response, "FAIL");
        send(client_sockfd, response, sizeof(response), 0);
        break;
    }
    case 4:
    {
        // login successfully
        // change user->socketfd
        User *user = searchUser(users, username);
        user->socketfd = client_sockfd;
        user->login_status = 1;
        strcpy(response, "SUCC");
        send(client_sockfd, response, sizeof(response), 0);

        // print all Current logged in users
        printf("Current logged in users: ");
        User *current = users;
        while (current != NULL)
        {
            if (current->login_status == 1)
            {
                printf("%s ", current->username);
            }
            current = current->next;
        }
        printf("\n");

        break;
    }
    default:
        break;
    }
}

void register_handle(char *username, char *password, int client_sockfd)
{
    // check if user already exist
    User *user = searchUser(users, username);
    if (user != NULL)
    {
        // user already exist
        strcpy(response, "DUPL"); // duplicate
        send(client_sockfd, response, sizeof(response), 0);
    }
    else
    {
        // add new user
        User *new_user = (User *)malloc(sizeof(User));
        strcpy(new_user->username, username);
        strcpy(new_user->password, password);
        new_user->status = 1;
        new_user->login_status = 1;
        new_user->login_attempts = 0;
        new_user->socketfd = client_sockfd;
        new_user->next = NULL;
        addUser(&users, new_user);
        // print all users
        printf("Current users: ");
        User *current = users;
        while (current != NULL)
        {
            printf("%s ", current->username);
            current = current->next;
        }
        printf("\n");

        // update to file
        writeNewUserToFile(new_user, "../File/user.txt");
        strcpy(response, "SUCC");
        send(client_sockfd, response, sizeof(response), 0);
        // print all Current logged in users
        printf("Current logged in users: ");
        current = users;
        while (current != NULL)
        {
            if (current->login_status == 1)
            {
                printf("%s ", current->username);
            }
            current = current->next;
        }
        printf("\n");
    }
}

void logout_handle(char *username, char *password, int client_sockfd)
{
    // update user->login_status using socketfd
    User *user = users;
    while (user != NULL)
    {
        if (user->socketfd == client_sockfd)
        {
            user->login_status = 0;
            user->socketfd = -1;
            break;
        }
        user = user->next;
    }
    // print all Current logged in users
    printf("Current logged in users: ");
    user = users;
    while (user != NULL)
    {
        if (user->login_status == 1)
        {
            printf("%s ", user->username);
        }
        user = user->next;
    }
    printf("\n");
}

void get_room_handle(char *username, int client_sockfd)
{
    char room_handle_response[100];
    // send all room id + difficulty + current numbers of players in each room to client
    Room *current = rooms;
    while (current != NULL)
    {
        memset(room_handle_response, 0, sizeof(room_handle_response));
        // send format: ROOM|room_id|difficulty|current_numbers_of_players
        strcpy(room_handle_response, "ROOM|");
        char room_id[10];
        sprintf(room_id, "%d", current->id);
        strcat(room_handle_response, room_id);
        strcat(room_handle_response, "|");
        char difficul[10];
        sprintf(difficul, "%d", current->difficulty);
        strcat(room_handle_response, difficul);
        strcat(room_handle_response, "|");
        char num[10];
        sprintf(num, "%d", current->current_number_users);
        strcat(room_handle_response, num);
        printf("room_handle_response: %s\n", room_handle_response);
        send(client_sockfd, room_handle_response, sizeof(room_handle_response), 0);
        current = current->next;
    }
    // send END to client
    strcpy(room_handle_response, "END");
    send(client_sockfd, room_handle_response, sizeof(room_handle_response), 0);
}