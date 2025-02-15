#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void primes(int input_pipe[2]) __attribute__((noreturn));

void primes(int input_pipe[2]) {
    int prime, number;
    int output_pipe[2];
    close(input_pipe[1]);

    if (read(input_pipe[0], &prime, sizeof(prime)) == 0) {
        close(input_pipe[0]);
        exit(0);
    }
    printf("prime %d\n", prime);

    if (pipe(output_pipe) < 0) {
        printf("pipe failed\n");
        exit(1);
    }

    if (fork() == 0) {
        close(input_pipe[0]);
        primes(output_pipe);
    } else {
        close(output_pipe[0]);
        while (read(input_pipe[0], &number, sizeof(number)) > 0) {
            if (number % prime != 0) {
                write(output_pipe[1], &number, sizeof(number));
            }
        }
        close(input_pipe[0]);
        close(output_pipe[1]);
        wait(0);
        exit(0);
    }
}

int main() {
    int value;
    int first_pipe[2];

    if (pipe(first_pipe) < 0) {
        printf("pipe failed\n");
        exit(1);
    }

    if (fork() == 0) {
        primes(first_pipe);
    } else {
        close(first_pipe[0]);
        for (value = 2; value <= 280; value++) {
            if (write(first_pipe[1], &value, sizeof(value)) < 0) {
                printf("write failed\n");
                exit(1);
            }
        }
        close(first_pipe[1]);
        wait(0);
        exit(0);
    }
}
