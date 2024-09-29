#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX 102400
#define NUM_THREADS 20

int n1, n2;
char *s1, *s2;

pthread_mutex_t lock;
int totalNum = 0;

int readf(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("ERROR: can't open emoji.txt!\n");
        return 0;
    }
    s1 = (char *)malloc(sizeof(char) * MAX);
    s2 = (char *)malloc(sizeof(char) * MAX);
    if (s1 == NULL || s2 == NULL) {
        printf("ERROR: Out of memory!\n");
        return -1;
    }
    s1 = fgets(s1, MAX, fp);
    s2 = fgets(s2, MAX, fp);
    n1 = strlen(s1);
    n2 = strlen(s2) - 1;
    
    if (s2[n2] == '\n') s2[n2] = '\0'; 

    fclose(fp);
    if (s1 == NULL || s2 == NULL || n1 < n2) return -1;
    return 0;
}

void *calSubStringThread(void *threadid) {
    long tid = (long)threadid;
    int start = tid * (n1 / NUM_THREADS);
    int end = (tid == NUM_THREADS - 1) ? n1 : start + (n1 / NUM_THREADS) + n2 - 1;

    if (start != 0) {
        start -= n2 - 1;
    }

    int count = 0;
    for (int i = start; i < end && i + n2 <= n1; i++) {
        if (strncmp(s1 + i, s2, n2) == 0) {
            count++;
        }
    }

    pthread_mutex_lock(&lock);
    totalNum += count;
    pthread_mutex_unlock(&lock);

    printf("Thread %ld found %d occurrences.\n", tid, count);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t threads[NUM_THREADS];
    int rc;

    if (readf("emoji.txt") != 0) {
        exit(-1);
    }

    pthread_mutex_init(&lock, NULL);

    for (int t = 0; t < NUM_THREADS; t++) {
        rc = pthread_create(&threads[t], NULL, calSubStringThread, (void *)(size_t)t);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    pthread_mutex_destroy(&lock);

    printf("Thread %ld found %d occurrences.\n", tid, count);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t threads[NUM_THREADS];
    int rc;

    if (readf("emoji.txt") != 0) {
        exit(-1);
    }

    pthread_mutex_init(&lock, NULL);

    for (int t = 0; t < NUM_THREADS; t++) {
        rc = pthread_create(&threads[t], NULL, calSubStringThread, (void *)(size_t)t);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    pthread_mutex_destroy(&lock);

    printf("The total number of matched substrings is: %d\n", totalNum);
    free(s1);
    free(s2);
    return 1;
}
