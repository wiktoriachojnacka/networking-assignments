#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#define FIFO_TO_SERVER "/tmp/fifo_to_server"
#define FIFO_TO_CLIENT "/tmp/fifo_to_client"

int main() {
    double number;
    char message[512];
    char response[1024];      // pełna odpowiedź
    char temp[600];           // pojedynczy kawałek
    response[0] = '\0';       // inicjalizacja pustym stringiem

    // Tworzenie kolejek FIFO, jeśli nie istnieją
    mkfifo(FIFO_TO_SERVER, 0600);
    mkfifo(FIFO_TO_CLIENT, 0600);

    // Otwieranie FIFO
    int toServer = open(FIFO_TO_SERVER, O_RDONLY);
    int toClient = open(FIFO_TO_CLIENT, O_WRONLY);

    if (toServer == -1 || toClient == -1) {
        perror("Blad otwarcia FIFO");
        return 1;
    }

    while (1) {
        // Odczyt danych od klienta
        ssize_t read_bytes = read(toServer, &number, sizeof(double));
        if (read_bytes <= 0) break; // klient zakończył

        read(toServer, message, 512);

        // Doklej nową część do odpowiedzi
        snprintf(temp, sizeof(temp), "%.2lf %s ", number, message);
        strncat(response, temp, sizeof(response) - strlen(response) - 1);

        // Odesłanie zaktualizowanej odpowiedzi do klienta
        write(toClient, response, strlen(response) + 1);

        printf("Serwer wyslal: %s\n", response);
    }

    // Zamknięcie kolejki
    close(toServer);
    close(toClient);
    unlink(FIFO_TO_SERVER);
    unlink(FIFO_TO_CLIENT);

    return 0;
}
