#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define FIFO_TO_SERVER "/tmp/fifo_to_server"
#define FIFO_TO_CLIENT "/tmp/fifo_to_client"

int main() {
    double number;
    char message[512];
    char result[600]; // większy bufor na wynik

    // Pobranie danych od użytkownika
    printf("Podaj liczbe: ");
    scanf("%lf", &number);
    printf("Podaj napis: ");
    scanf(" %[^\n]", message); // czytanie całej linii ze spacjami

    // Otwieranie FIFO
    int toServer = open(FIFO_TO_SERVER, O_WRONLY);
    int toClient = open(FIFO_TO_CLIENT, O_RDONLY);
    
    if (toServer == -1 || toClient == -1) {
        perror("Blad otwarcia FIFO");
        return 1;
    }

    // Wysyłanie danych do serwera
    write(toServer, &number, sizeof(double));
    write(toServer, message, 512);

    // Odczyt odpowiedzi od serwera
    read(toClient, result, 600);

    printf("Odpowiedz serwera: %s\n", result);

    // Zamknięcie kolejki
    close(toServer);
    close(toClient);

    return 0;
}
