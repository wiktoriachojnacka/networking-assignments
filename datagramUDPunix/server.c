#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <math.h>
#include <signal.h>

#define MY_SOCK_PATH "/tmp/serverDatagram1"

int sfd;

void signalHandler(int sig) {
    printf("\nZatrzymywanie serwera...\n");
    close(sfd);
    unlink(MY_SOCK_PATH);  // Usuwanie pliku gniazda
    exit(0);
}

int main() {
    struct sockaddr_un server_addr, client_addr;
    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1) {
        perror("socket");
        return -1;
    }

    // Usunięcie starego pliku gniazda serwera, jeśli istnieje
    unlink(MY_SOCK_PATH);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, MY_SOCK_PATH);

    if (bind(sfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        return -1;
    }

    // Rejestracja obsługi sygnału SIGINT (np. Ctrl+C)
    signal(SIGINT, signalHandler);

    printf("Serwer nasłuchuje na %s...\n", MY_SOCK_PATH);

    double number;

    while (1) {
        socklen_t client_addr_size = sizeof(client_addr);
        ssize_t len = recvfrom(sfd, &number, sizeof(number), 0, (struct sockaddr *) &client_addr, &client_addr_size);
        if (len == -1) {
            perror("recvfrom");
            continue;
        }

        printf("Otrzymano liczbę: %.2lf\n", number);

        double result;
        if (number > 0) {
            result = sin(number);
        } else {
            result = pow(2, number);
        }

        printf("Wynik obliczeń: %.2lf\n", result);
        printf("Serwer odpowiada klientowi na ścieżkę: %s\n", client_addr.sun_path);

        ssize_t send_len = sendto(sfd, &result, sizeof(result), 0, (struct sockaddr *) &client_addr, client_addr_size);
        if (send_len == -1) {
            perror("sendto");
            continue;
        }
    }

    close(sfd);
    unlink(MY_SOCK_PATH);
    return 0;
}
