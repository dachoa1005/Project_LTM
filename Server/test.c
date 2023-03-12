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

    // print_all_rooms(rooms);
    assert(rooms->next != NULL);

    // print_questions(room1);

    // add user to room
    User *user1 = searchUser(users, "test1");
    User *user2 = searchUser(users, "dachoa1005");

    // add_user_to_room(room1, user1);
    // print_room(room1);
    // remove_user_from_room(room1, user1);
    // print_room(room1);
    add_user_to_room(room1, user2);
    add_user_to_room(room1, user1);
    // print_room(room1);
    // assert(room1 != NULL);
    Room *room2 = create_room(2);
    
    Room *room_has_id_2 = search_room(rooms, 2);
    print_room(room_has_id_2);
    // assert(room_has_id_2 == NULL);

    // print_all_rooms(rooms);
    return 0;
}
