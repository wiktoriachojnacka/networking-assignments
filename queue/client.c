#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>

#define PATH "/tmp/ipc_wiki"

struct request {
    long type;
    float val;
    long pow;
};

struct response {
    long type;
    float result;
};

int main() {
    key_t key_req = ftok(PATH, 1);
    key_t key_res = ftok(PATH, 2);

    if (key_req == -1 || key_res == -1) {
        perror("ftok");
        exit(1);
    }

    int q_req = msgget(key_req, 0600);
    int q_res = msgget(key_res, 0600);

    if (q_req == -1 || q_res == -1) {
        perror("msgget");
        exit(1);
    }

    struct request req;
    struct response res;

    req.type = getpid();
    printf("Podaj float: ");
    scanf("%f", &req.val);
    printf("Podaj long: ");
    scanf("%ld", &req.pow);

    if (msgsnd(q_req, &req, sizeof(req) - sizeof(long), 0) == -1) {
        perror("msgsnd");
        exit(1);
    }

    if (msgrcv(q_res, &res, sizeof(res.result), req.type, 0) == -1) {
        perror("msgrcv");
        exit(1);
    }

    printf("Odebrano od serwera: %.4f\n", res.result);

    while (1) {
        float additional;
        char cont;

        printf("Podaj liczbę do dodania: ");
        scanf("%f", &additional);
        res.result += additional;

        if (msgsnd(q_req, &res, sizeof(res.result), 0) == -1) {
            perror("msgsnd");
            exit(1);
        }

        printf("Odesłano: %.4f\n", res.result);

        printf("Kontynuować? (t/n): ");
        scanf(" %c", &cont);
        if (cont == 'n' || cont == 'N') {
            // wyślij -1 jako sygnał końca sesji
            res.result = -1.0f;
            if (msgsnd(q_req, &res, sizeof(res.result), 0) == -1) {
                perror("msgsnd");
                exit(1);
            }
            printf("Zamykam klienta...\n");
            break;
        }
    }

    return 0;
}
