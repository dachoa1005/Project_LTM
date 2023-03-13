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
        memset(buffer, 0, sizeof(buffer));
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

        if (strcmp(request_type, "LOGINREQ") == 0)
        {
            char *password = strtok(NULL, "|");
            login_handle(username, password, client_sockfd);
        }

        // register new user
        if (strcmp(request_type, "REGISREQ") == 0)
        {
            char *password = strtok(NULL, "|");
            register_handle(username, password, client_sockfd);
        }

        // logout
        if (strcmp(request_type, "LOGOUTREQ") == 0)
        {
            char *password = strtok(NULL, "|");
            logout_handle(username, password, client_sockfd);
        }

        // handle GETROOMREQ|username
        if (strcmp(request_type, "GETROOMREQ") == 0)
        {
            // send all room id + difficulty + current numbers of players in each room to client
            get_room_handle(username, client_sockfd);
        }

        // handle JOINROOMREQ|username|room_id
        if (strcmp(request_type, "JOINROOMREQ") == 0)
        {
            int room_id = atoi(strtok(NULL, "|"));
            join_room_handle(username, room_id, client_sockfd);
        }

    } while (read_len > 0);

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

    // close socket
    close(client_sockfd);
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
        if (current->id == 0)
        {
            current = current->next;
            continue;
        }
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
        room_handle_response[strlen(room_handle_response)] = '\0';
        printf("room_handle_response: %s\n", room_handle_response);
        send(client_sockfd, room_handle_response, sizeof(room_handle_response), 0);
        current = current->next;
    }
    // send END to client
    strcpy(room_handle_response, "END");
    printf("room_handle_response: %s\n", room_handle_response);
    send(client_sockfd, room_handle_response, sizeof(room_handle_response), 0);
}

void join_room_handle(char *username, int room_id, int socketfd)
{
    // check if room exist
    Room *room = search_room(rooms, room_id);
    if (room == NULL)
    {
        // room not exist
        strcpy(response, "NOEX"); // room not exist
        send(socketfd, response, sizeof(response), 0);
    }
    else
    {
        // check if room is full
        if (room->current_number_users == 2)
        {
            // room is full
            strcpy(response, "FULL"); // room is full
            send(socketfd, response, sizeof(response), 0);
        }
        else
        {
            // add user to room
            User *user = searchUser(users, username);
            user->current_room_id = room_id;
            // room->current_number_users++;
            add_user_to_room(room, user);
            // send SUCC to client
            strcpy(response, "SUCC");
            send(socketfd, response, sizeof(response), 0);
            // change room state
            if (room->current_number_users == 1)
            {
                room->state = WAITING;
            }
            else if (room->current_number_users == 2)
            {
                room->state = PLAYING;
                // send START to both users in room
                strcpy(response, "STAR");

                send(room->room_users[0].socketfd, response, sizeof(response), 0);
                send(room->room_users[1].socketfd, response, sizeof(response), 0);

                // send question to both users in room
                // question format: QUES|question1|questioncontent|question_answer|question2|questioncontent|question_answer|...
                char question[1000];
                memset(question, 0, sizeof(question));
                strcpy(question, "QUES|");
                int current_question = 0;
                while (current_question < room->num_questions)
                {
                    strcat(question, room->questions[current_question].content);
                    strcat(question, "|");
                    strcat(question, room->questions[current_question].answer);
                    strcat(question, "|");
                    current_question++;
                }
                question[strlen(question)] = '\0';
                printf("question: %s\n", question);
                send(room->room_users[0].socketfd, question, sizeof(question), 0);
                send(room->room_users[1].socketfd, question, sizeof(question), 0);

                // recive users's point from both users in room
                // point format: PONT|point
                int point0, point1; // point of room->room_users[0] and room->room_users[1]
                char point[100];
                memset(point, 0, sizeof(point));
                recv(room->room_users[0].socketfd, point, sizeof(point), 0);
                printf("User 1: %s point: %s\n", room->room_users[0].username, point);
                point0 = atoi(point);
                memset(point, 0, sizeof(point));
                recv(room->room_users[1].socketfd, point, sizeof(point), 0);
                printf("User 2: %s point: %s\n", room->room_users[1].username, point);
                point1 = atoi(point);

                if (point0 > point1)
                {
                    // send WINN to room->room_users[0]
                    strcpy(response, "WINN");
                    send(room->room_users[0].socketfd, response, sizeof(response), 0);
                    // send LOSE to room->room_users[1]
                    strcpy(response, "LOSE");
                    send(room->room_users[1].socketfd, response, sizeof(response), 0);
                    // update user's point
                    User *user0 = searchUser(users, room->room_users[0].username);
                    user0->points += 20;
                    User *user1 = searchUser(users, room->room_users[1].username);
                    user1->points -= 15;
                    writeAllUsersToFile(users, "../File/user.txt");
                }
                else if (point0 < point1)
                {
                    // send WINN to room->room_users[1]
                    strcpy(response, "WINN");
                    send(room->room_users[1].socketfd, response, sizeof(response), 0);
                    // send LOSE to room->room_users[0]
                    strcpy(response, "LOSE");
                    send(room->room_users[0].socketfd, response, sizeof(response), 0);
                    // update user's point
                    User *user0 = searchUser(users, room->room_users[0].username);
                    user0->points -= 15;
                    User *user1 = searchUser(users, room->room_users[1].username);
                    user1->points += 20;
                    writeAllUsersToFile(users, "../File/user.txt");
                }
                else
                {
                    // send DRAW to both users
                    strcpy(response, "DRAW");
                    send(room->room_users[0].socketfd, response, sizeof(response), 0);
                    send(room->room_users[1].socketfd, response, sizeof(response), 0);
                    // update user's point
                    User *user0 = searchUser(users, room->room_users[0].username);
                    user0->points += 15;
                    User *user1 = searchUser(users, room->room_users[1].username);
                    user1->points += 15;
                    writeAllUsersToFile(users, "../File/user.txt");
                }
                // change room state
                room->state = WAITING;
                // remove user from room
                User *user0 = searchUser(users, room->room_users[0].username);
                user0->current_room_id = -1;
                User *user1 = searchUser(users, room->room_users[1].username);
                user1->current_room_id = -1;
                remove_user_from_room(room, user0);
                remove_user_from_room(room, user1);

            }
        }
    }
}
