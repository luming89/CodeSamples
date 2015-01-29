#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#define DEBUG
/* Author: Luming Zhang
   Commands implemented: exit, cd, pwd, ls, Python
*/
void output_redirect(char *outfile)
{
    int close_rc;
    int fd;	
    close_rc=close(STDOUT_FILENO);
    if(close_rc<0)
        printf("Close STDOUT failed.\n");
    fd=open(outfile,O_RDWR|O_TRUNC|O_CREAT,S_IRWXU);
    if(fd<0)
    {
        printf("Open output file failed.\n");
    }

}

int
getcmd(char *buf, int nbuf, int batch, FILE* fd)
{
    if (batch==0) 
	printf("mysh> ");
    memset(buf, 0, nbuf);
    if (batch)
	fgets(buf,nbuf,fd);
    else
	fgets(buf,nbuf,stdin);
    if(buf[0] == 0) // EOF
	return -1;
    return 0;
}


int main(int argc, char* argv[])
{
    static char buf[512];
    static char cwd[512];
    char *args[512];
    int i=0;
    int j=0;
    int length_args0=0;
    int batchmode=0;//DEBUGING 
    FILE* batchin;
    int background=0;
    int redirection=0;
    char error_message[30] = "An error has occurred\n";

    #ifdef DEBUG
	printf("argc is %d\n",argc);
	printf("argv[0][1][2] is %s,%s,%s\n",argv[0],argv[1],argv[2]); 
    #endif
    if(argc==2)
    
    {	
	batchmode=1;
	batchin=fopen(argv[1],"r");
    }

    #ifdef DEBUG
	if(batchmode) printf("Batchmode activiated.\n");
    #endif

    while(getcmd(buf,sizeof(buf),batchmode,batchin)>=0)
	{
	    buf[strlen(buf)-1] = 0;// chop \n
	    #ifdef DEBUG
		printf("%s\n",buf);
		printf("length of buf %d\n",(int)strlen(buf));
	    #endif
	    i=0;
	    if((args[i]=strtok(buf," "))!=NULL)
	    {
			#ifdef DEBUG
			    printf("args[%d] is %s\n",i,args[i]);
			#endif
			i++;
			while((args[i]=strtok(NULL," "))!=NULL)
			{
			    #ifdef DEBUG
			        printf("args[%d] is %s\n",i,args[i]);
			    #endif
			    i++;
			}
	    }
	    else
			continue;

	    #ifdef DEBUG
	    	for(i=0;i<5;i++)
				printf("args[%d] is %s\n",i,args[i]);
	    #endif
	
	    //Background judgement
	    i=0;
	    while(args[i]!=NULL)
	    {
			if(strcmp(args[i],"&")==0)
			{
			    background=1;
		            j=i;//Remove & from args
			    while(args[j]!=NULL)
			    {
				args[j]=args[j+1];
				j++;
			    }	
			}
			i++;
	    }

	    //Implement build-in command: exit
	    if(strcmp(args[0],"exit")==0) 
	    {
		#ifdef DEBUG
		    printf("You have reached exit.\n");
		#endif
			exit(0);
	    }
  		
	    //Implement build-in command: cd
	    if(strcmp(args[0],"cd")==0)
	    {
		#ifdef DEBUG
		    printf("HOME IS %s\n",getenv("HOME"));
		#endif
			if(args[1]==NULL)
			{
		        chdir(getenv("HOME"));	
			}
			else if(chdir(args[1]) < 0)
	        {
			    printf("cannot cd %s\n", buf+3);
			}
	    }

	    //Implement build-in command: pwd
	    if(strcmp(args[0],"pwd")==0)
        {
            //printf("You have reached pwd.\n");
            printf("%s\n",getcwd(cwd,sizeof(cwd)));
        }
	    
	    //Implement Python
	    length_args0=strlen(args[0]);
	    #ifdef DEBUG
			printf("Args0 is %s, length is %d\n",args[0],length_args0);
	    #endif	
	    if((args[0][length_args0-3]=='.')&&(args[0][length_args0-2]=='p')&&(args[0][length_args0-1]=='y'))
	    {
			execvp("/usr/bin/python", args);
	    }

	    //Implement ls
	    if(strcmp(args[0],"ls")==0)
	    {
			#ifdef DEBUG
                for(i=0;i<5;i++)
                    printf("Before fork, args[%d] is %s\n",i,args[i]);
            #endif



			int pid;
			pid = fork();
			if (pid>0)
			{
			    //parent process code here
			    if(background==0)
			    {
				#ifdef DEBUG
			            printf("Parent will wait.\n");
			        #endif
			        wait(NULL);
			    }
			}
			else if (pid==0)
			{
			    //child process code here
			    #ifdef DEBUG
				printf("Child will execute.\n");
			    #endif

			    //redirection
			    i=0;
			    j=0;
			    while(args[i]!=NULL)
			    {
					if(strcmp(args[i],">")==0)
		            {
	            	    #ifdef DEBUG
	                		printf("Detected >\n");
	            	    #endif
				   		if(redirection==0)
	            	    {	

	                   		if(args[i+1]==NULL)
	                    	{
	                    	    printf(error_message);
	                    	    #ifdef DEBUG
	                        		printf("Reached args[i+1]==NULL\n");
	                    	    #endif
							}
		                    else
	        	            {
	            	            redirection=1;
	                    	    #ifdef DEBUG
	                            	printf("Reached redirection=1;\n");
	                            	printf("args[i+1] is %s\n",args[i+1]);
	                    	    #endif
					    		output_redirect(args[i+1]);				    

					    		for(j=i;j<10;j++)//delete > output arguments
		                            args[j]=args[j+2];
		
	    	                }
	            	    }
	            	    else
	            	    {
							#ifdef DEBUG
							    printf("Two > detected.\n");
							#else
					    		printf(error_message);
							#endif
	            	    }
	            	}	
		        	i++;
			    }
			    //End of redirection
		 		
			    //if((background)&&(redirection==0))
				//close(STDOUT_FILENO);
			
			    execvp(args[0], args);
			    printf("If you reached here, something went wrong");
			}
			else
			{
			    write(STDERR_FILENO, error_message, strlen(error_message));
			}
	    }
    }
    return 0;		
}
