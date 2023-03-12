#include "room.h"

Room *rooms = NULL;
Question all_questions[100];
int current_room_id = 0;

void init_questions_from_file() // read all question from file, store in array
{
    char *content;
    char *answer;
    FILE *f = fopen("../File/questions.txt", "r");
    if (f == NULL) {
        printf("Error opening file!\n");
        return;
    }

    char line[256];
    int i = 0;
    while (fgets(line, sizeof(line), f)) {
        content = malloc(256);
        answer = malloc(256);
        strcpy(content, strtok(line, "|"));
        strcpy(answer, strtok(NULL, "|"));

        strcpy(all_questions[i].content, content);
        strcpy(all_questions[i].answer, answer);
        all_questions[i].id = i;
        i++;
    }
}

void init_questions_to_room(Room *room)
{
    int num_questions = sizeof(all_questions) / sizeof(Question);
    int selected_questions[5];

    // initialize selected_questions array to -1 to mark unused index
    for (int i = 0; i < 5; i++) {
        selected_questions[i] = -1;
    }

    // randomly select 5 unique question indices
    int i = 0;
    while (i < 5) {
        int rand_index = rand() % num_questions;
        if (selected_questions[rand_index] == -1) {
            selected_questions[rand_index] = rand_index;
            i++;
        }
    }

    // store selected questions in room->questions
    for (int i = 0; i < 5; i++) {
        int question_index = selected_questions[i];
        Question selected_question = all_questions[question_index];
        room->questions[i] = selected_question;
    }
}

void print_questions(Room *room)
{
    for (int i = 0; i < 5; i++) {
        printf("Question %d: %s", i + 1, room->questions[i].content);
    }
}

Room *add_room(Room **rooms, Room *new_room)
{
    if (*rooms == NULL) {
        *rooms = new_room;
        return new_room;
    }

    Room *current = *rooms;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new_room;
    return new_room;
}

Room *create_room(int difficulty)
{
    Room *room = (Room *)malloc(sizeof(Room));
    room->id = current_room_id++;
    room->difficulty = difficulty;
    room->max_users = 2;
    room->current_number_users = 0;
    room->room_users = (User*) malloc(2 * sizeof(User)); // array of users in the room - 2 users max
    room->state = READY;
    room->questions = NULL;
    room->num_questions = 5;
    room->current_question = 0;
    room->next = NULL;

    init_questions_to_room(room);
    add_room(&rooms, room);
    return room;
}

Room *init_rooms()
{
    init_questions_from_file();
    Room *rooms =(Room *)malloc(sizeof(Room));
}

void add_user_to_room(Room *room, User *user)
{
    if (room->current_number_users < room->max_users) {
        room->room_users[room->current_number_users] = *user;
        room->current_number_users++;
    }
}

void remove_user_from_room(Room *room, User *user)
{
    for (int i = 0; i < room->current_number_users; i++) {
        if (strcmp(room->room_users[i].username, user->username) == 0) {
            room->room_users[i] = room->room_users[room->current_number_users - 1];
            room->current_number_users--;
            break;
        }
    }
}

void print_room(Room *room)
{
    // printf("Room %d - Difficulty: %d - Current number of users: %d\n", room->id, room->difficulty, room->current_number_users);

}

void print_all_rooms(Room *room)
{
    Room *current = room;
    while (current != NULL) {
        print_room(current);
        current = current->next;
    }
}

void send_question(Room *room, User *user);
void send_answer(Room *room, User *user, char *answer);
