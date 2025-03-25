#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int file;

void signalHandler(int sig) {
    printf("Server stopping...\n");
    close(file);
    unlink("fifo");  // Usunięcie FIFO po zamknięciu
    exit(0);
}

int main() {
    signal(SIGINT, signalHandler);

    if (mkfifo("fifo", 0600) == -1) {
        perror("mkfifo");
        return 1;
    }

    char fromClient[50];

    while (1) {
        file = open("fifo", O_RDONLY);
        if (file == -1) {
            perror("open");
            return 1;
        }

        if (read(file, fromClient, 50) == -1) {
            perror("read");
        } else {
            printf("From client: %s\n", fromClient);
        }

        close(file);
        sleep(1);
    }

    return 0;
}
