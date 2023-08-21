#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("usage:\n\tcrono MINUTES SECONDS\n");
        return 1;
    }

    int minutes = atoi(argv[1]);
    int seconds = atoi(argv[2]);

    while (minutes != 0 || seconds != 0) {
        printf("%02d:%02d\n", minutes, seconds);

        if (seconds == 0) minutes--;
        seconds = (seconds + 60 - 1) % 60;

        sleep(1);
    }

    return 0;
}
