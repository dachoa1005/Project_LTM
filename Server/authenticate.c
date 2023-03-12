#include "authenticate.h"
#include "user.h"
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

int authenticate(char *username, char *password)
{
    /* return value:
    0: user not exist
    1: locked
    2: wrong password
    3: already logged in
    4: login successfully
    */

    User *user = searchUser(users, username);
    if (user == NULL)
    {
        return 0; // user not exist
    }
    // first, check if user is locked
    if (user->status == 0)
    {
        return 1; // locked
    }

    // check if user is already logged in
    if (user->login_status == 1)
    {
        return 2; // already logged in
    }

    if (strcmp(user->password, password) != 0)
    {
        // increase login attempts
        user->login_attempts++;
        if (user->login_attempts >= 3) // lock user
        {
            user->status = 0;
            user->login_attempts = 0;
        }
        // update to file
        writeAllUsersToFile(users, "../File/user.txt");
        return 3; // wrong password
    }


    // login successfully
    user->login_status = 1;
    user->login_attempts = 0;
    return 4; // login successfully
}

void *connection_handle(void *client_socket)
{
    int client_sockfd = *(int *)client_socket;
    char buffer[1024];
    int read_len;
    char response[5];
    // first, handle login process
    do
    {
        read_len = recv(client_sockfd, buffer, 1024, 0);
        if (read_len == 0)
        {
            printf("Client on socket %d disconnected\n", client_sockfd);
            break;
        }

        buffer[read_len] = '\0'; // add null terminator
        // receive format: REQUEST_TYPE|USERNAME|PASSWORD
        // send format: RESPONSE_TYPE
        char *request_type = strtok(buffer, "|");
        char *username = strtok(NULL, "|");
        char *password = strtok(NULL, "|");

        int response_type = authenticate(username, password);

        if (strcmp(request_type, "LOGINREQ") == 0)
        {
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

        // register new user
        if (strcmp(request_type, "REGISREQ") == 0)
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