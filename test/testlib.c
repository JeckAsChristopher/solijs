#include <stdio.h>

const char* greet() {
    return "Hello from greet()!";
}

const char* echo(int argc, const char** argv) {
    static char buffer[256];
    snprintf(buffer, sizeof(buffer), "Received %d args: %s %s", argc,
             argc > 0 ? argv[0] : "NULL",
             argc > 1 ? argv[1] : "NULL");
    return buffer;
}

void say_hello() {
    printf("Hello from say_hello()\n");
}
