// source code MPI_TEST.C program

#include<mpi.h>
#include<stdlib.h>
#include<stdio.h>

#define Wtime MPI_Wtime
#define MEGABYTE (1024*1024)
#define BUFFER_SIZE (16*MEGABYTE)
#define TAG1 17
#define MASTER_RANK 0
#define MAX_REQUESTS 16
#define MILLION 1000000.0

static int NTIMES = 1000;
static int NTIMES_PREC = 10000;
static int REPEAT = 1;

#define NTIMES_TIMING NTIMES_PREC
#define NTIMES_BARRIER NTIMES_PREC
#define NTIMES_LATENCY NTIMES_PREC

#define TEST_SENDRECV 0

#define MASTER if(i_am_the_master) 

int nproc,myid;
int i_am_the_master = 0;

static int msglen_min = 1;
static int msglen_max = 1000;
static int msglen_multiplier = 10;
static int msglen_step = 0;

int *buf,*obuf;

void Test_MPI_Routines(MPI_Comm comm,int len);
int checktick();

int main(int argc,char *argv[]) {

	int i,k;
	int bufsize = BUFFER_SIZE;
	int msglen;
	double t_start,t_end;
	char processor_name[128];
	int namelen = 128;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&nproc);
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    i_am_the_master = (myid == MASTER_RANK);

	t_start = Wtime();
	MASTER printf("MPItest/C 2.0: running %d processes...\n",nproc);
	
	if(nproc < 2) {
		MASTER printf("MPItest expects at least 2 processes\n");
		MPI_Finalize();
		exit(0);
	}

	
    MPI_Get_processor_name(processor_name,&namelen);
    fprintf(stdout,"Process %d of %d on %s\n",myid, nproc, processor_name);
    MPI_Barrier(MPI_COMM_WORLD);
	fflush(stdout);


	MASTER /* read command line parameters */	
	for(i = 1; i < argc; i ++)
		switch(argv[i][0]) {
			case 'm': msglen_min = atoi(argv[i]+1); break;
			case 'M': msglen_max = atoi(argv[i]+1); break;
			case 'K': msglen_multiplier = atoi(argv[i]+1); msglen_step = 0; break;
			case 'T': NTIMES = atoi(argv[i]+1); break;			
			case 't': NTIMES_PREC = atoi(argv[i]+1); break;			
			case 'R': REPEAT = atoi(argv[i]+1); break;			
			case 'B': bufsize = (atoi(argv[i]+1)*MEGABYTE)/sizeof(int); break;
			default: fprintf(stderr,"WARNING: unrecognized option: %s\n",argv[i]);
			break;
		}

	MPI_Bcast(&NTIMES,1,MPI_INT,MASTER_RANK,MPI_COMM_WORLD);
	MPI_Bcast(&NTIMES_PREC,1,MPI_INT,MASTER_RANK,MPI_COMM_WORLD);
	MPI_Bcast(&REPEAT,1,MPI_INT,MASTER_RANK,MPI_COMM_WORLD);
	MPI_Bcast(&msglen_min,1,MPI_INT,MASTER_RANK,MPI_COMM_WORLD);
	MPI_Bcast(&msglen_max,1,MPI_INT,MASTER_RANK,MPI_COMM_WORLD);
	MPI_Bcast(&msglen_multiplier,1,MPI_INT,MASTER_RANK,MPI_COMM_WORLD);
	MPI_Bcast(&bufsize,1,MPI_INT,MASTER_RANK,MPI_COMM_WORLD);

	if(bufsize < msglen_max * nproc) {
		MASTER printf("Too small buffer size - %d. At least %d words required\n",bufsize,msglen_max * nproc);
		MPI_Finalize();
		exit(0);
	}
	buf  = (int*) malloc(sizeof(int)*bufsize);
	obuf = (int*) malloc(sizeof(int)*bufsize);
	
    if(buf == NULL || obuf == NULL) 
    {
		MASTER printf("Too small buffer size. At least %d words required\n",msglen_max * nproc);
		MPI_Finalize();
		exit(0);
	}

	MASTER printf("Testing basic MPI routines.\n");
	MASTER printf("T = %d, t = %d\n",NTIMES,NTIMES_PREC);

	for(msglen = msglen_min; msglen <= msglen_max; msglen *= msglen_multiplier)
		for(k = 0; k < REPEAT; k ++)
		{
			MASTER printf("\n--- Size %d, Iteration %d ---\n",msglen,k);
			Test_MPI_Routines(MPI_COMM_WORLD,msglen);
		}

	t_end = Wtime();
	MASTER printf("MPItest/C complete in %g sec.\n",t_end-t_start);
	MPI_Finalize();
	return 0;
}	

void Test_MPI_Routines(MPI_Comm comm,int msglen) {
	int nproc,myid,i,tick;
	int i_am_the_master = 0;
	double t, t1, t2, tt, dt, lat, tb;
	MPI_Status status, statuses[MAX_REQUESTS];
	MPI_Request request, requests[MAX_REQUESTS];

	MPI_Comm_size(MPI_COMM_WORLD,&nproc);
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);

    i_am_the_master = (myid == MASTER_RANK);

	MPI_Barrier(comm);

	/* Initialization of buffer data */

	for(i = 0; i < BUFFER_SIZE; i ++)
         buf[i] = myid;

	MASTER printf("** Timing for MPI operations in microseconds ***\n");
	fflush(stdout);
/* ----- Measuring time for a MPI_Wtime() call  ------ */

    dt = 0;
    t1 = Wtime();
	MASTER for(i = 0; i < NTIMES_TIMING; i ++) 
	{
         t = Wtime();
	}
    t2 = Wtime();
    dt = t2 - t1;
	dt /= (double) NTIMES_TIMING;
	t = dt;
	t *= MILLION;
    
    MASTER tick = checktick();
	MASTER printf("%g\t\tTiming; clock tick: %d\n",t,tick);
    
	fflush(stdout);

       

/* ----------- Testing barrier synchronization ---------- */

fflush(stdout);

	tb = 0;
    t1 = Wtime();
    for(i = 0; i < NTIMES_BARRIER; i ++) 
	{
		 MPI_Barrier(comm);
#if 0
	printf("%d:%d ",myid,i);
	fflush(stdout);
#endif
	}
    t2 = Wtime();
    tb = t2 - t1;
    tb /= (double) NTIMES_BARRIER;
	t = tb;
	t *= MILLION;

	MASTER printf("%g\t\tBarrier\n",t);
	fflush(stdout);
	
/* ----------- Testing reduction via MPI_Allreduce ---------- */

	t = 0;
    MPI_Barrier(comm);
	t1 = Wtime();
	for(i = 0; i < NTIMES; i ++) 
	{
		MPI_Allreduce(buf,obuf,msglen,MPI_INT,MPI_SUM,comm);
	}
	t2 = Wtime();
	t = (t2 - t1);
	t /= (double) NTIMES;
	t *= MILLION;

	MASTER printf("%g\t\tGlobal sum of %d numbers (Allreduce)\n",t,msglen);
	fflush(stdout);

/* ----------- Testing reduction via MPI_Reduce ---------- */

	t = 0;
    MPI_Barrier(comm);
	t1 = Wtime();
	for(i = 0; i < NTIMES; i ++) 
	{
		MPI_Reduce(buf,obuf,msglen,MPI_INT,MPI_SUM,MASTER_RANK,comm);
	}
	t2 = Wtime();
	t = (t2 - t1);
	t /= (double) NTIMES;
	t *= MILLION;

	MASTER printf("%g\t\tGlobal sum of %d numbers (Reduce)\n",t,msglen);
	fflush(stdout);

/* ----------- Testing broadcast routine ---------- */

	t = 0;
    MPI_Barrier(comm);
	t1 = Wtime();
    for(i = 0; i < NTIMES; i ++) 
	{
		MPI_Bcast(buf,msglen,MPI_INT,MASTER_RANK,comm);
	}
	t2 = Wtime();
	t = (t2 - t1);
	t /= (double) NTIMES;
	t *= MILLION;

	MASTER printf("%g\t\tBroadcast of %d numbers\n",t,msglen);
	fflush(stdout);
/* ----------- Testing gather routine ---------- */

	t = 0;
	MPI_Barrier(comm);
	t1 = Wtime();
    for(i = 0; i < NTIMES; i ++) 
	{
		MPI_Gather(buf,msglen,MPI_INT,obuf,msglen,MPI_INT,MASTER_RANK,comm);
	}
	t2 = Wtime();
	t = (t2 - t1);
	t /= (double) NTIMES;
	t *= MILLION;

	MASTER printf("%g\t\tGathering of %d numbers\n",t,msglen);
	fflush(stdout);

/* ----------- Testing Allgather routine ---------- */

	t = 0;
	MPI_Barrier(comm);
	t1 = Wtime();
    for(i = 0; i < NTIMES; i ++) 
	{
		MPI_Allgather(buf,msglen,MPI_INT,obuf,msglen,MPI_INT,comm);
	}
	t2 = Wtime();
	t = (t2 - t1);
	t /= (double) NTIMES;
	t *= MILLION;

	MASTER printf("%g\t\tAll-gather of %d numbers\n",t,msglen);
	fflush(stdout);

/* ----------- Testing gather routine ---------- */

	t = 0;
	MPI_Barrier(comm);
	t1 = Wtime();
    for(i = 0; i < NTIMES; i ++) 
	{
		MPI_Alltoall(buf,msglen,MPI_INT,obuf,msglen,MPI_INT,comm);
	}
	t2 = Wtime();
	t = (t2 - t1);
	t /= (double) NTIMES;
	t *= MILLION;

	MASTER printf("%g\t\tAll-to-All send of %d numbers\n",t,msglen);
	fflush(stdout);

/* ----------- Testing non-blocking send with wait ---------- */
#if 1
	t = 0;
    MPI_Barrier(comm);
	if(i_am_the_master)
	{
		t1 = Wtime();
	    for(i = 0; i < NTIMES; i ++) 
		{
			MPI_Isend(buf,msglen,MPI_INT,1,TAG1,comm,&request);
			MPI_Wait(&request,&status);
		}
		t2 = Wtime();
		t = (t2 - t1);
	}
	else if(myid == 1)
	    for(i = 0; i < NTIMES; i ++) 
		{
			MPI_Irecv(obuf,msglen,MPI_INT,MASTER_RANK,TAG1,comm,&request);
			MPI_Wait(&request,&status);
		}
	t /= (double) NTIMES;
	t *= MILLION;

	MASTER printf("%g\t\tNon-blocking send of %d numbers with wait\n",t,msglen);
	fflush(stdout);
#endif

/* ----------- Testing blocking vector send ---------- */

	t = 0;
	 MPI_Barrier(comm);
	if(i_am_the_master)
	{
		t1 = Wtime();
	    for(i = 0; i < NTIMES; i ++) 
		{
			MPI_Send(buf,msglen,MPI_INT,1,TAG1,comm);
		}
		t2 = Wtime();
		t = (t2 - t1);
	}
	else if(myid == 1)
	    for(i = 0; i < NTIMES; i ++) 
			MPI_Recv(obuf,msglen,MPI_INT,MASTER_RANK,TAG1,comm,&status);

	t /= (double) NTIMES;
	t *= MILLION;

	MASTER printf("%g\t\tBlocking vector send of %d numbers\n",t,msglen);
	fflush(stdout);

/* ----------- Testing simultaneous send & receive ---------- */

	t = 0;
    MPI_Barrier(comm);
	if(i_am_the_master)
	{
		t1 = Wtime();
	    for(i = 0; i < NTIMES; i ++) 
		{
            		MPI_Sendrecv(buf,msglen,MPI_INT,1,TAG1,obuf,msglen,MPI_INT,1,TAG1,comm,statuses);
		}
		t2 = Wtime();
		t = (t2 - t1);
	}
	else if(myid == 1) 
	    for(i = 0; i < NTIMES; i ++) 
	            MPI_Sendrecv(buf,msglen,MPI_INT,MASTER_RANK,TAG1,obuf,msglen,MPI_INT,MASTER_RANK,TAG1,comm,statuses);

	t /= (double) NTIMES;
	t *= MILLION;

	MASTER printf("%g\t\tSendRecv, %d numbers\n",t,msglen);
	fflush(stdout);

/* ----------- Testing sequential send & receive ---------- */
	t = 0;
    MPI_Barrier(comm);
	if(i_am_the_master)
	{
		t1 = Wtime();
	    for(i = 0; i < NTIMES; i ++) 
		{
            MPI_Send(buf,msglen,MPI_INT,1,TAG1,comm);
            MPI_Recv(obuf,msglen,MPI_INT, 1,TAG1,comm,&status);
		}
		t2 = Wtime();
		t = (t2 - t1);
	}
	else if(myid == 1) 
	    for(i = 0; i < NTIMES; i ++) 
		{
            MPI_Recv(obuf,msglen,MPI_INT,MASTER_RANK,TAG1,comm,&status);
            MPI_Send(buf,msglen,MPI_INT,MASTER_RANK,TAG1,comm);
		}
	t /= (double) NTIMES;
	t *= MILLION;

	MASTER printf("%g\t\tSend and receive of %d numbers\n",t,msglen);
	fflush(stdout);
/* ----------- Testing simultaneous execution of requests ---------- */

#if 1
	t = 0;
	 MPI_Barrier(comm);
	if(i_am_the_master)
	{
    MPI_Send_init(&buf[0],1,MPI_INT,1,TAG1,comm,&requests[0]);            
    MPI_Send_init(&buf[1],1,MPI_INT,1,TAG1,comm,&requests[1]);
    MPI_Send_init(&buf[2],1,MPI_INT,1,TAG1,comm,&requests[2]);
    MPI_Send_init(&buf[3],1,MPI_INT,1,TAG1,comm,&requests[3]);
    MPI_Send_init(&buf[4],1,MPI_INT,1,TAG1,comm,&requests[4]);

        t1 = Wtime();
	    for(i = 0; i < NTIMES; i ++) 
		{
            MPI_Startall(5,requests);
            MPI_Waitall(5,requests,statuses);
		}
        t2 = Wtime();
        t = t2 - t1;

	MPI_Request_free(&requests[0]);
	MPI_Request_free(&requests[1]);
	MPI_Request_free(&requests[2]);
	MPI_Request_free(&requests[3]);
	MPI_Request_free(&requests[4]);

	}
	else if(myid == 1) 
	{
    MPI_Recv_init(&obuf[0],1,MPI_INT,0,TAG1,comm,&requests[0]);
    MPI_Recv_init(&obuf[1],1,MPI_INT,0,TAG1,comm,&requests[1]);
    MPI_Recv_init(&obuf[2],1,MPI_INT,0,TAG1,comm,&requests[2]);
    MPI_Recv_init(&obuf[3],1,MPI_INT,0,TAG1,comm,&requests[3]);
    MPI_Recv_init(&obuf[4],1,MPI_INT,0,TAG1,comm,&requests[4]);

	    for(i = 0; i < NTIMES; i ++) 
		{
            MPI_Startall(5,requests);
            MPI_Waitall(5,requests,statuses);
		}

	MPI_Request_free(&requests[0]);
	MPI_Request_free(&requests[1]);
	MPI_Request_free(&requests[2]);
	MPI_Request_free(&requests[3]);
	MPI_Request_free(&requests[4]);

	}
	t /= (double) NTIMES;
	t *= MILLION;

	MASTER printf("%g\t\t5 sends of 1 number in one\n",t);
	fflush(stdout);
#endif

/* ----------- Testing execution of requests ---------- */

	t = 0;
	 MPI_Barrier(comm);
	if(i_am_the_master)
	{
        t1 = Wtime();
	    for(i = 0; i < NTIMES; i ++) 
		{
            MPI_Isend(&buf[0],1,MPI_INT,1,TAG1,comm,&requests[0]);            
            MPI_Isend(&buf[0],1,MPI_INT,1,TAG1,comm,&requests[1]);            
            MPI_Isend(&buf[0],1,MPI_INT,1,TAG1,comm,&requests[2]);            
            MPI_Isend(&buf[0],1,MPI_INT,1,TAG1,comm,&requests[3]);            
            MPI_Isend(&buf[0],1,MPI_INT,1,TAG1,comm,&requests[4]);            
            MPI_Waitall(5,requests,statuses);
		}
        t2 = Wtime();
		t = t2 - t1;
	}
	else if(myid == 1) 
	    for(i = 0; i < NTIMES; i ++) 
		{
			MPI_Irecv(&obuf[0],1,MPI_INT,MASTER_RANK,TAG1,comm,&requests[0]);
			MPI_Irecv(&obuf[1],1,MPI_INT,MASTER_RANK,TAG1,comm,&requests[1]);
			MPI_Irecv(&obuf[2],1,MPI_INT,MASTER_RANK,TAG1,comm,&requests[2]);
			MPI_Irecv(&obuf[3],1,MPI_INT,MASTER_RANK,TAG1,comm,&requests[3]);
			MPI_Irecv(&obuf[4],1,MPI_INT,MASTER_RANK,TAG1,comm,&requests[4]);
            MPI_Waitall(5,requests,statuses);
		}

	t /= (double) NTIMES;
	t *= MILLION;

 	MASTER printf("%g\t\t5 async. sends of 1 number\n",t);
	fflush(stdout);
/* ----------- Testing signal send/receive ---------- */
	t = 0;
	 MPI_Barrier(comm);
	if(i_am_the_master)
	{
     t1 = Wtime();
	    for(i = 0; i < NTIMES_LATENCY; i ++) 
		{
			MPI_Send(NULL,0,MPI_INT,1,TAG1,comm);
			MPI_Recv(NULL,0,MPI_INT,1,TAG1,comm,&status);
		}
    t2 = Wtime();
    t = t2 - t1;
	}
	else if(myid == 1) 
	    for(i = 0; i < NTIMES_LATENCY; i ++) 
		{
			MPI_Recv(NULL,0,MPI_INT,MASTER_RANK,TAG1,comm,&status);
			MPI_Send(NULL,0,MPI_INT,MASTER_RANK,TAG1,comm);
		}
		
	t /= (double) NTIMES_LATENCY;
	t *= MILLION;

	MASTER printf("%g\t\tsignal send and receive (double latency)\n",t);
	fflush(stdout);	
}

# define	M	2000
# ifndef MIN
# define MIN(x,y) ((x)<(y)?(x):(y))
# endif
# ifndef MAX
# define MAX(x,y) ((x)>(y)?(x):(y))
# endif

int checktick()
    {
    int		i, minDelta, Delta;
    double	t1, t2, timesfound[M];

/*  Collect a sequence of M unique time values from the system. */

    for (i = 0; i < M; i++) {
	t1 = Wtime();
	while( ((t2=Wtime()) - t1) < 1.0E-6 )
	    ;
	timesfound[i] = t1 = t2;
	}

/*
 * Determine the minimum difference between these M values.
 * This result will be our estimate (in microseconds) for the
 * clock granularity.
 */

    minDelta = 1000000;
    for (i = 1; i < M; i++) {
	Delta = (int)( 1.0E6 * (timesfound[i]-timesfound[i-1]));
	minDelta = MIN(minDelta, MAX(Delta,0));
	}

    return(minDelta);
    }
// end of MPI_TEST program