#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *trim_space(char *str)
{
    char *dst = str;
    while (*str)
    {
        if (!isspace((unsigned char)*str))
            *dst++ = *str;
        str++;
    }
    *dst = '\0';
    strcpy(str, dst);
    return str;
}

int main(int argc, char const *argv[])
{
    char str[] = "    asd  aa";
    printf("%s\n", trim_space(str));
    return 0;
}