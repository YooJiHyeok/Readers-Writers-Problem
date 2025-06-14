# Readers-Writers Problem
# Reader preference
## Code

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
## Implementation Result
![readers](https://github.com/YooJiHyeok/Readers-Writers-Problem/blob/main/reader_prefernece.png)
## Evaluation
Reader가 우선적으로 처리되는 것을 볼 수 있다.

# Writer Preference
## Code

    #include <stdio.h>
    #include <pthread.h>
    #include <semaphore.h>

    int readcount = 0;
    int writecount = 0;
    int data = 0;

    sem_t rmutex;     // readcount 보호
    sem_t wmutex;     // writecount 보호
    sem_t readTry;    // reader 접근 제어
    sem_t resource;   // 실제 데이터 접근

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

## Implementation Result
![writers](https://github.com/YooJiHyeok/Readers-Writers-Problem/blob/main/writers_preference.png)
## Evaluation
writer가 우선적으로 처리되는 것을 볼 수 있다.
## Conclusion
코드 구현이 어려워 코딩 ai(챗 gpt 및 Gemini)를 일부 참고하여 구현 하였다. 앞으로의 진로 및 실무를 위해서 추가적인 공부의 필요성을 느꼈다.
