#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>

#define PATH "/tmp/ipc_wiki"

int q_in, q_out;

struct mymsg {
    long type;
    char text[50];
};

void signalHandler(int sig) {
    printf("\nSerwer zatrzymany. Usuwanie kolejek...\n");
    msgctl(q_in, IPC_RMID, NULL);
    msgctl(q_out, IPC_RMID, NULL);
    exit(0);
}

int main() {
    signal(SIGINT, signalHandler);

    key_t key_in = ftok(PATH, 1);
    key_t key_out = ftok(PATH, 2);

    if (key_in == -1 || key_out == -1) {
        perror("ftok");
        exit(1);
    }

    q_in = msgget(key_in, IPC_CREAT | 0600);
    q_out = msgget(key_out, IPC_CREAT | 0600);

    if (q_in == -1 || q_out == -1) {
        perror("msgget");
        exit(1);
    }

    struct mymsg msg;

    printf("Serwer uruchomiony. Oczekiwanie na wiadomości...\n");

    while (1) {
        if (msgrcv(q_in, &msg, sizeof(msg.text), 0, 0) == -1) {
            perror("msgrcv");
            continue;
        }

        printf("Odebrano od klienta [%ld]: %s\n", msg.type, msg.text);

        // Modyfikujemy wiadomość
        strncat(msg.text, " [OK]", sizeof(msg.text) - strlen(msg.text) - 1);

        // Odesłanie odpowiedzi do klienta
        if (msgsnd(q_out, &msg, sizeof(msg.text), 0) == -1) {
            perror("msgsnd");
        } else {
            printf("Odesłano odpowiedź do klienta [%ld]\n", msg.type);
        }
    }

    return 0;
}
