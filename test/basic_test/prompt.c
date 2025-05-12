#include <stdio.h>
#include <string.h>

void hello_prompt() {
    char input[100];

    printf("system: hello!\n");
    printf("target answer: ");

    // Get user input
    fgets(input, sizeof(input), stdin);

    // Remove the newline character from input
    input[strcspn(input, "\n")] = '\0';

    if (strcmp(input, "hi") == 0) {
        printf("system: Hello there! How can I assist you?\n");
    } else {
        printf("system: I didn't understand that.\n");
    }
}
