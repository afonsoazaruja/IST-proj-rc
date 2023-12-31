#ifndef USER_H
#define USER_H

#include "commands.h"
#include "../validations.h"
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/time.h>
#include <signal.h>

#define DEFAULT_PORT "58023" // 58000 + GN
#define SIZE_PORT 6
#define BUFFER_SIZE 7000
#define MAX_HOSTNAME_SIZE 253
#define ASSET_DIR "../ASSETS"
#define SA_DIR "../SA"
#define TIMEOUT 5

int main(int argc, char **argv);
void send_request_udp(char *port, char *asip, char *buffer);
void send_request_tcp(char *port, char *asip, char *buffer);
char* getIpAddress();
void send_open(char *buffer, int fd);
void handle_sigint(int SIGNAL);
void set_timeout(int fd);
void welcome();

#endif