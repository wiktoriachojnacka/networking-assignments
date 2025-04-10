#include <stdio.h>
#include <stdlib.h>
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

    int q_req = msgget(key_req, 0600);
    int q_res = msgget(key_res, 0600);

    struct request req;
    struct response res;

    req.type = getpid();
    printf("Float: ");
    scanf("%f", &req.val);
    printf("Long: ");
    scanf("%ld", &req.pow);

    msgsnd(q_req, &req, sizeof(req) - sizeof(long), 0);
    msgrcv(q_res, &res, sizeof(res.result), req.type, 0);

    printf("Wynik: %.4f\n", res.result);

    while (1) {
        float add;
        char cont;

        printf("Dodaj: ");
        scanf("%f", &add);
        res.result += add;
        msgsnd(q_req, &res, sizeof(res.result), 0);
        printf("Wysłano: %.4f\n", res.result);

        printf("Kontynuować? (t/n): ");
        scanf(" %c", &cont);
        if (cont == 'n' || cont == 'N') {
            res.result = -1.0f;
            msgsnd(q_req, &res, sizeof(res.result), 0);
            break;
        }
    }

    return 0;
}
