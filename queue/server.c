#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/ipc.h>
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
    msgctl(q_req, IPC_RMID, NULL);
    msgctl(q_res, IPC_RMID, NULL);
    exit(0);
}

int main() {
    signal(SIGINT, cleanup);

    key_t key_req = ftok(PATH, 1);
    key_t key_res = ftok(PATH, 2);

    q_req = msgget(key_req, IPC_CREAT | 0600);
    q_res = msgget(key_res, IPC_CREAT | 0600);

    long active_client = 0;

    while (1) {
        struct request req;
        struct response res;

        // Jeśli nie ma aktywnej sesji – odbierz nowe dane klienta
        if (active_client == 0 && msgrcv(q_req, &req, sizeof(req) - sizeof(long), 0, 0) != -1) {
            active_client = req.type;
            float result = powf(sinf(req.val), req.pow);
            res.type = req.type;
            res.result = result;
            msgsnd(q_res, &res, sizeof(res.result), 0);
        }

        // Odbieranie aktualizacji od klienta
        while (msgrcv(q_req, &res, sizeof(res.result), 0, IPC_NOWAIT) != -1) {
            if (res.type == active_client) {
                if (res.result == -1.0f) {
                    active_client = 0;
                } else {
                    printf("Serwer odebrał: %.4f\n", res.result);
                }
            }
        }

        usleep(100000); // 0.1 sekundy
    }

    return 0;
}
