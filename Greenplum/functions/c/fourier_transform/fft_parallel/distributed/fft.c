#include "postgres.h"
#include "funcapi.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

PG_FUNCTION_INFO_V1(hello);
Datum 
hello(PG_FUNCTION_ARGS){
	int32 arg = PG_GETARG_INT32(0);

	ereport(INFO,(errmsg("num_threads")));

	PG_RETURN_INT32(arg);
}


