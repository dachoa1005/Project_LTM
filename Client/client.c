#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <unistd.h>
#include <stdbool.h>

#define PORT 8889
#define IP_ADDRESS "127.0.0.1"

int main(int argc, char const *argv[])
{
    // create socket
    int client_socket;
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        printf("Socket creation failed\n");
        exit(EXIT_FAILURE);
    }
    // specify an address for the socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(IP_ADDRESS);

    int connection_status = connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    if (connection_status == -1)
    {
        printf("Connection failed\n");
        exit(EXIT_FAILURE);
    }
    printf("Connection successful\n");

    // int login_attemps = 0;
    char response[1024];
    int option = 0;
    bool logged_in = false; // 0: not login or login fail, 1: login success
    int exit_login = 0;     // 0: not exit, 1: exit
    char username[100];
    char password[100];
    char email[100];
    int choice = 0;
    char input[100];
    int joined_room = 0; // 0: not join any room, 1: join a room
    while (1)
    {
        // login screen
        while (exit_login == 0 && logged_in == false)
        {
            printf("Select an option: \n1. Login\n2. Register\n3. Exit\n");
            printf("Enter your choice: ");
            // scanf("%d", &choice);
            // getchar();
            fgets(input, 100, stdin);
            choice = strtol(input, NULL, 10);
            switch (choice)
            {
            case 1: // Login - việc đếm số lần client attemps bị fail sẽ do server đảm nhiệm
            {
                memset(username, 0, sizeof(username));
                memset(password, 0, sizeof(password));

                printf("Enter username: ");
                fgets(username, 100, stdin);
                username[strlen(username) - 1] = '\0';
                // getchar();

                printf("Enter password: ");
                fgets(password, 100, stdin);
                password[strlen(password) - 1] = '\0';
                // getchar();

                char login_message[200];
                memset(login_message, 0, sizeof(login_message));
                strcpy(login_message, "LOGINREQ|");
                strcat(login_message, username);
                strcat(login_message, "|");
                strcat(login_message, password);

                printf("login_message: %s\n", login_message);
                // send login request to server
                // size_t message_len = strlen(login_message);
                send(client_socket, login_message, sizeof(login_message), 0);

                // receive response from server
                memset(response, 0, sizeof(response));
                recv(client_socket, &response, sizeof(response), 0);
                if (strcmp(response, "SUCC") == 0) // login success
                {
                    printf("Login success\n");
                    logged_in = true;
                    exit_login = 1;
                    break;
                }
                else if (strcmp(response, "FAIL") == 0) // login fail
                {
                    printf("Login fail, you have been entered wrong password, please try again.\n");
                    logged_in = false;
                }
                else if (strcmp(response, "LOCK") == 0) // login fail
                {
                    printf("Login fail, your account have been blocked\n");
                    logged_in = false;
                }
                else if (strcmp(response, "NOEX") == 0) // username not exist
                {
                    printf("Login fail, username not exist\n");
                    logged_in = false;
                }
                else if (strcmp(response, "ALOG") == 0) // already login
                {
                    printf("Login fail, your account have been logged in\n");
                    logged_in = false;
                }

                break;
            }
            case 2:
            {
                memset(username, 0, sizeof(username));
                memset(password, 0, sizeof(password));

                printf("Enter username: ");
                fgets(username, 100, stdin);
                username[strlen(username) - 1] = '\0';

                printf("Enter password: ");
                fgets(password, 100, stdin);
                password[strlen(password) - 1] = '\0';

                char register_message[200];
                strcpy(register_message, "REGISREQ|"); // register request
                strcat(register_message, username);
                strcat(register_message, "|");
                strcat(register_message, password);

                // send register request to server
                send(client_socket, register_message, sizeof(register_message), 0);

                // receive response from server
                recv(client_socket, &response, sizeof(response), 0);
                if (strcmp(response, "SUCC") == 0) // register success
                {
                    printf("Register success\n");
                    logged_in = true;
                    exit_login = 1;
                    break;
                }
                else if (strcmp(response, "DUPL") == 0) // register fail - duplicate username
                {
                    printf("Register fail, username already exists\n");
                    logged_in = false;
                }

                break;
            }
            case 3:
            {
                exit_login = 1;
                printf("Exit\n");
                exit(EXIT_SUCCESS);
            }
            default:
            {
                printf("Invalid option\n");
                break;
            }
            }
        }

        // after login success
        // main screen
        if (logged_in == true)
        {
            printf("You are logged in as %s\n_______________________\n", username);

            // main screen
            while (logged_in == true)
            {
                printf("Welcome to King Of Vietnamese Game\n");
                printf("Select an option: \n1. Play game\n2. View ranking\n3. Change password\n4. Logout\n");
                printf("Enter your choice: ");
                fgets(input, 100, stdin);
                input[strlen(input) - 1] = '\0';
                choice = strtol(input, NULL, 10);
                switch (choice)
                {
                case 1: // play game
                {
                    // print all available room to client or create new room
                    // client choose to join a room or create a new room
                    // if join a room, client will be waiting for another player to join
                    // if create a new room, client will be waiting for another player to join
                    // if another player join, game will start
                    // if another player not join, client can choose to wait or exit
                    // if client choose to wait, client will be waiting for another player to join
                    // if client choose to exit, client will be back to main screen
                    // send request to server to get all available room
                    printf("Play game\n");
                    char get_room_message[250];
                    sprintf(get_room_message, "GETROOMREQ|%s", username);
                    send(client_socket, get_room_message, strlen(get_room_message), 0);

                    // receive response from server
                    memset(response, 0, sizeof(response));
                    printf("Available rooms:\n");
                    printf("Room ID\tDifficulty\tCurrent number of players\n");
                    // response format: ROOM|room_id|difficulty|current_number_of_players or END
                    while (1)
                    {
                        memset(response, 0, sizeof(response));
                        recv(client_socket, response, sizeof(response), 0);
                        response[strlen(response)] = '\0';
                        if (strcmp(response, "END") == 0)
                        {
                            break;
                        }
                        char *token = strtok(response, "|");
                        token = strtok(NULL, "|");
                        printf("%s\t", token);
                        token = strtok(NULL, "|");
                        printf("%s\t\t", token);
                        token = strtok(NULL, "|");
                        printf("%s\n", token);
                    }
                    while (1)
                    {
                        printf("Enter room ID to join, enter 0 to create a new room, enter -1 to exit: ");
                        fgets(input, 100, stdin);
                        input[strlen(input) - 1] = '\0';
                        choice = strtol(input, NULL, 10);

                        if (choice == 0) // create new room
                        {
                            // TO DO
                        }
                        else if (choice > 0) // join room
                        {
                            // send request to server to join room
                            char join_room_message[250];
                            sprintf(join_room_message, "JOINROOMREQ|%s|%d", username, choice);
                            send(client_socket, join_room_message, sizeof(join_room_message), 0);

                            // receive response from server
                            memset(response, 0, sizeof(response));
                            recv(client_socket, response, sizeof(response), 0);
                            response[strlen(response)] = '\0';

                            // response = "NOEX" - room not exist, "FULL" - room is full, "SUCC" - join success
                            if (strcmp(response, "NOEX") == 0)
                            {
                                printf("Room not exist, please enter again\n");
                                continue;
                            }
                            if (strcmp(response, "FULL") == 0)
                            {
                                printf("Room is full, please enter again\n");
                                continue;
                            }
                            if (strcmp(response, "SUCC") == 0)
                            {
                                printf("Join room success, waiting for another player to join\n");
                                joined_room = 1;
                                break;
                            }
                        }
                        else if (choice == -1)
                        {
                            break;
                        }
                        else
                        {
                            printf("Invalid option, please enter again\n");
                            continue;
                        }
                    }
                    if (joined_room == 1)
                    {
                        // wait for another player to join
                        // if another player join, game will start -> wait for STARTGAME message from server
                        // receive response from server
                        memset(response, 0, sizeof(response));
                        recv(client_socket, response, sizeof(response), 0);
                        response[strlen(response)] = '\0';

                        // response = "STAR" - game start
                        if (strcmp(response, "STAR") == 0) 
                        {
                            int point = 0;
                            printf("Game start\n");
                            // receive question from server
                            // question format: QUES|question_content|question_answer|question_content|question_answer|...
                            memset(response, 0, sizeof(response));
                            recv(client_socket, response, sizeof(response), 0);
                            response[strlen(response)] = '\0';
                            // split response to get question content and answer
                            char *token = strtok(response, "|"); // QUES
                            while (token != NULL)
                            {
                                token = strtok(NULL, "|"); // question content
                                printf("Enter the correct answer to 5 questions below:\n");
                                printf("%s\n", token);
                                token = strtok(NULL, "|"); // question answer
                                printf("Enter your answer: ");
                                fgets(input, 100, stdin);
                                input[strlen(input) - 1] = '\0';
                                if (strcmp(input, token) == 0)
                                {
                                    printf("Correct answer!\n");
                                    point++;
                                } else {
                                    printf("Wrong answer!\n");
                                }
                            }
                            printf("Your point: %d.\n Waiting for another player to finish and see the result\n", point);
                            // send point to server
                            char send_point_message[20];
                            sprintf(send_point_message, "PONT|%d", point);
                            send(client_socket, send_point_message, sizeof(send_point_message), 0);

                            // receive response from server
                            // 
                            memset(response, 0, sizeof(response));
                            recv(client_socket, response, sizeof(response), 0);
                            response[strlen(response)] = '\0';
                            // respone = WINN or LOSE or DRAW
                            if (strcmp(response, "WINN") == 0)
                            {
                                printf("You win!\n");
                            }
                            if (strcmp(response, "LOSE") == 0)
                            {
                                printf("You lose!\n");
                            }
                            if (strcmp(response, "DRAW") == 0)
                            {
                                printf("Draw!\n");
                            }

                            // back to room list
                            joined_room = 0;

                        }
                    }

                    break;
                }
                case 2: // view ranking
                {
                    printf("View ranking\n");
                    break;
                }
                case 3: // change password
                {
                    printf("Change password\n");
                    break;
                }
                case 4: // logout
                {
                    // send logout request to server
                    char logout_message[250];
                    sprintf(logout_message, "LOGOUTREQ|%s|%s", username, password);
                    send(client_socket, logout_message, strlen(logout_message), 0);
                    logged_in = false;
                    exit_login = 0;
                    printf("Logout success\n");
                    printf("Back to login screen\n_______________________________\n");
                    break;
                }
                default:
                {
                    printf("Invalid option\n");
                    break;
                }
                }
            }
        }
        close(client_socket);
        return 0;
    }
}