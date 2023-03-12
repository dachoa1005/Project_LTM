#include "user.h"
#include "room.h"

#include <assert.h>
extern User *users;
extern Room *rooms;

int main() {
    readUsersFromFile(&users, "../File/user.txt");
    printAllUsers(users);
    
    rooms = init_rooms();
    assert(rooms->id == 1);
    assert(rooms->difficulty == 1);
    assert(rooms->max_users == 2);
    assert(rooms->current_number_users == 0);
    assert(rooms->state == READY);
    assert(rooms->questions != NULL);
    assert(rooms->num_questions == 5);
    assert(rooms->current_question == 0);
    assert(rooms->next == NULL);
    


    return 0;
}
