#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

#define EXIT_OK 0
#define ERR_BAD_ARG 1
#define ERR_CONV 2
#define ERR_TH_CREATE 3
#define ERR_TH_JOIN 4

#define ITERATIONS_NUMBER 20000
#define ARGUMENTS_NUMBER 2

#define MIN_THREAD_COUNT 1
#define MAX_THREAD_COUNT ITERATIONS_NUMBER/2

struct Args {
    int serial_offset;
    double partial_sum;
    int threads_number;
    int iteration_number;
};

void *calc_partial_sum(void *args) {
    if (NULL == args) {
        return (void *) ERR_BAD_ARG;
    }
    struct Args *param = (struct Args *) args;
    param->partial_sum = 0;
    param->iteration_number = 0;

/*    for (int i = param->serial_offset; i < ITERATIONS_NUMBER; i += param->threads_number) {
        ((struct Args *) args)->partial_sum += 1.0 / (i * 4.0 + 1.0);
        ((struct Args *) args)->partial_sum -= 1.0 / (i * 4.0 + 3.0);
        param->iteration_number++;
    }*/

    int iterations_per_thread = ITERATIONS_NUMBER / param->threads_number;
    int offset = iterations_per_thread * param->serial_offset;

    for (int i = offset; i < offset + iterations_per_thread; i ++){
        ((struct Args *) args)->partial_sum += 1.0 / (i * 4.0 + 1.0);
        ((struct Args *) args)->partial_sum -= 1.0 / (i * 4.0 + 3.0);
        param->iteration_number++;
    }
    return EXIT_OK;
}

int conv_arguments(int argc, char **argv, int *threads_number) {
    char *invalid_characters;

    if (argc != ARGUMENTS_NUMBER) {
        fprintf(stderr, "Bad input, expected %d arguments\n", ARGUMENTS_NUMBER - 1);
        return ERR_BAD_ARG;
    }
    errno = 0;
    long scanned_number = strtol(argv[1], &invalid_characters, 10);
    if (0 != errno) {
        perror("Cannot convert string argument");
        return ERR_CONV;
    }
    if (invalid_characters == argv[1]) {
        fprintf(stderr, "No digits were found\n");
        return ERR_CONV;
    }
    if (scanned_number > MAX_THREAD_COUNT || scanned_number < MIN_THREAD_COUNT) {
        fprintf(stderr, "Thread count must be between %d and %d\n", MIN_THREAD_COUNT, MAX_THREAD_COUNT);
        return ERR_CONV;
    }

    *threads_number = (int) scanned_number;
    return EXIT_OK;
}

int create_threads(pthread_t *threads, struct Args *params, int threads_number) {
    for (int i = 0; i < threads_number; i++) {
        params[i].serial_offset = i;
        params[i].threads_number = threads_number;

        int creation_code = pthread_create(&threads[i], NULL, calc_partial_sum, (void *) &params[i]);
        if (0 != creation_code) {
            fprintf(stderr, "Cannot create thread #%d: %s\n", i, strerror(creation_code));
            return ERR_TH_CREATE;
        }
    }
    return EXIT_OK;
}

int join_threads(int threads_number, pthread_t *threads) {
    int exit_code = EXIT_OK;
    for (int i = 0; i < threads_number; i++) {
        void *return_value;
        int join_code = pthread_join(threads[i], &return_value);
        if (0 != join_code) {
            fprintf(stderr, "Cannot join child thread: %s\n", strerror(join_code));
            exit_code = ERR_TH_JOIN;
        }

        if (EXIT_OK != (long) return_value) {
            fprintf(stderr, "Illegal thread argument");
            exit_code = ERR_BAD_ARG;
        }
    }

    return exit_code;
}

int main(int argc, char *argv[]) {
    int threads_number;
    int conv_code = conv_arguments(argc, argv, &threads_number);
    if (EXIT_OK != conv_code) {
        return EXIT_FAILURE;
    }
    pthread_t threads[threads_number];
    struct Args *params = (struct Args *) malloc(sizeof(struct Args) * threads_number);
    if (NULL == params) {
        fprintf(stderr, "Cannot allocate memory\n");
        return EXIT_FAILURE;
    }

    int creation_code = create_threads(threads, params, threads_number);
    if (EXIT_OK != creation_code) {
        free(params);
        return EXIT_FAILURE;
    }

    int join_code = join_threads(threads_number, threads);
    if (EXIT_OK != join_code) {
        free(params);
        return EXIT_FAILURE;
    }

    double result = 0;
    int iteration_number = 0;

    for (int i = 0; i < threads_number; i++) {
        result += (params[i]).partial_sum;
        iteration_number += (params[i]).iteration_number;
    }

    result = result * 4.0;
    printf("iteration_number = %d \n", iteration_number);
    printf("pi done = %.15g \n", result);

    free(params);
    return EXIT_OK;
}