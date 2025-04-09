#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <math.h>
#include <unistd.h>

#define PATH "/tmp/ipc_wiki"

int q_req, q_res;

struct request {
    long type;
    float val;
    long pow;
};

struct response {
    long type;
    float result;
};

void cleanup(int sig) {
    printf("\nZatrzymanie serwera. Usuwanie kolejek...\n");
    msgctl(q_req, IPC_RMID, NULL);
    msgctl(q_res, IPC_RMID, NULL);
    exit(0);
}

int main() {
    signal(SIGINT, cleanup);

    key_t key_req = ftok(PATH, 1);
    key_t key_res = ftok(PATH, 2);

    if (key_req == -1 || key_res == -1) {
        perror("ftok");
        exit(1);
    }

    q_req = msgget(key_req, IPC_CREAT | 0600);
    q_res = msgget(key_res, IPC_CREAT | 0600);

    if (q_req == -1 || q_res == -1) {
        perror("msgget");
        exit(1);
    }

    struct request req;
    struct response res;

    printf("Serwer uruchomiony...\n");

    while (1) {
        if (msgrcv(q_req, &req, sizeof(req) - sizeof(long), 0, 0) == -1) {
            perror("msgrcv");
            continue;
        }

        printf("Odebrano od klienta [%ld]: float=%.2f, long=%ld\n", req.type, req.val, req.pow);

        res.type = req.type;
        res.result = powf(sinf(req.val), req.pow);

        if (msgsnd(q_res, &res, sizeof(res.result), 0) == -1) {
            perror("msgsnd");
            continue;
        }

        // Czekamy na odpowiedź od klienta
        if (msgrcv(q_req, &res, sizeof(res.result), req.type, 0) == -1) {
            perror("msgrcv");
            continue;
        }

        printf("Zaktualizowany wynik od klienta [%ld]: %.4f\n", res.type, res.result);
    }

    return 0;
}
