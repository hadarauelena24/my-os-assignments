#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <stdlib.h>
#include <pthread.h>
void *threadFn6(void *unused)
{
    //srand(time(NULL));
    int nrthr=(int)((long)unused);
    info(BEGIN,6,nrthr);
    info(END,6,nrthr);
    return NULL;
}
void *threadFn3(void *unused)
{
    //srand(time(NULL));
    int nrthr=(int)((long)unused);
    info(BEGIN,3,nrthr);
    info(END,3,nrthr);
    return NULL;
}
void *threadFn5(void *unused)
{
    //srand(time(NULL));
    int nrthr=(int)((long)unused);
    info(BEGIN,5,nrthr);
    info(END,5,nrthr);
    return NULL;
}

int main()
{
    init();
    int status[7]= {-1};
    pthread_t tidp6[5]= {-1};
    pthread_t tidp3[35]= {-1};
    pthread_t tidp5[6]= {-1};
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
                for(int i=0; i<5; i++)
                {
                    pthread_create(&tidp6[i], NULL, threadFn6, (void*)(long)(i+1)) ;
                }
                for(int i=0; i<5; i++)
                {
                    pthread_join(tidp6[i], NULL);;
                }
                info(END,6,0);
                for(int i=0; i<6; i++)
                {
                    pthread_create(&tidp5[i], NULL, threadFn5, (void*)(long)(i+1)) ;
                }
                for(int i=0; i<6; i++)
                {
                    pthread_join(tidp5[i], NULL);;
                }
                exit(0);
            }
            wait(&status[5]);
            for(int i=0; i<35; i++)
            {
                pthread_create(&tidp3[i], NULL, threadFn3, (void*)(long)(i+1)) ;
            }
            for(int i=0; i<35; i++)
            {
                pthread_join(tidp3[i], NULL);;
            }
            info(END,5,0);
            exit(0);
        }
        wait(&status[4]);
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
        wait(&status[6]);
        info(END,4,0);
        exit(0);
    }
    wait(&status[1]);
    wait(&status[2]);
    wait(&status[3]);
    info(END, 1, 0);
    return 0;
}
