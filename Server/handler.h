#include "user.h"

void *connection_handle(void *client_socket);
void login_handle(char *username, char *password, int socketfd);
void register_handle(char *username, char *password, int socketfd);
void logout_handle(char *username, char *password, int socketfd);
void get_room_handle(char *username, int socketfd);
void join_room_handle(char *username, int room_id, int socketfd);