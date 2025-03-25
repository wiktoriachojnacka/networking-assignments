#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>
//test
struct mymsg {
    long type;        /* Typ wiadomości */
    char text[50];    /* Treść wiadomości */
};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Użycie: %s <wiadomość>\n", argv[0]);
        exit(1);
    }

    int msqid = msgget((key_t)69, 0600 | IPC_CREAT);
    if (msqid == -1) {
        perror("msgget");
        exit(1);
    }

    struct mymsg msg;
    msg.type = 1;  // Typ wiadomości (musi być > 0)
    strncpy(msg.text, argv[1], sizeof(msg.text) - 1);
    msg.text[sizeof(msg.text) - 1] = '\0';  // Zapobieganie przepełnieniu bufora

    if (msgsnd(msqid, (void *)&msg, sizeof(msg.text), 0) == -1) {
        perror("msgsnd");
        exit(1);
    }

    printf("Wysłano: %s\n", msg.text);
    return 0;
}
