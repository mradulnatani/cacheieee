#include "cacheieee.h"

bool scontinuation;
bool ccontinuation;
void print_tree_structure(int s);

KVStore kvstore;

void log_message(const char *message) {
    FILE *logfile = fopen("server_log.txt", "a");
    if (logfile) {
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        char timestr[64];
        strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", t);
        fprintf(logfile, "[%s] %s\n", timestr, message);
        fclose(logfile);
    }
}

void client_connected(Client *cli) {
    char message[256];
    snprintf(message, sizeof(message), "Client Connected: %s:%d", cli->ip, cli->port);
    log_message(message);
}

void client_disconnected(Client *cli) {
    char message[256];
    snprintf(message, sizeof(message), "Client Disconnected: %s:%d", cli->ip, cli->port);
    log_message(message);
}


void zero(int8 *buf, int16 size) {
    int8 *p;
    int16 n;
    for (n = 0, p = buf; n < size; n++, p++)
        *p = 0;
    return;
}

void clean_expired_keys() {
    time_t now = time(NULL);
    for (int i = 0; i < kvstore.size;) {
        if (kvstore.store[i].expiry > 0 && kvstore.store[i].expiry <= now) {
            del_key(kvstore.store[i].key);
        } else {
            i++;
        }
    }
}

void set_key(const char* key, const char* value) {
    for (int i = 0; i < kvstore.size; ++i) {
        if (strcmp(kvstore.store[i].key, key) == 0) {
            strncpy(kvstore.store[i].value, value, MAX_VAL_SIZE);
            kvstore.store[i].expiry = 0;
            return;
        }
    }
    if (kvstore.size < MAX_STORE_SIZE) {
        strncpy(kvstore.store[kvstore.size].key, key, MAX_KEY_SIZE);
        strncpy(kvstore.store[kvstore.size].value, value, MAX_VAL_SIZE);
        kvstore.store[kvstore.size].expiry = 0;
        kvstore.size++;
    }
}

const char* get_key(const char* key) {
    time_t now = time(NULL);
    for (int i = 0; i < kvstore.size; ++i) {
        if (strcmp(kvstore.store[i].key, key) == 0) {
            if (kvstore.store[i].expiry > 0 && kvstore.store[i].expiry <= now) {
                del_key(key);
                return NULL;
            }
            return kvstore.store[i].value;
        }
    }
    return NULL;
}

bool del_key(const char* key) {
    for (int i = 0; i < kvstore.size; ++i) {
        if (strcmp(kvstore.store[i].key, key) == 0) {
            for (int j = i; j < kvstore.size - 1; ++j) {
                kvstore.store[j] = kvstore.store[j + 1];
            }
            kvstore.size--;
            return true;
        }
    }
    return false;
}

int exists_key(const char* key) {
    for (int i = 0; i < kvstore.size; ++i) {
        if (strcmp(kvstore.store[i].key, key) == 0)
            return 1;
    }
    return 0;
}

int get_key_index(const char* key) {
    for (int i = 0; i < kvstore.size; ++i) {
        if (strcmp(kvstore.store[i].key, key) == 0)
            return i;
    }
    return -1;
}

void keys_list(int sock) {
    for (int i = 0; i < kvstore.size; ++i) {
        dprintf(sock, "%s\n", kvstore.store[i].key);
    }
}

void flush_all() {
    kvstore.size = 0;
}

void child_loop(Client *cli) {
    char buffer[2048];
    int n;
    

    dprintf(cli->s,"\033[0;32mcacheieee> \033[0m");
    while ((n = read(cli->s, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[n] = '\0';

        clean_expired_keys();

        char command[10], key[MAX_KEY_SIZE], val[MAX_VAL_SIZE];
        int args = sscanf(buffer, "%s %s \"%[^\"]\"", command, key, val);

        if (strcasecmp(command, "SET") == 0 && args == 3) {
            set_key(key, val);
            dprintf(cli->s, "OK\n");
        }
        else if (strcasecmp(command, "SETEX") == 0) {
            char ttl_str[16];
            char value[MAX_VAL_SIZE];
            int ttl;
            if (sscanf(buffer, "%*s %s %s \"%[^\"]\"", key, ttl_str, value) == 3) {
                ttl = atoi(ttl_str);
                time_t expiry = time(NULL) + ttl;
                int idx = get_key_index(key);
                if (idx != -1) {
                    strncpy(kvstore.store[idx].value, value, MAX_VAL_SIZE);
                    kvstore.store[idx].expiry = expiry;
                } else if (kvstore.size < MAX_STORE_SIZE) {
                    strncpy(kvstore.store[kvstore.size].key, key, MAX_KEY_SIZE);
                    strncpy(kvstore.store[kvstore.size].value, value, MAX_VAL_SIZE);
                    kvstore.store[kvstore.size].expiry = expiry;
                    kvstore.size++;
                }
                dprintf(cli->s, "OK\n");
            } else {
                dprintf(cli->s, "Invalid SETEX format\n");
            }
        }
        else if (strcasecmp(command, "EXPIRE") == 0 && args >= 2) {
            char ttl_str[16];
            if (sscanf(buffer, "%*s %s %s", key, ttl_str) == 2) {
                int ttl = atoi(ttl_str);
                int idx = get_key_index(key);
                if (idx != -1) {
                    kvstore.store[idx].expiry = time(NULL) + ttl;
                    dprintf(cli->s, "1\n");
                } else {
                    dprintf(cli->s, "0\n");
                }
            }
        }
        else if (strcasecmp(command, "TTL") == 0 && args >= 2) {
            int idx = get_key_index(key);
            if (idx != -1) {
                time_t now = time(NULL);
                if (kvstore.store[idx].expiry == 0)
                    dprintf(cli->s, "-1\n");
                else if (kvstore.store[idx].expiry <= now) {
                    del_key(key);
                    dprintf(cli->s, "-2\n");
                } else {
                    dprintf(cli->s, "%ld\n", kvstore.store[idx].expiry - now);
                }
            } else {
                dprintf(cli->s, "-2\n");
            }
        }
        else if (strcasecmp(command, "APPEND") == 0 && args == 3) {
            int idx = get_key_index(key);
            if (idx != -1) {
snprintf(kvstore.store[idx].value + strlen(kvstore.store[idx].value),
         MAX_VAL_SIZE - strlen(kvstore.store[idx].value),
         "%s", val);
            } else {
                strncpy(kvstore.store[kvstore.size].key, key, MAX_KEY_SIZE);
                strncpy(kvstore.store[kvstore.size].value, val, MAX_VAL_SIZE);
                kvstore.size++;
            }
            dprintf(cli->s, "OK\n");
        }
        else if (strcasecmp(command, "INCR") == 0 && args >= 2) {
            int idx = get_key_index(key);
            if (idx != -1) {
                int v = atoi(kvstore.store[idx].value);
                v++;
                snprintf(kvstore.store[idx].value, MAX_VAL_SIZE, "%d", v);
                dprintf(cli->s, "%d\n", v);
            } else {
                strncpy(kvstore.store[kvstore.size].key, key, MAX_KEY_SIZE);
                strcpy(kvstore.store[kvstore.size].value, "1");
                kvstore.size++;
                dprintf(cli->s, "1\n");
            }
        }
        else if (strcasecmp(command, "DECR") == 0 && args >= 2) {
            int idx = get_key_index(key);
            if (idx != -1) {
                int v = atoi(kvstore.store[idx].value);
                v--;
                snprintf(kvstore.store[idx].value, MAX_VAL_SIZE, "%d", v);
                dprintf(cli->s, "%d\n", v);
            } else {
                strncpy(kvstore.store[kvstore.size].key, key, MAX_KEY_SIZE);
                strcpy(kvstore.store[kvstore.size].value, "-1");
                kvstore.size++;
                dprintf(cli->s, "-1\n");
            }
        }
        else if (strcasecmp(command, "GET") == 0 && args >= 2) {
            const char* v = get_key(key);
            dprintf(cli->s, v ? "%s\n" : "(nil)\n", v);
        }
        else if (strcasecmp(command, "DEL") == 0 && args >= 2) {
            bool removed = del_key(key);
            dprintf(cli->s, "%s\n", removed ? "(deleted)" : "(nil)");
        }
        else if (strcasecmp(command, "EXISTS") == 0 && args >= 2) {
            dprintf(cli->s, "%d\n", exists_key(key));
        }
        else if (strcasecmp(command, "KEYS") == 0) {
            keys_list(cli->s);
        }
        else if (strcasecmp(command, "FLUSHALL") == 0) {
            flush_all();
            dprintf(cli->s, "OK\n");
        }
        else if (strcasecmp(command, "PING") == 0) {
            dprintf(cli->s, "PONG\n");
        }
        else if (strcasecmp(command, "QUIT") == 0) {
            ccontinuation = false;
            dprintf(cli->s, "Goodbye!\n");
		    close(cli->s);
	    return;
        }
	else if (strcasecmp(command, "TREE") == 0) {
    print_tree_structure(cli->s);
}
        else {
            dprintf(cli->s, "Invalid command\n");
        }
	dprintf(cli->s, "\033[0;32mcacheieee> \033[0m");
    }
}



void print_tree_structure(int s){
	dprintf(s,"The tree structure for the key value pairs is:");
	for(int i=0;i<kvstore.size;i++){
		KVPair *pair = &kvstore.store[i];
		dprintf(s, "Key: %s\n", pair->key);
                dprintf(s, "Value: %s\n", pair->value);

		 if (pair->expiry > 0) {
            dprintf(s, "Expiration Time: %ld\n", pair->expiry);
        } else {
            dprintf(s, "Expiration Time: Not Set\n");
        }

        dprintf(s, "--------------------------\n");
    }

    dprintf(s, "End of Tree Structure\n");
}



void mainloop(int s) {
    struct sockaddr_in cli;
    int32 len = sizeof(cli);
    int s2;
    char *ip;
    int16 port;
    Client *client;
    pid_t pid;

    s2 = accept(s, (struct sockaddr *)&cli, (unsigned int *)&len);
    if (s2 < 0)
        return;

port = ntohs(cli.sin_port);
ip = inet_ntoa(cli.sin_addr);

printf("Connection from %s:%d\n", ip, port);

// Allocate memory first
client = (Client *)malloc(sizeof(Client));
assert(client);

// Zero the memory and initialize
zero((int8 *)client, sizeof(Client));
client->s = s2;
client->port = port;
strncpy(client->ip, ip, 15);

// Now safely call client_connected
client_connected(client);

// Now fork
pid = fork();



    if (pid) {
        close(s2);
        free(client);
        return;
    } else {
        dprintf(s2,
     "100 Connected to cacheieee server \n"
    "\n"
    "Available Commands:\n"
    "  SET key value             - Sets the value for the given key\n"
    "  GET key                   - Retrieves the value associated with the key\n"
    "  DEL key                   - Deletes the key and its value\n"
    "  EXISTS key                - Checks if the key exists in the store\n"
    "  KEYS pattern              - Returns all keys matching the given pattern (e.g., *, user*, etc.)\n"
    "  SETEX key ttl value       - Sets a value with a TTL (in seconds)\n"
    "  TTL key                   - Returns the remaining time to live (TTL) for a key\n"
    "  INCR key                  - Increments an integer value by 1\n"
    "  DECR key                  - Decrements an integer value by 1\n"
    "  PING                      - Health check — responds with PONG\n"
    "  FLUSHALL                  - Deletes all keys in the current database (dangerous!)\n"
    "  INFO                      - Returns server stats (upcoming)\n"
    "  QUIT                      - Closes the client connection gracefully\n"
    "  TREE                      - Displays the current structure of the key value store shored inside the tree\n"
    "\n"
    "Features:\n"
    "  • In-memory key-value store\n"
    "  • TTL expiration support via SETEX\n"
    "  • Concurrent multi-client support\n"
    "  • Lightweight Redis-like interface\n"
    "\n"
    "Enjoy using cacheieee\n\n"
);

        ccontinuation = true;
        while (ccontinuation)
            child_loop(client);

  client_disconnected(client);

        close(s2);
        free(client);
        exit(0);
    }
}

int initserver(int16_t port) {
    struct sockaddr_in sock;
    int s;

    sock.sin_family = AF_INET;
    sock.sin_port = htons((int)port);
    sock.sin_addr.s_addr = inet_addr(HOST);

    s = socket(AF_INET, SOCK_STREAM, 0);
    assert(s > 0);

    errno = 0;
    if (bind(s, (struct sockaddr *)&sock, sizeof(sock)))
        assert_perror(errno);

    errno = 0;
    if (listen(s, 20))
        assert_perror(errno);

    printf("The server is running on %s:%d\n", HOST, port);
    return s;
}


int main(int argc, char *argv[]) {
    char *sport;
    int16 port;
    int s;

    kvstore.size = 0;

    if (argc < 2)
        sport = PORT;
    else
        sport = argv[1];

    port = (int16)atoi(sport);
    s = initserver(port);
    scontinuation = true;
    while (scontinuation)
        mainloop(s);

    printf("Shutting down....\n");
    close(s);
    return 0;
}
