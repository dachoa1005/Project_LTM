#include "user.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_ROOMS 10
#define MAX_USERS 2

typedef enum State
{
    READY,  // waiting for players
    WAITING, // waiting for another player to join
    PLAYING // playing
}State;

typedef struct question {
    int id;
    char content[256];
    char answer[256];
} Question;

typedef struct room {
    int id;
    int difficulty;
    int max_users;
    int current_number_users; // number of users currently in the room
    User *room_users; // array of users in the room
    State state;
    Question *questions; // array of questions
    int num_questions;
    int current_question;
    struct room *next;    
} Room;

void init_questions_from_file(); // read all question from file, store in array
void init_questions_to_room(Room *room);
void print_questions(Room *room);

Room *init_rooms();
Room *create_room(int difficulty);
Room *add_room(Room **rooms, Room *new_room);
Room *search_room(Room *rooms, int id);

void add_user_to_room(Room *room, User *user);
void remove_user_from_room(Room *room, User *user);

void print_room(Room *room);
void print_all_rooms(Room *room);

void send_question(Room *room, User *user);
void send_result(Room *room, User *user, char *answer);
void send_to_all(Room *room, char *message);
