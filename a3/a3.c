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
    char file_name[BUFFER_SIZE];
    char readdata[BUFFER_SIZE];
    char rddatasect[BUFFER_SIZE];
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
    unsigned int dimShm=0;
    int shmFd=-1;
    void* sharedZone;
    unsigned int offsetZone=0,value=0;
    unsigned int offsetFile=0,noBytes=0;
    volatile unsigned int* sharedValue=0;
    unsigned int sectionNo=0,offsetSect=0,noBSect=0;
    int noSections=0;
    unsigned short hdsz=0;
    unsigned int offsetSection=0;
    char* data=NULL;
    int sizedata=0;
    int fd=-1;
    while(1==1)
    {
        while(request!='#')
        {
            read(req_pipe,&request,1);
            if(request!='#')
            {
                buffer_req[i]=request;
                i++;
            }
            else
            {
                buffer_req[i]='\0';
                i=0;
            }
        }
        request='0';
        //buffer_req[i]='\0';
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


        //int fd1=-1;
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
            sharedZone = mmap(0, dimShm, PROT_READ | PROT_WRITE,MAP_SHARED, shmFd, 0);
            if(sharedZone == (void*)-1)
            {
                perror("Could not map the shared memory");
                return 1;
            }
        }

        if(strcmp(buffer_req,"WRITE_TO_SHM")==0)
        {
            read(req_pipe,&offsetZone,sizeof(unsigned int));
            read(req_pipe,&value,sizeof(unsigned int));
            write(resp_pipe,"WRITE_TO_SHM#",strlen("WRITE_TO_SHM#"));
            if(offsetZone>0 && offsetZone<=dimShm && offsetZone+sizeof(value)<=dimShm)
            {
                sharedValue=(volatile unsigned int*)((char*)sharedZone+offsetZone);
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

        if(strcmp(buffer_req,"MAP_FILE")==0)
        {
            write(resp_pipe,"MAP_FILE#",strlen("MAP_FILE#"));
            while(request!='#')
            {
                read(req_pipe,&request,1);
                if(request!='#')
                {
                    file_name[i]=request;
                    i++;
                }
                else
                {
                    file_name[i]='\0';
                    i=0;
                }
            }
            request='0';
            fd=open(file_name,O_RDONLY);
            sizedata=lseek(fd,0,SEEK_END);
            lseek(fd,0,SEEK_SET);
            data=(char*)mmap(NULL,sizedata,PROT_READ,MAP_SHARED,fd,0);
            if(data==(void*)-1)
            {
                write(resp_pipe,"ERROR#",strlen("ERROR#"));
                perror("Could not map file");
                close(fd);
                return 1;
            }
            write(resp_pipe,"SUCCESS#",strlen("SUCCESS#"));
        }


        if(strcmp(buffer_req,"READ_FROM_FILE_OFFSET")==0)
        {
            write(resp_pipe,"READ_FROM_FILE_OFFSET#",strlen("READ_FROM_FILE_OFFSET#"));
            read(req_pipe,&offsetFile,sizeof(unsigned int));
            read(req_pipe,&noBytes,sizeof(unsigned int));
            for(int i=0; i<noBytes; i++)
            {
                readdata[i]=data[i+offsetFile];
            }
            readdata[noBytes]='\0';
            if(offsetFile>0 && offsetFile<=sizedata && offsetFile+noBytes<=sizedata)
            {
                memcpy(sharedZone,&readdata,noBytes);

                write(resp_pipe,"SUCCESS#",strlen("SUCCESS#"));
            }
            else
            {
                write(resp_pipe,"ERROR#",strlen("ERROR#"));
            }
        }
        if(strcmp(buffer_req,"READ_FROM_FILE_SECTION")==0)
        {
            read(req_pipe,&sectionNo,sizeof(unsigned int));
            read(req_pipe,&offsetSect,sizeof(unsigned int));
            read(req_pipe,&noBSect,sizeof(unsigned int));
            write(resp_pipe,"READ_FROM_FILE_SECTION#",strlen("READ_FROM_FILE_SECTION#"));
            hdsz= *(unsigned short*)(data + sizedata-4-2);
            noSections=data[sizedata-hdsz+2];
            if(sectionNo>1 && sectionNo<=noSections)
            {
                offsetSection=*(unsigned int*)(data+ sizedata-hdsz+3+(sectionNo-1)*17+9);
                for(int i=0; i<noBSect; i++)
                {
                    rddatasect[i]=data[offsetSection+offsetSect+i];
                }
                rddatasect[noBSect]='\0';
            }
            if(offsetSection+offsetSect>0 && offsetSection+offsetSect<=sizedata && offsetSection+offsetSect+noBytes<=sizedata)
            {
                memcpy(sharedZone,&rddatasect,noBSect);

                write(resp_pipe,"SUCCESS#",strlen("SUCCESS#"));
            }
            else
            {
                write(resp_pipe,"ERROR#",strlen("ERROR#"));
            }


        }

        if(strcmp(buffer_req,"READ_FROM_LOGICAL_SPACE_OFFSET")==0)
        {
            write(resp_pipe,"READ_FROM_LOGICAL_SPACE_OFFSET#",strlen("READ_FROM_LOGICAL_SPACE_OFFSET#"));
            break;
        }

        if(strcmp(buffer_req,"EXIT")==0)
        {
            close(req_pipe);
            close(resp_pipe);
            unlink(RESP_PIPE_NAME);
            munmap(sharedZone, dimShm);
            close(shmFd);
            shm_unlink("/cghIRY");
            munmap(data,sizedata);
            close(fd);
            break;
        }
    }

    return 0;
}
