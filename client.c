#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <message>\n", argv[0]);
        return 1;
    }

    int file = open("fifo", O_WRONLY);
    if (file == -1) {
        perror("open");
        return 1;
    }

    if (write(file, argv[1], 50) == -1) {
        perror("write");
    }
    
    close(file);
    return 0;
}
