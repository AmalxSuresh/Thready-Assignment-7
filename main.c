#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <pthread.h>

typedef struct {
    int thread_number;
    int thread_total;
} thread_arguments;

void* thread_identifier(void* vptr);
void pattern_one(int things);
void pattern_two(int things);
int random_time();

int main(int argc, char *argv[]) {
    srand(time(NULL));
    int things, pattern_number;
    
    if (argc > 2) {
        sscanf(argv[1], "%d", &things);
        sscanf(argv[2], "%d", &pattern_number);

    } else {
        return 1; 
    }
    printf("this is things - %d, and this is pattern number - %d\n", things, pattern_number);
    if(pattern_number == 1) {
        pattern_one(things);
    }
    else if(pattern_number == 2) {
        pattern_two(things);
    }
}

//time function
int random_time() {
    int random_time;
    random_time = (rand() % 8) + 1;
    return random_time;
}

//create thread process and recursive processes
void* thread_identifier(void* vptr) {
    //access thread arguments structure and be used by pthread_create
    thread_arguments* arguments = (thread_arguments*) vptr;
    int thread_number = arguments -> thread_number;
    //keep track of total number of things for recursive function
    int thread_total = arguments -> thread_total;
    int time = random_time();

    printf("Thread %d is starting with address %p\n", thread_number, pthread_self());
    printf("Thread %d with address: %p will sleep for %d seconds\n", thread_number, pthread_self(), time);
    sleep(time);

    if(thread_number < thread_total) {
        //repeat code for creating a new thread. Update thread_arguments structure with new thread data
        pthread_t new_thread;
        thread_arguments* new_arguments = malloc(sizeof(thread_arguments));
        new_arguments -> thread_number = thread_number + 1;
        new_arguments -> thread_total = thread_total;

        printf("Thread %d is creating thread %d\n", thread_number, new_arguments -> thread_number);
        if (pthread_create(&new_thread, NULL, thread_identifier, new_arguments) != 0) {
            return NULL;
        } else {
            pthread_join(new_thread, NULL); 
        }
    }
    printf("Thread %d is now exiting with address: %p\n", thread_number, pthread_self());
    return NULL;
}

void pattern_one(int things) {
    pthread_t threads[things];

    for (int ix = 0; ix < things; ++ix) {
        //allocate memory to structure and make it a pointer to be accessible to thread_identifier function
        thread_arguments* arguments = malloc(sizeof(thread_arguments));
        arguments -> thread_number = ix + 1;
        if (pthread_create(&threads[ix], NULL, thread_identifier, arguments) != 0) {
            return 1;
        }
    }

    for (int nx = 0; nx < things; ++nx) {
        if (pthread_join(threads[nx], NULL) != 0) {
            return 1;
        }
    }
}

void pattern_two(int things) {
    pthread_t thread;
    thread_arguments* arguments = malloc(sizeof(thread_arguments));
    arguments -> thread_number = 1;
    arguments -> thread_total = things;

    if (pthread_create(&thread, NULL, thread_identifier, arguments) != 0) {
        free(arguments);
        return 1;
    }
    
    pthread_join(thread, NULL);
    
}