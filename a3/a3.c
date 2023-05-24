#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define REQ_PIPE_NAME "REQ_PIPE_13706"
#define RESP_PIPE_NAME "RESP_PIPE_13706"
#define BUFFER_SIZE 256

int main()
{
    int req_pipe, resp_pipe;
    char buffer_resp[BUFFER_SIZE];
    char buffer_req[BUFFER_SIZE];
    char request;
    unlink(RESP_PIPE_NAME);
    // Crearea pipe-ului de răspuns
    if (mkfifo(RESP_PIPE_NAME, 0666) == -1)
    {
        perror("ERROR\ncannot create the response pipe");
        exit(EXIT_FAILURE);
    }

    // Deschiderea pipe-ului de cerere în citire
    if ((req_pipe = open(REQ_PIPE_NAME, O_RDONLY)) == -1)
    {
        perror("ERROR\ncannot open the request pipe");
        exit(EXIT_FAILURE);
    }

    // Deschiderea pipe-ului de răspuns în scriere
    if ((resp_pipe = open(RESP_PIPE_NAME, O_WRONLY)) == -1)
    {
        perror("ERROR\ncannot open the response pipe");
        exit(EXIT_FAILURE);
    }

    strcpy(buffer_resp,"CONNECT#");
    if (write(resp_pipe, &buffer_resp, strlen(buffer_resp)) == -1)
    {
        perror("ERROR: Cannot write to the response pipe");
        exit(EXIT_FAILURE);
    }
    //printf("SUCCESS\n");
    int i=0;
    while(request!='#')
    {
        read(req_pipe,&request,1);
        if(request!='#')
        {
            buffer_req[i]=request;
            i++;
        }
    }
    buffer_req[i]='\0';
    if(strcmp(buffer_req,"PING")==0)
    {
        strcpy(buffer_resp,"PING#");
        write(resp_pipe,&buffer_resp,strlen(buffer_resp));
        int nr=13706;
        write(resp_pipe,&nr,sizeof(int));
        //write(resp_pipe,"PING#",strlen("PING#"));
        strcpy(buffer_resp,"PONG#");
        write(resp_pipe,&buffer_resp,strlen(buffer_resp));
        //write(resp_pipe,"PONG#",strlen("PONG#"));
    }

    //strcpy(buffer_resp,"PING 13706 PONG");
    //write(resp_pipe,&buffer_resp,sizeof(buffer_resp));
    // Închiderea pipe-urilor
    if(strcmp(buffer_req,"EXIT")==0)
    {
        close(req_pipe);
        close(resp_pipe);
        unlink(RESP_PIPE_NAME);
    }
    unsigned int dimShm=0;
    int shmFd;
    //int fd1=-1;
    void* sharedMem;
    if(strcmp(buffer_req,"CREATE_SHM")==0)
    {
        read(req_pipe,&dimShm,sizeof(unsigned int));
        write(resp_pipe,"CREATE_SHM#",strlen("CREATE_SHM#"));
        shmFd = shm_open("/cghIRY", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
        if(shmFd < 0)
        {
            write(resp_pipe,"ERROR#",strlen("ERROR#"));
            perror("Could not aquire shm");
            return 1;
        }

        ftruncate(shmFd,dimShm);
        write(resp_pipe,"SUCCESS#",strlen("SUCCESS#"));
        sharedMem = mmap(0, dimShm, PROT_READ | PROT_WRITE,MAP_SHARED, shmFd, 0);
        if(sharedMem == (void*)-1)
        {
            perror("Could not map the shared memory");
            return 1;
        }
    }
    unsigned int offsetMem=0,value=0;
    volatile unsigned int* sharedValue=0;
    if(strcmp(buffer_req,"WRITE_TO_SHM")==0)
    {
        read(req_pipe,&offsetMem,sizeof(unsigned int));
        read(req_pipe,&value,sizeof(unsigned int));
        write(resp_pipe,"WRITE_TO_SHM#",strlen("WRITE_TO_SHM#"));
        if(offsetMem>0 && offsetMem<=dimShm && offsetMem+sizeof(value)<=dimShm)
        {
            sharedValue=(volatile unsigned int*)((char*)sharedMem+offsetMem);
            *sharedValue=value;
            //write(resp_pipe,"WRITE_TO_SHM#",strlen("WRITE_TO_SHM#"));
            write(resp_pipe,"SUCCESS#",strlen("SUCCESS#"));
        }
        else
        {
            //write(resp_pipe,"WRITE_TO_SHM#",strlen("WRITE_TO_SHM#"));
            write(resp_pipe,"ERROR#",strlen("ERROR#"));
        }
    }

    munmap(sharedMem, dimShm);
    close(shmFd);
    shm_unlink("/cghIRY");
    return 0;
}
