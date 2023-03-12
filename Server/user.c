#include "user.h"

User *users;
User *initUser(char *username, char *password, int status, int points) 
{
    User *newUser = (User *)malloc(sizeof(User));
    if (newUser == NULL) {
        printf("Allocated error!\n");
        return NULL;
    }
    strcpy(newUser->username, username);
    strcpy(newUser->password, password);
    newUser->status = status;
    newUser->points = points;
    newUser->login_status = 0;
    newUser->login_attempts = 0;
    newUser->socketfd = -1;
    newUser->current_room_id = -1;
    newUser->next = NULL;
    return newUser;
}

void addUser(User **users, User *user) {
    if (*users == NULL) {
        *users = user;
        return;
    }

    User *cur = *users;
    while (cur->next != NULL) {
        cur = cur->next;
    }
    cur->next = user;
}

User *searchUser(User *users, char *username)
{
	User *cur = users;
	while (cur != NULL)
	{
		if (strcmp(cur->username, username) == 0)
		{
			return cur;
		}
		cur = cur->next;
	}
	return NULL;
}

void readUsersFromFile(User **users,char *filepath)
{
    // file format: username|password|status|points
    // array format: username, password, status, login_status, loggin_attempts, socketfd, current_room_id, 
    char username[100];
    char password[100];
    int status;
    int points;
    int login_status;
    int login_attempts;
    int socketfd;
    int current_room_id;
    char line[250];
    FILE *f = fopen(filepath, "r");
    if (f == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }

    while (fgets(line, 250, f) != NULL) 
    {   
        if (strlen(line) <= 1) {  // ignore empty lines
            continue;
        }

        char *token = strtok(line, "|");
        if (token == NULL) continue;
        strcpy(username, token);

        token = strtok(NULL, "|");
        if (token == NULL) continue;
        strcpy(password, token);

        token = strtok(NULL, "|");
        if (token == NULL) continue;
        status = atoi(token);

        token = strtok(NULL, "|");
        if (token == NULL) continue;
        points = atoi(token);

        token = strtok(NULL, "|");
        if (token != NULL) continue;

        User *user = initUser(username, password, status, points);
        addUser(users, user);
    }
    fclose(f);
}

void writeNewUserToFile(User *user, char *filepath)
{
    FILE *f = fopen(filepath, "a"); // open file in append mode
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    
    // file format: username|password|status|points
    fprintf(f, "%s|%s|%d|%d\n", user->username, user->password, user->status, user->points);

    fclose(f);
}

void writeAllUsersToFile(User *users, char *filepath) {
    FILE *fp = fopen(filepath, "w");
    if (!fp) {
        printf("Failed to open file: %s\n", filepath);
        return;
    }

    User *current = users;
    while (current != NULL) {
        fprintf(fp, "%s|%s|%d|%d\n", current->username, current->password, current->status, current->points);
        current = current->next;
    }

    fclose(fp);
}

void printAllUsers(User *users)
{
    User *cur = users;
    printf("%-20s%-20s%-20s%-20s\n", "Username", "Password", "Status", "Points");
    while (cur != NULL)
    {
        printf("%-20s%-20s%-20d%-20d\n", cur->username, cur->password, cur->status, cur->points);
        cur = cur->next;
    }
}