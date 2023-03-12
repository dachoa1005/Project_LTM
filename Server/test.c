#include "user.h"
#include "room.h"

#include <assert.h>
extern User *users;
extern Room *rooms;

int main() {
    readUsersFromFile(&users, "../File/user.txt");
    printAllUsers(users);
    
    init_rooms();
    Room *room1 = create_room(1);

    print_all_rooms(rooms);
    assert(rooms->next != NULL);

    print_questions(room1);
    // assert(room1 != NULL);
    return 0;
}
