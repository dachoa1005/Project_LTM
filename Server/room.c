#include "room.h"
#include <time.h>

Room *rooms = NULL;
Question all_questions[10];
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
    while (fgets(line, sizeof(line), f) != NULL) {
        content = malloc(256);
        answer = malloc(256);
        // add \0 to the end of the string
        line[sizeof(line) - 1] = '\0';
        // printf("line: %s\n", line);
        strcpy(content, strtok(line, "|"));
        if (content == NULL|| strcmp(content, "") == 0 || strcmp(content, " ") == 0) {
            continue;
        }
        strcpy(answer, strtok(NULL, "|"));

        strcpy(all_questions[i].content, content);
        strcpy(all_questions[i].answer, answer);
        all_questions[i].id = i;
        i++;
    }
}

// void init_questions_to_room(Room *room)
// {
//     int num_questions = sizeof(all_questions) / sizeof(Question);
//     int selected_questions[5];

//     // initialize selected_questions array to -1 to mark unused index
//     for (int i = 0; i < 5; i++) {
//         selected_questions[i] = -1;
//     }

//     // randomly select 5 unique question indices
//     int i = 0;
//     time_t t;  
//     srand((unsigned) time(&t));

//     // printf("num_questions: %d\n", num_questions);
//     while (i < 5) {
//         int rand_index = rand() % num_questions;
//         // printf("rand_index: %d\n", rand_index);
//         if (selected_questions[rand_index] == -1) {
//             selected_questions[rand_index] = rand_index;
//             i++;
//         }
//     }

//     // store selected questions in room->questions
//     for (int i = 0; i < 5; i++) {
//         int question_index = selected_questions[i];
//         Question selected_question = all_questions[question_index];
//         room->questions[i] = selected_question;
//     }
// }

void init_questions_to_room(Room *room)
{
    int num_questions = sizeof(all_questions) / sizeof(Question);
    int indices[num_questions];

    // initialize indices array to store the indices of all_questions
    for (int i = 0; i < num_questions; i++) {
        indices[i] = i;
    }

    // shuffle the indices array
    time_t t;
    srand((unsigned) time(&t));
    for (int i = 0; i < num_questions - 1; i++) {
        int j = i + rand() / (RAND_MAX / (num_questions - i) + 1);
        int temp = indices[j];
        indices[j] = indices[i];
        indices[i] = temp;
    }

    // select the first 5 indices and store the corresponding questions in room->questions
    int i = 0;
    int j = 0;
    while (i < 5) {
        int question_index = indices[j];
        Question selected_question = all_questions[question_index];
        if (strcmp(selected_question.content, "") != 0) {
            room->questions[i] = selected_question;
            i++;
        }
        j++;
    }
}

void print_questions(Room *room)
{
    for (int i = 0; i < 5; i++) {
        printf("Question %d: %s\n", i + 1, room->questions[i].content);
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

Room* create_room(int difficulty) {
    Room *new_room = (Room*)malloc(sizeof(Room));  
    
    if (new_room == NULL) {  
        printf("Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    current_room_id += 1;
    new_room->id = current_room_id;
    new_room->difficulty = 1;
    new_room->max_users = 2;
    new_room->current_number_users = 0;
    new_room->state = READY;
    new_room->questions = NULL;
    new_room->num_questions = 5;
    new_room->current_question = 0;
    new_room->next = NULL;
    new_room->room_users = (User *)malloc(sizeof(User) * new_room->max_users);
    new_room->questions = (Question *)malloc(sizeof(Question) * new_room->num_questions);

    init_questions_to_room(new_room);

    add_room(&rooms, new_room);
    return new_room;
}

Room *init_rooms()
{
    init_questions_from_file();
    rooms = (Room *)malloc(sizeof(Room));
    create_room(1);
    return rooms;
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
    printf("Room %d - Difficulty: %d - Current number of users: %d\n", room->id, room->difficulty, room->current_number_users);
    if (room->current_number_users > 0) {
        printf("Users in room id %d: \n", room->id);
        for (int i = 0; i < room->current_number_users; i++) {
            printf("%s\n", room->room_users[i].username);
        }
    }
}

void print_all_rooms(Room *room)
{
    Room *current = room;
    while (current != NULL) {
        print_room(current);
        current = current->next;
    }
}

Room *search_room(Room *room, int room_id)
{
    Room *current = room;
    while (current != NULL) {
        if (current->id == room_id) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void send_question(Room *room, User *user);
void send_result(Room *room, User *user, char *answer);
