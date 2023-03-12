#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <unistd.h>

#define PORT 8888
#define IP_ADDRESS "127.0.0.1"

int main(int argc, char const *argv[])
{
    // create socket
    int client_socket;
    client_socket = socket(AF_INET, SOCK_STREAM, 0);

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

    int login_attemps = 0;
    char response[1024];
    int option = 0;
    int logged_in = 0;  // 0: not login or login fail, 1: login success
    int exit_login = 0; // 0: not exit, 1: exit
    char username[100];
    char password[100];
    char email[100];
    int choice = 0;
    char input[100];
    // login screen
    while (!exit_login)
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
            strcpy(login_message, "LOGINREQ|");
            strcat(login_message, username);
            strcat(login_message, "|");
            strcat(login_message, password);

            printf("login_message: %s\n", login_message);
            // send login request to server
            send(client_socket, login_message, sizeof(login_message), 0);

            // receive response from server
            recv(client_socket, &response, sizeof(response), 0);
            if (strcmp(response, "SUCC") == 0) // login success
            {
                printf("Login success\n");
                logged_in = 1;
                exit_login = 1;
                break;
            }
            else if (strcmp(response, "FAIL") == 0) // login fail
            {
                printf("Login fail, you have been entered wrong password, please try again.\n");
                logged_in = 0;
            }
            else if (strcmp(response, "LOCK") == 0) // login fail
            {
                printf("Login fail, your account have been blocked\n");
                logged_in = 0;
            }
            else if (strcmp(response, "NOEX") == 0) // username not exist
            {
                printf("Login fail, username not exist\n");
                logged_in = 0;
            }
            else if (strcmp(response, "ALOG") == 0) // already login
            {
                printf("Login fail, your account have been logged in\n");
                logged_in = 0;
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
                logged_in = 1;
                exit_login = 1;
                break;
            }
            else if (strcmp(response, "DUPL") == 0) // register fail - duplicate username
            {
                printf("Register fail, username already exists\n");
                logged_in = 0;
            }

            break;
        }
        case 3:
        {
            exit_login = 1;
            printf("Exit\n");
            break;
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
    if (logged_in == 1)
    {
        printf("You are logged in as %s\n", username);

        // main screen
        while (1)
        {
            printf("Welcome to King Of Vietnamese Game\n");
            printf("Select an option: \n1. Play game\n2. View ranking\n3. Change password\n4. Logout\n");
            fgets(input, 100, stdin);
            choice = strtol(input, NULL, 10);
            switch (choice)
            {
            case 1: // play game
            {
                printf("Play game\n");
                // print all available room to client or create new room
                // client choose to join a room or create a new room
                // if join a room, client will be waiting for another player to join
                // if create a new room, client will be waiting for another player to join
                // if another player join, game will start
                // if another player not join, client can choose to wait or exit
                // if client choose to wait, client will be waiting for another player to join
                // if client choose to exit, client will be back to main screen

                // send request to server to get all available room
                char get_room_message[200];
                strcpy(get_room_message, "GETROOMREQ|");
                strcat(get_room_message, username);
                send(client_socket, get_room_message, sizeof(get_room_message), 0);

                // receive response from server
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
                printf("Logout\n");
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

    return 0;
}
