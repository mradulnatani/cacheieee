#ifndef cacheieee
#define cacheieee
#define MAX_IP_LEN 16
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <time.h>
#include <ctype.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define HOST    "127.0.0.1"
#define PORT    "12049"

#define MAX_KEY_SIZE 256
#define MAX_VAL_SIZE 1024
#define MAX_STORE_SIZE 1024

typedef unsigned int int32;
typedef unsigned short int int16;
typedef unsigned char int8;

typedef struct {
    char key[MAX_KEY_SIZE];
    char value[MAX_VAL_SIZE];
    time_t expiry;  // new TTL field
} KVPair;

typedef struct {
    KVPair store[MAX_STORE_SIZE];
    int size;
} KVStore;

typedef struct {
    int s;
    char ip[16];
    int16 port;
} Client;

extern KVStore kvstore;

void zero(int8*, int16);
void child_loop(Client*);
int initserver(int16_t);
void mainloop(int);
int main(int, char**);
int is_number(const char*);

void set_key(const char*, const char*);
const char* get_key(const char*);
bool del_key(const char*);
int exists_key(const char*);
void keys_list(int);
void clean_expired_keys(void);
void log_messege(const char *messege);


#endif
