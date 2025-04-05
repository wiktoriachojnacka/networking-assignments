#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>

#define PATH "/tmp/ipc_wiki"

struct mymsg {
    long type;        
    char text[50];    
};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Użycie: %s <wiadomość>\n", argv[0]);
        exit(1);
    }

    key_t key_in = ftok(PATH, 1);
    key_t key_out = ftok(PATH, 2);

    if (key_in == -1 || key_out == -1) {
        perror("ftok");
        exit(1);
    }

    int q_in = msgget(key_in, 0600);
    int q_out = msgget(key_out, 0600);

    if (q_in == -1 || q_out == -1) {
        perror("msgget (czy serwer działa?)");
        exit(1);
    }

    struct mymsg msg;
    msg.type = getpid(); // unikalny identyfikator klienta

    snprintf(msg.text, sizeof(msg.text), "%s", argv[1]);

    if (msgsnd(q_in, &msg, sizeof(msg.text), 0) == -1) {
        perror("msgsnd");
        exit(1);
    }

    printf("Wysłano: %s\n", msg.text);

    // Czekamy na odpowiedź z serwera
    if (msgrcv(q_out, &msg, sizeof(msg.text), msg.type, 0) == -1) {
        perror("msgrcv");
        exit(1);
    }

    printf("Odebrano odpowiedź: %s\n", msg.text);

    return 0;
}
