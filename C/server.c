#include "cs537.h"
#include "request.h"

#define DEBUG 0

pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

buf_entry* buf;

int port, n_thr, n_buf, n_alg;

int num_items = 0;
int wr_ptr = 0, rd_ptr = 0;

// 
// server.c: A very, very simple web server
//
// To run:
//  server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

// CS537: Parse the new arguments too
void getargs(int *port,int* n_thr,int* n_buf,int* n_alg,int argc,char *argv[])
{
    if (argc != 5) {
	fprintf(stderr, "Usage: %s <port><threads><buffers><schedalg>\n", argv[0]);
	exit(1);
    }
    *port = atoi(argv[1]);
    *n_thr = atoi(argv[2]);
    *n_buf = atoi(argv[3]);
    *n_alg = atoi(argv[4]);

}

void* producer()
{
    int listenfd, connfd, clientlen;
    struct sockaddr_in clientaddr;

    listenfd = Open_listenfd(port);

    while(1)
    {
	clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);

        if(DEBUG) printf("Producer running.\n");
	pthread_mutex_lock(&m);
	if(DEBUG) printf("Producer entered critical region.\n");	
	while(num_items==n_buf)
	{
	    if(DEBUG) printf("Buffer full. Producer wait.\n");
	    pthread_cond_wait(&empty,&m);
	}
	buf[wr_ptr].buf_fd = connfd;
	wr_ptr++;
	if(wr_ptr>=n_buf)
	    wr_ptr=0;
	num_items++;
	pthread_cond_signal(&full);
	pthread_mutex_unlock(&m);
	if(DEBUG) printf("Producer exited critical region.\n");
	//Close(connfd);
	
    }
}

void* consumer()
{
    int fd_tmp=0;
    while(1)
    {
	if(DEBUG) printf("Consumer running.\n");
	pthread_mutex_lock(&m);
	if(DEBUG) printf("Consumer entered critical region.\n");

	while(num_items==0)
	{
	    if(DEBUG) printf("Buffer empty. Consumer wait.\n");
	    pthread_cond_wait(&full,&m);
	}
	fd_tmp = buf[rd_ptr].buf_fd;
	rd_ptr++;
	if(rd_ptr>=n_buf)
	    rd_ptr=0;
	num_items--;
	pthread_cond_signal(&empty);
	pthread_mutex_unlock(&m);
	if(DEBUG) printf("Producer exited critical region.\n");
        requestHandle(fd_tmp);
        Close(fd_tmp);
    }
}


int main(int argc, char *argv[])
{
    int i=0,rc=-1;
    //pthread_mutex_init(&m, NULL);
    

    getargs(&port, &n_thr, &n_buf, &n_alg, argc, argv);

    if(DEBUG) 
    {
	printf("# of thr: %d, # of buf: %d, # of alg: %d.\n",n_thr,n_buf,n_alg);
    }
    // 
    // CS537: Create some threads...
    //

    //struct buff_entry buff[n_buf];
    
    pthread_t pid, cid[n_thr];
    buf =(buf_entry*) malloc(n_buf*sizeof(buf_entry));
    if(DEBUG) printf("Size of buff entry: %ld.\n",sizeof(buf_entry));

    rc=pthread_create(&pid, NULL, producer, NULL);//create producer
    if(DEBUG)
        {
            if(rc==0) 
                printf("Producer %ld created successfully.\n",pid);
            else
                printf("Failed to create producer %ld.\n",pid);
        }

    for(i=0;i<n_thr;i++)//create working threads - consumers
    {
	rc=pthread_create(&cid[i], NULL, consumer, NULL);
	if(DEBUG)
	{
	    if(rc==0) 
		printf("Consumer %ld created successfully.\n",cid[i]);
	    else
		printf("Failed to create consumer %ld.\n",cid[i]);
	}

    }
    

    while (1) {

	// 
	// CS537: In general, don't handle the request in the main thread.
	// Save the relevant info in a buffer and have one of the worker threads 
	// do the work. However, for SFF, you may have to do a little work
	// here (e.g., a stat() on the filename) ...
	// 
	
	//requestHandle(connfd);
	pthread_join(pid,NULL);	
    }

}


    


 
