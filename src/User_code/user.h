#ifndef USER_H
#define USER_H

#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include "getIp.h"
#include "verify_commands.h"
#define DEFAULT_PORT "58023"

int main(int argc, char **argv);
void send_request_udp(char *port, char *asip, char *buffer);
void send_request_tcp(char *port, char *asip, char *buffer);

#endif