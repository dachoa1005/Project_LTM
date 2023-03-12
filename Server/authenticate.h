#include "user.h"

int authenticate(char *username, char *password);
void *connection_handle(void *client_socket);
char *login(char *username, char *password);
char *register_user(char *username, char *password);
void send_login_status(int socketfd, char *status);