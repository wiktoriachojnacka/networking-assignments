#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/msg.h>
//test
int msqid;

struct mymsg {
    long type;
    char text[50];
};

void signalHandler(int sig) {
    printf("\nSerwer zatrzymany. Usuwanie kolejki...\n");
    if (msgctl(msqid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
    }
    exit(0);
}

int main() {
    signal(SIGINT, signalHandler);

    msqid = msgget((key_t)69, 0600 | IPC_CREAT);
    if (msqid == -1) {
        perror("msgget");
        exit(1);
    }

    struct mymsg msg;
    printf("Serwer uruchomiony. Oczekiwanie na wiadomości...\n");

    while (1) {
        if (msgrcv(msqid, (void *)&msg, sizeof(msg.text), 0, 0) == -1) {
            perror("msgrcv");
        } else {
            printf("Odebrano od klienta: %s\n", msg.text);
        }
    }

    return 0;
}
