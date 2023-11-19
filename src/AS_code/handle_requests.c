#include "handle_requests.h"
#include <stdio.h>

void handle_requests_udp(char *port, bool verbose) {
    int fd, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char words_command[10][20];

    fd = socket(AF_INET, SOCK_DGRAM, 0); 
    if (fd == -1) /*error*/ exit(1);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;       
    hints.ai_socktype = SOCK_DGRAM;  
    hints.ai_flags = AI_PASSIVE;

    errcode = getaddrinfo(NULL, port, &hints, &res);
    if (errcode != 0) /*error*/ exit(1);

    n = bind(fd, res->ai_addr, res->ai_addrlen);
    if (n == -1) /*error*/ exit(1);

    while (true) {
        addrlen = sizeof(addr);
        n = recvfrom(fd, words_command, 128, 0, (struct sockaddr*)&addr, &addrlen);
        if (n == -1) /*error*/ exit(1);

        execute_command_udp(fd, addr, words_command);
        printf("%s\n", strlen(words_command[0]) > 0 ? "command received" : "error");
    }
    freeaddrinfo(res);
    close(fd);
}

void execute_command_udp(int fd, struct sockaddr_in addr, char words_command[10][20]) {
    if (!strcmp(words_command[0], "login")) 
        ex_login(fd, addr, words_command);
    else if (!strcmp(words_command[0], "logout")) 
        ex_logout(fd, addr, words_command); 
    else if (!strcmp(words_command[0], "unregister")) 
        ex_unregister(fd, addr, words_command); 
    else if (!strcmp(words_command[0], "myauctions") ||
    !strcmp(words_command[0], "ma")) 
        ex_myauctions(fd, addr, words_command); 
    else if (!strcmp(words_command[0], "mybids") ||
    !strcmp(words_command[0], "mb")) 
        ex_mybids(fd, addr, words_command); 
    else if (!strcmp(words_command[0], "list") ||
    !strcmp(words_command[0], "l")) 
        ex_list(fd, addr, words_command); 
    else if (!strcmp(words_command[0], "show_record") ||
    !strcmp(words_command[0], "sr")) 
        ex_show_record(fd, addr, words_command); 
}

void send_msg_to_user(int fd, struct sockaddr_in addr, char *message) {
    int n;
    n = sendto(fd, message, strlen(message), 0, (struct sockaddr*)&addr, sizeof(addr));
    if (n == -1) /*error*/ exit(1);
}

void ex_login(int fd, struct sockaddr_in addr, char words_command[10][20]) {
    // To do (incomplete)
    send_msg_to_user(fd, addr, "successful login");
}
void ex_logout(int fd, struct sockaddr_in addr, char words_command[10][20]) {
    // To do
}
void ex_unregister(int fd, struct sockaddr_in addr, char words_command[10][20]) {
    // To do
}
void ex_myauctions(int fd, struct sockaddr_in addr, char words_command[10][20]) {
    // To do
}
void ex_mybids(int fd, struct sockaddr_in addr, char words_command[10][20]) {
    // To do
}
void ex_list(int fd, struct sockaddr_in addr, char words_command[10][20]) {
    // To do
}
void ex_show_record(int fd, struct sockaddr_in addr, char words_command[10][20]) {
    // To do
}

void handle_requests_tcp(char *port, bool verbose) {

}