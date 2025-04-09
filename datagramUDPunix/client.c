#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>

#define SERVER_SOCK_PATH "/tmp/serverDatagram1"
#define CLIENT_SOCK_PATH "/tmp/clientDatagram1"

int main() {
    struct sockaddr_un server_addr, client_addr;
    int sfd = socket(AF_UNIX, SOCK_DGRAM, 0);

    if (sfd == -1) {
        perror("socket");
        return -1;
    }

    // Usunięcie starego pliku gniazda klienta, jeśli istnieje
    unlink(CLIENT_SOCK_PATH);

    // Adres klienta
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sun_family = AF_UNIX;
    strcpy(client_addr.sun_path, CLIENT_SOCK_PATH);

    if (bind(sfd, (struct sockaddr *)&client_addr, sizeof(client_addr)) == -1) {
        perror("bind (client)");
        close(sfd);
        return -1;
    }

    // Adres serwera
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SERVER_SOCK_PATH);

    double number, result;
    char cont;

    do {
        printf("Podaj liczbę (double): ");
        scanf("%lf", &number);

        // Wysłanie liczby do serwera
        if (sendto(sfd, &number, sizeof(double), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
            perror("sendto");
            break;
        }

        // Odbiór wyniku od serwera
        if (recvfrom(sfd, &result, sizeof(double), 0, NULL, NULL) == -1) {
            perror("recvfrom");
            break;
        }

        printf("Otrzymany wynik od serwera: %.2f\n", result);

        result += 5.14;

        // Wysłanie zaktualizowanego wyniku
        if (sendto(sfd, &result, sizeof(double), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
            perror("sendto");
            break;
        }

        printf("Wysłano zaktualizowany wynik: %.2f\n", result);

        printf("Chcesz kontynuować? (t/n): ");
        scanf(" %c", &cont);

    } while (cont == 't' || cont == 'T');

    // Sprzątanie
    close(sfd);
    unlink(CLIENT_SOCK_PATH);

    return 0;
}
