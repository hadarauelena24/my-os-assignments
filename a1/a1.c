#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

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

void listSF(const char *path)
{
	int fd=-1;
	int ok=1;
	char magic[5];
	int header_sz=0;
	int version=0;
	int nsections=0;
	char** name_sect;
	int* type_sect;
	int* size_sect;
	//off_t* off_sect;
	char name[9];
	int type=0;
	int sizesec=0;
	//off_t offsec=0;
	fd=open(path,O_RDONLY);
	if(fd==-1){
		perror("Could not open input file");
		return;
	}
	lseek(fd,-4,SEEK_END);
	if(read(fd,magic,4)!=4){
		perror("reading magic error!");
		return;
	}
	else{
		magic[5]='\0';
		if(strcmp(magic,"E7pu")!=0){
			ok=0;
		}
		//else printf("magic= %s\n",magic);
	}
	if(ok==1){
	lseek(fd,-6,SEEK_END);
	if(read(fd,&header_sz,2)!=2){
		perror("reading headers size error!");
		return;
	}
	
	/*else{
		header_sz=(int)headerss[0];
		printf("header size= %d \n",header_sz);
	}*/
	lseek(fd,-header_sz,SEEK_END);
	if(read(fd,&version,2)!=2){
		perror("reading versions error!");
		return;
	}
	else{
		//version=(int)versionss[0];
		//printf("version=%d\n",version);
		if(!(version>=82 && version<=146)){
			ok=-1;
		}
	}
	}
	if(ok==1){
	if(read(fd,&nsections,1)!=1){
		perror("reading nr sections error!");
		return;
	}
	else{
		//nsections=(int)nsectss[0];
		//printf("nr_sections=%d\n",nsections);
		
		if(!(nsections>=2 && nsections<=15)){
			ok=-2;
			
		}}}
	if(ok==1){
			name_sect=(char**)calloc(nsections,sizeof(char*));
			for(int secti=0;secti<nsections;secti++)
				name_sect[secti]=(char*)calloc(9,sizeof(char));
			type_sect=(int*)calloc(nsections,sizeof(int));
			size_sect=(int*)calloc(nsections,sizeof(int));
			//off_sect=(off_t*)calloc(nsections,sizeof(off_t));
			for(int h=0;h<nsections;h++){
				if(read(fd,name,8)!=8)
					return;
				else
					name[9]='\0';
				if(read(fd,&type,1)!=1)
					return;
				//if(read(fd,&offsec,4)!=4)
				//	return;
				lseek(fd,4,SEEK_CUR);
				if(read(fd,&sizesec,4)!=4)
					return;
				if(type!=10 && type!=70 && type!=30 && type!=66)
					ok=-3;

				strcpy(name_sect[h],name);
				type_sect[h]=type;
				size_sect[h]=sizesec;
				//off_sect=offsec;

			}
			
		}
	
	

	if(ok==1){
		printf("SUCCESS\n");
		printf("version=%d\n",version);
		printf("nr_sections=%d\n",nsections);
		for(int h=0;h<nsections;h++){
			printf("section%d: %s %d %d\n",h+1,name_sect[h],type_sect[h],size_sect[h]);
			}
		free(type_sect);
		free(size_sect);
		for(int secti=0;secti<nsections;secti++)
			free(name_sect[secti]);
		free(name_sect);
	}
	else if(ok==0)
		printf("ERROR\nwrong magic\n");
	else if(ok==-1)
		printf("ERROR\nwrong version\n");
	else if(ok==-2)
		printf("ERROR\nwrong sect_nr\n");
	else if(ok==-3){
			printf("ERROR\nwrong sect_types\n");
			free(type_sect);
			free(size_sect);
			for(int secti=0;secti<nsections;secti++)
				free(name_sect[secti]);
			free(name_sect);
		}
	
	close(fd);
}



int main(int argc, char **argv){
    if(argc >= 2){
        if(strcmp(argv[1], "variant") == 0){
            printf("13706\n");
        }
    }

    if(argc >= 3){
    	int i=1;
    	int rec=0,filt_size=0,filt_name=0,listare=0,f_path=0,i_path=0;//flags pt listare
    	int parsare=0;//flags pt parsare
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
    		if(strcmp(argv[i], "parse") ==0){
    			parsare=1;
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
    	if(f_path!=0){
    		char sep[]="=";
		char * p = strtok(argv[f_path] , sep);
		while(p != NULL && i_path<1)
		{
			i_path=i_path+1;
    			//printf("%s \n",p);
    			p = strtok(NULL , sep);
		}
		fullPath=p;
    		if( listare==1){
    		//printf("%s \n",argv[f_path]);
    		printf("SUCCESS \n");
    		
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
		
    	}//end listare
    	else if(parsare==1){
    		listSF(fullPath);
    		
    	}//end parsare
    	
    	}//end f_path!=0

    }//end argc>=3
    
    return 0;
}
