#include "user.h"

void *connection_handle(void *client_socket);
void login_handle(char *username, char *password, int socketfd);
void register_handle(char *username, char *password, int socketfd);
void logout_handle(char *username, char *password, int socketfd);