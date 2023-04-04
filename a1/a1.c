#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

void listRec(const char *path,int rec,int filt_size,int filt_name,off_t fsize,char* fstring)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char fullPath[512];
    struct stat statbuf;

    dir = opendir(path);
    if(dir == NULL) {
        perror("Could not open directory");
        return;
    }
    
    while((entry = readdir(dir)) != NULL) {
    
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
        
            snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
            
            if(lstat(fullPath, &statbuf) == 0) {
            
            	if(filt_name==0 && filt_size==0)
                	printf("%s\n", fullPath);
                else if(filt_name!=0){
                
                	if(strstr(entry->d_name,fstring)){
                	
                		char* ss=strstr(entry->d_name,fstring);
                		if(strcmp(ss,fstring)==0)
                			printf("%s\n", fullPath);
                		}
                	}
                	
                	else if(filt_size!=0){
                	
                		if(S_ISREG(statbuf.st_mode)){
                			if(statbuf.st_size<=fsize)
                				printf("%s\n", fullPath);
                		}
                	}
                	if(S_ISDIR(statbuf.st_mode)) {
                	
                		if(rec==1){
                    		listRec(fullPath,rec,filt_size,filt_name,fsize,fstring);
                		}
                	}
            }
        }
    }
    closedir(dir);
}
int main(int argc, char **argv){
    if(argc >= 2){
        if(strcmp(argv[1], "variant") == 0){
            printf("13706\n");
        }
    }

    if(argc >= 3){
    	int i=1;
    	int rec=0,filt_size=0,filt_name=0,listare=0,f_path=0,i_path=0;
    	char* fullPath;
    	f_path=0;
   	rec=0;
   	filt_size=0;
   	filt_name=0;
   	listare=0;
    	for(i=1;i<argc;i++){
    		if(strcmp(argv[i], "list") ==0){
    			listare=1;
    		}
    		if(strcmp(argv[i],"recursive")==0){
    			rec=1;
    		}
    		if(strstr(argv[i],"size_smaller=")!=NULL){
    			filt_size=i;
    		}
    		if(strstr(argv[i],"name_ends_with=")!=NULL){
    			filt_name=i;
    		}
    		if(strstr(argv[i],"path=")!=NULL){
    			f_path=i;
    		}	
    	}
    	//printf("%d %d %d %d \n",listare,rec,filt_size,filt_name);
    	if(f_path!=0 && listare==1){
    		//printf("%s \n",argv[f_path]);
    		printf("SUCCESS \n");
    		char sep[]="=";
		char * p = strtok(argv[f_path] , sep);
		while(p != NULL && i_path<1)
		{
			i_path=i_path+1;
    			//printf("%s \n",p);
    			p = strtok(NULL , sep);
		}
		fullPath=p;
		//printf("full path : %s \n",fullPath);
		i_path=0;
		off_t dim_reg=0;
		if(filt_size!=0){
			char* s=argv[filt_size];
			int ns=strlen(s);
			char* t=(char*)malloc(ns*sizeof(char));
			int nt=0;
			//printf("%s %s %d\n",s,t,ns);
			int ks=0;
			for(ks=0;ks<ns;ks++){
				
				if(strchr("0123456789",s[ks])){

					t[nt]=s[ks];
					nt=nt+1;
				}
			}
			t[nt]='\0';

			dim_reg=atoi(t);
			free(t);
			//printf("%s %s %ld\n",s,t,dim_reg);

			
		}
		i_path=0;
		char* end_name;
		if(filt_name!=0){
			
			char * d = strtok(argv[filt_name] , sep);
			while(d != NULL && i_path<1)
			{
				i_path=i_path+1;
    				//printf("%s \n",p);
    				d = strtok(NULL , sep);
			}
			end_name=d;
			//printf("%s\n",end_name);
		}
		listRec(fullPath,rec,filt_size,filt_name,dim_reg,end_name);
		
    	}

    }
    
    return 0;
}
