#ifndef CACHEME  
#define CACHEME

#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

#define HOST "127.0.0.1"
#define PORT 12049  // Defined as an integer

typedef unsigned int int32;
typedef unsigned short int int16;
typedef unsigned char int8;

void mainloop(int16);
void handle_signal(int);
int main(int, char**);  

#endif
