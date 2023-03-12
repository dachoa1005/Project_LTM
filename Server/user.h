#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#ifndef USER_H
#define USER_H

typedef struct user
{
    char username[100];
    char password[100];
    int status; // 0: locked, 1: actived
    int points;
    int login_status; // 0: offline, 1: online
    int login_attempts;
    int socketfd;
    int current_room_id;
    struct user *next;
} User;
#endif // USER_H

User *initUser(char *username, char *password, int status, int points);
void addUser(User **users,User *user);
User* searchUser(User *users,char *username);
void readUsersFromFile(User **users,char *filepath);
void writeNewUserToFile(User *user,char *filepath);
void writeAllUsersToFile(User *users,char *filepath);
void printAllUsers(User *users);