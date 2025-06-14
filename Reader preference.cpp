#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

int readcount = 0;
int data = 0;

sem_t mutex;     // readcount 보호용
sem_t wrt;       // writer 진입 제어용

void* reader(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 3; i++) {
        sem_wait(&mutex);
        readcount++;
        if (readcount == 1)
            sem_wait(&wrt);  
        sem_post(&mutex);

        printf("Reader %d: read data = %d\n", id, data);

        sem_wait(&mutex);
        readcount--;
        if (readcount == 0)
            sem_post(&wrt); 
        sem_post(&mutex);
    }
    return NULL;
}

void* writer(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 3; i++) {
        sem_wait(&wrt);
        data++;
        printf("Writer %d: wrote data = %d\n", id, data);
        sem_post(&wrt);
    }
    return NULL;
}

int main() {
    pthread_t r[3], w[2];
    int rid[3] = { 1, 2, 3 };
    int wid[2] = { 1, 2 };

    sem_init(&mutex, 0, 1);
    sem_init(&wrt, 0, 1);

    for (int i = 0; i < 3; i++)
        pthread_create(&r[i], NULL, reader, &rid[i]);

    for (int i = 0; i < 2; i++)
        pthread_create(&w[i], NULL, writer, &wid[i]);

    for (int i = 0; i < 3; i++)
        pthread_join(r[i], NULL);
    for (int i = 0; i < 2; i++)
        pthread_join(w[i], NULL);

    sem_destroy(&mutex);
    sem_destroy(&wrt);
    return 0;
}