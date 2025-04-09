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
    printf("\nUsuwanie kolejek...\n");
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

    printf("Serwer uruchomiony. Oczekiwanie na zapytania...\n");

    long active_client = 0;

    while (1) {
        struct request req;
        struct response res;

        // Odbieranie pierwszego zapytania (float + long)
        if (msgrcv(q_req, &req, sizeof(req) - sizeof(long), 0, IPC_NOWAIT) != -1) {
            // Jeśli nie ma aktywnej sesji — przyjmij nowego klienta
            if (active_client == 0) {
                active_client = req.type;
                float sinval = sinf(req.val);
                float result = powf(sinval, req.pow);

                res.type = req.type;
                res.result = result;

                printf("Obliczono sin(%.2f)^%ld = %.4f\n", req.val, req.pow, result);

                if (msgsnd(q_res, &res, sizeof(res.result), 0) == -1) {
                    perror("msgsnd");
                }
            } else if (req.type != active_client) {
                // Ktoś inny próbuje się wbić — ignorujemy
                printf("Ignoruję klienta [PID=%ld], trwa sesja z klientem [PID=%ld]\n", req.type, active_client);
            }
        }

        // Odbieranie zaktualizowanych wyników tylko od aktywnego klienta
        while (msgrcv(q_req, &res, sizeof(res.result), 0, IPC_NOWAIT) != -1) {
            if (res.type == active_client) {
                if (res.result == -1.0f) {
                    printf("Zamykam sesję klienta [PID=%ld]\n", active_client);
                    active_client = 0;
                } else {
                    printf("Odebrano od klienta [PID=%ld] nowy wynik: %.4f\n", res.type, res.result);
                }
            } else {
                // ignorujemy innych klientów
                printf("Ignoruję wynik od klienta [PID=%ld] — trwa sesja z [PID=%ld]\n", res.type, active_client);
            }
        }

        usleep(100000); // 0.1 sekundy oddechu
    }

    return 0;
}
