#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

typedef struct
{
    int id;
    sem_t *logSem2;
    sem_t *logSem3;
} TH_STRUCT;

void *threadFn6(void *unused)
{
    TH_STRUCT *s=(TH_STRUCT*)unused;
    //srand(time(NULL));
    int nrthr=s->id;
    if(nrthr==3)
        sem_wait(s->logSem2);
    //syncro_threadp6_st(s->logSem3);
    info(BEGIN,6,nrthr);
    if(nrthr==2)
        sem_post(s->logSem2);
    if(nrthr==2)
        sem_wait(s->logSem3);
    info(END,6,nrthr);
    if(nrthr==3)
        sem_post(s->logSem3);
    return NULL;
}

void *threadFn3(void *unused)
{
    TH_STRUCT *s=(TH_STRUCT*)unused;
    //srand(time(NULL));
    int nrthr=s->id;
    info(BEGIN,3,nrthr);
    info(END,3,nrthr);
    return NULL;
}

void *threadFn5(void *unused)
{
    TH_STRUCT *s=(TH_STRUCT*)unused;
    //srand(time(NULL));
    int nrthr=s->id;
    info(BEGIN,5,nrthr);
    info(END,5,nrthr);
    return NULL;
}

int main()
{
    init();
    sem_t logSem2;
    sem_t logSem3;

    TH_STRUCT paramsp6[5];
    TH_STRUCT paramsp3[35];
    TH_STRUCT paramsp5[6];
    //int status[7]= {-1};
    pthread_t tidp6[5]= {0};
    pthread_t tidp3[35]= {0};
    pthread_t tidp5[6]= {0};
    //pid_t pid[7]= {-1};
    info(BEGIN, 1, 0);
    if(fork()==0)
    {
        info(BEGIN,2,0);
        info(END,2,0);
        exit(0);
    }
    if(fork()==0)
    {
        info(BEGIN,3,0);

        if(fork()==0)
        {
            info(BEGIN,5,0);

            if(fork()==0)
            {
                info(BEGIN,6,0);
                if(sem_init(&logSem2, 0, 0) != 0)
                {
                    perror("Could not init the semaphore2");
                    return -1;
                }
                if(sem_init(&logSem3, 0, 0) != 0)
                {
                    perror("Could not init the semaphore3");
                    return -1;
                }
                for(int i=0; i<5; i++)
                {
                    paramsp6[i].id=i+1;
                    paramsp6[i].logSem2 = &logSem2;
                    paramsp6[i].logSem3 = &logSem3;
                    pthread_create(&tidp6[i], NULL, threadFn6, (void*)&paramsp6[i]);
                }
                for(int i=0; i<5; i++)
                {
                    pthread_join(tidp6[i], NULL);
                }
                info(END,6,0);
                exit(0);
            }
            for(int i=0; i<6; i++)
            {
                paramsp5[i].id=i+1;
                pthread_create(&tidp5[i], NULL, threadFn5, (void*)&paramsp5[i]);
            }
            for(int i=0; i<6; i++)
            {
                pthread_join(tidp5[i], NULL);
            }
            //wait(&status[5]);
            wait(NULL);
            info(END,5,0);
            exit(0);

        }
        for(int i=0; i<35; i++)
        {
            paramsp3[i].id=i+1;
            pthread_create(&tidp3[i], NULL, threadFn3, (void*)&paramsp3[i]);
        }
        for(int i=0; i<35; i++)
        {
            pthread_join(tidp3[i], NULL);
        }
        //wait(&status[4]);
        wait(NULL);
        info(END,3,0);
        exit(0);
    }
    if(fork()==0)
    {
        info(BEGIN,4,0);
        if(fork()==0)
        {
            info(BEGIN,7,0);
            info(END,7,0);
            exit(0);
        }
        //wait(&status[6]);
        wait(NULL);
        info(END,4,0);
        exit(0);
    }
    //wait(&status[1]);
    //wait(&status[2]);
    //wait(&status[3]);
    wait(NULL);
    wait(NULL);
    wait(NULL);
    sem_destroy(&logSem2);
    sem_destroy(&logSem3);
    info(END, 1, 0);
    return 0;
}
