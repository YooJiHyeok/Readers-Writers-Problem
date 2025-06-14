#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

int readcount = 0;
int writecount = 0;
int data = 0;

sem_t rmutex;     // readcount ��ȣ
sem_t wmutex;     // writecount ��ȣ
sem_t readTry;    // reader ���� ����
sem_t resource;   // ���� ������ ����

void* reader(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 3; i++) {
        sem_wait(&readTry);
        sem_wait(&rmutex);
        readcount++;
        if (readcount == 1)
            sem_wait(&resource);
        sem_post(&rmutex);
        sem_post(&readTry);

        printf("Reader %d: read data = %d\n", id, data);

        sem_wait(&rmutex);
        readcount--;
        if (readcount == 0)
            sem_post(&resource);
        sem_post(&rmutex);
    }
    return NULL;
}

void* writer(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 3; i++) {
        sem_wait(&wmutex);
        writecount++;
        if (writecount == 1)
            sem_wait(&readTry);
        sem_post(&wmutex);

        sem_wait(&resource);
        data++;
        printf("Writer %d: wrote data = %d\n", id, data);
        sem_post(&resource);

        sem_wait(&wmutex);
        writecount--;
        if (writecount == 0)
            sem_post(&readTry);
        sem_post(&wmutex);
    }
    return NULL;
}

int main() {
    pthread_t r[3], w[2];
    int rid[3] = { 1, 2, 3 };
    int wid[2] = { 1, 2 };

    sem_init(&rmutex, 0, 1);
    sem_init(&wmutex, 0, 1);
    sem_init(&readTry, 0, 1);
    sem_init(&resource, 0, 1);

    for (int i = 0; i < 3; i++)
        pthread_create(&r[i], NULL, reader, &rid[i]);

    for (int i = 0; i < 2; i++)
        pthread_create(&w[i], NULL, writer, &wid[i]);

    for (int i = 0; i < 3; i++)
        pthread_join(r[i], NULL);
    for (int i = 0; i < 2; i++)
        pthread_join(w[i], NULL);

    sem_destroy(&rmutex);
    sem_destroy(&wmutex);
    sem_destroy(&readTry);
    sem_destroy(&resource);

    return 0;
}
