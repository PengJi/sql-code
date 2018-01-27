#include<stdio.h>
#include<pthread.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>

#include "postgres.h"
#include "funcapi.h"
#include "access/heapam.h"
#include "access/relscan.h"
#include "utils/fmgroids.h"
#include "utils/tqual.h"
#include "utils/builtins.h"
#include "executor/spi.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

pthread_t main_tid;

void print_ids(const char *str)
{
    pid_t pid;      //进程id
    pthread_t tid;  //线程id
    pid = getpid();       //获取当前进程id
    tid = pthread_self(); //获取当前线程id

	ereport(INFO,(errmsg("%s pid: %u tid: %u (0x%x)\n",
				str,
				(unsigned int)pid,
				(unsigned int)tid,
				(unsigned int)tid)
				));
}

void *func(void *arg)
{
	ereport(INFO,(errmsg("new  thread:")));
    return ((void *)0);
}

PG_FUNCTION_INFO_V1(fft_main);
Datum
fft_main(PG_FUNCTION_ARGS)
{
    int err;
	int32 arg = PG_GETARG_INT32(0);

    err = pthread_create(&main_tid, NULL, func, NULL); //创建线程
    if(err != 0){
		ereport(INFO,(errmsg("create thread error: %s\n",strerror(err))));
        return 1;
    }

	ereport(INFO,(errmsg(
				"main thread: pid: %u tid: %u (0x%x)\n",
				(unsigned int)getpid(),
				(unsigned int)pthread_self(),
				(unsigned int)pthread_self()
				)));

    //print_ids("main thread:");
    //sleep(1);
    
	PG_RETURN_INT32(arg);
}

