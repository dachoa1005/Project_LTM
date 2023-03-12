#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

typedef struct question {
    int id;
    char content[256];
    char answer[256];
} Question;

Question all_questions[100];

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

int main(int argc, char const *argv[])
{
    init_questions_from_file();
    for (int i = 0; i < 100; i++) {
        printf("%d. %s\n", all_questions[i].id, all_questions[i].content);
    }
    return 0;
}
