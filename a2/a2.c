#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <stdlib.h>


int main()
{
    init();
    int status[7]= {-1};
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
                info(END,6,0);
                exit(0);
            }
            wait(&status[5]);
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
