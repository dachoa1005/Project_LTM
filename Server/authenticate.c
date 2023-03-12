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
