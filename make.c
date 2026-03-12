/* charan */
/* working code */
/* simple make command */
/* makefile need */
/* \tcc abc.o xyz.o */
#include<stdio.h>
#include<string.h>
#include<stdlib.h>	
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>

#define VALID 	1
#define INVALID 0
#define EXIT_ALL_UPDATED 3
#define SOURCE_INVALID 4

int flag=1;
int line=1;
int update=0;
int cnt=1;

char o[50];
char c[50];
char *compile=0;
char **m_mr=0;

char * readline(FILE *);
int main_receipe(char *);
int mini_receipe(char *);
int file_check(char *,char *);


int main(){

int f=0;
char *mr=0;

	FILE *fp=fopen("makefile","r");
	if(fp==0){
		perror("makefile not exist/not opening-\tfopen");
		return 1;
	}

	//file opened
	while(1){
		mr=readline(fp);
		if(mr){
			//printf("****** %s ******\n",mr);
			if(flag==1){
				f=main_receipe(mr);
				free(mr);
				if(f==EXIT_ALL_UPDATED){ //exe & files all exist & updated
					return 0;
				}
			}
			else if(flag==2){
				f=mini_receipe(mr);
			}
		}
		else{
			break;
		}
	}

	if(m_mr){
		for(int i=2;i<(cnt-1);i++){
			if(m_mr[i]){
				strcpy(c,"cc -c ");
				strcat(c,m_mr[i]);
				c[strlen(c)-1]='c';
				printf("--------%s--------\n",c);
				system(c);
				free(m_mr[i]);
				m_mr[i]=0;
			}
		}
	}

	if(compile){
		system(compile);
		printf("--------%s-------\n",compile+1);
	}

	return 0;
}

int main_receipe(char *p){
	
	struct stat s;
	char *r=0;
	char *target=0;
	char d[50];
	int ret=0;
	int j=0;
	int i=0;
	int f=0;


	if(line==1){	//main_receipe line1

		r=strchr(p,':');
		if(r==0){ //not related line1 / invalid line
			return INVALID;
		}

		r=r+1; //if valid
		while(1){

			while(r[i]==' '){ //ignore white spaces
				i++;
			}

			j=0;
			while(r[i]!=' ' && r[i]){ //read word by word
				d[j++]=r[i];
				i++;
			}
			d[j++]=0;

			f=file_check(d,0); //check file update/outdate
			if(f==INVALID){ //outdated/not exist
					//UPDATE IN 2D M/R
				m_mr=realloc(m_mr,sizeof(char*)*cnt);
				m_mr[cnt-1]=malloc(strlen(d));
				strcpy(m_mr[cnt-1],d);
				cnt++;
			}
			else if(f==SOURCE_INVALID){
				printf("%s,.c not exist\n",d);
				exit(0);
			}

			if(r[i]==0){
				break;
			}
		}

		line=2;

		if(m_mr==0){

			target=p;
			*(r-1)=0;
			
			//remove extra space at end
			int k=0;
			while(target[k]!=' ' && target[k]){
				k++;
			}
			target[k]='\0';

			ret=stat(target,&s);
			if(ret==0){
				printf("everything updated...\n");
				return EXIT_ALL_UPDATED;
			}
		}
		return VALID;
	}

	if(line==2){

		//check line2 / invalid line
		r=strstr(p,"\tcc");
		if(r==0){ //not related line2/invalid
			return INVALID;
		}

		//if valid
		compile=p;

		if(m_mr==0){//if all files upadted only exe not exist
			printf("----------%s---------\n",compile+1);
			system(compile);
			return EXIT_ALL_UPDATED;
		}

		compile=malloc(strlen(p)+1);
		strcpy(compile,p);
		flag=2;
		line=1;

		return VALID;
	}
}

int mini_receipe(char *p)
{
  int i=0;
  int status=0;
  char *r=0;
  int ret=0;

	//check mini_receipe line / invalid
	if(line==1){	
		//format file.o:file.c
		r=strchr(p,':');
		if(r==0){
			free(p);
			return INVALID;
		}

		//if mini line1 
		//check update/outdate
		*r=0;
		

		strcpy(o,p);  
		strcpy(c,r+1);
		
		//ignore empty spaces in o,c
	
		for(i=0;o[i]==' ' || o[i]=='\t';i++);
		if(i!=0){
			memmove(o,&o[i],strlen(o+i)+1);
		}
		r=strchr(o,'.');
		*(r+2)=0;
		
		for(i=0;c[i]==' ' || c[i]=='\t';i++);
                if(i!=0){
			memmove(c,&c[i],strlen(c+i)+1);
                }
                r=strchr(c,'.');
                *(r+2)=0;



		ret=file_check(o,c);
		if(ret==INVALID){ //outdated, read nxt sequence mini_receipe line2
			update=0;
		}
		else if(ret==SOURCE_INVALID){
			printf("%s not exist\n",c);
			exit(0);
		}
		else if(ret==VALID){//updated ,ignore nxt line2
			update=1;
		}

		line=2;
		free(p);
	}

	else if(line==2){

		r=strstr(p,"\tcc");
		if(r==0){  //if not line2
			free(p);
			return INVALID;
		}

		//if line2
		if(update == 0) {//compile line2
			printf("--------%s-------\n",p+1);
			system(p);
			for(int i=0;i<cnt;i++){ //if we store in m_mr remove entry in that
				if(m_mr[i]){
					status=strcmp(o,m_mr[i]);
					if(status==0){
						free(m_mr[i]);
						m_mr[i]=0;
					}
				}
			}
		}
		//else ignore

		line=1;
		free(p);
	}
}

int file_check(char *o,char *c){

	struct stat obj,sou;
	unsigned long int obj_t=0,sou_t=0;
	int reto=0,retc=0;
	char *r=0;
	char source[50];

	if(o){ // 2file / 1file
		reto=stat(o,&obj);
		if(reto==-1){
			//perror("stat");
			//printf("-%s not exist-\n",o);
			return INVALID;
		}

		obj_t=obj.st_mtime;

	}
	if(c==0){
		strcpy(source,o);
		r=strchr(source,'.');
		*(r+1)='c';
		c=source;
	}

	if(c){ // 2file
		retc=stat(c,&sou);
		if(retc==-1){
			return SOURCE_INVALID;
		}

		sou_t=sou.st_mtime;
	}

	if(sou_t < obj_t){
		return VALID;
	}
	else{
		return INVALID;
	}

}

char * readline(FILE *fp){
	char ch=0;
	char *p=0;
	int cnt=0;
	
	while(ch=fgetc(fp)){
		if(ch==EOF && cnt==0)
			break;
		cnt++;
		p=realloc(p,cnt);
		if(ch==EOF || ch=='\n'){
			p[cnt-1]='\0';
			break;
		}
		else{
			p[cnt-1]=ch;
		}
	}
	return p;
}
