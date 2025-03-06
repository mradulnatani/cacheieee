#include "cacheme.h"

bool scontinuation = true;


void handle_signal(int signum) {
    scontinuation = false;
}

void mainloop(int16 port) {
    struct sockaddr_in sock;
    int s;

    sock.sin_family = AF_INET;
    sock.sin_port = htons(port);
    sock.sin_addr.s_addr = inet_addr(HOST);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    if (bind(s, (struct sockaddr *)&sock, sizeof(sock)) < 0) {
        perror("Bind failed");
        close(s);
        exit(EXIT_FAILURE);
    }

    if (listen(s, 20) < 0) {
        perror("Listen failed");
        close(s);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", port);

    while (scontinuation) {
        sleep(1); 
    }

    printf("Shutting down server...\n");
    close(s);
}

int main(int argc, char *argv[]) {
    char *sport;
    int16 port;
    char port_str[6];

    if (argc < 2) {
        snprintf(port_str, sizeof(port_str), "%d", PORT);
        sport = port_str;
    } else {
        sport = argv[1];
    }

    port = (int16)strtol(sport, NULL, 10);
    
    signal(SIGINT, handle_signal);

    mainloop(port);

    return 0;
}
