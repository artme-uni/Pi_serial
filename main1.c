#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define printError(text, error) fprintf(stderr, text": %s\n",strerror(error));
#define NO_ERROR 0
#define THREADS_COUNT 10000

void *printLines(void *args) {
    sleep(3);
    return NULL;
}

int main(int argc, char **argv) {
    pthread_t thread[THREADS_COUNT];
    int working_threads_count = 0;

    int return_value = EXIT_SUCCESS;
    int error = NO_ERROR;
    while (error == NO_ERROR){
        error = pthread_create(&thread[working_threads_count], NULL, printLines, NULL);
        if (error == NO_ERROR) {
            working_threads_count++;
        }
    }

    for (int i = 0; i < working_threads_count; i++) {
        int join_code = pthread_join(thread[i], NULL);
        if (0 != join_code) {
            printError("Could not join thread", error);
            return_value = EXIT_FAILURE;
        }
    }

    printf("Max thread number: %d\n", working_threads_count);

    return return_value;
}