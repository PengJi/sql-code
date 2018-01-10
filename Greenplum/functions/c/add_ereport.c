#include "postgres.h"
#include "fmgr.h"
#include "utils/array.h"
#include "catalog/pg_type.h"

PG_MODULE_MAGIC;

/*
 * 基本加法
 */
PG_FUNCTION_INFO_V1(add_ab);
Datum 
add_ab(PG_FUNCTION_ARGS){
	ereport(INFO,
			(errcode(ERRCODE_UNDEFINED_OBJECT),
			errmsg("got invalid segno value NULL for tid %d",10)));

	int32 arg_a = PG_GETARG_INT32(0);
	int32 arg_b = PG_GETARG_INT32(1);

	PG_RETURN_INT32(arg_a + arg_b);
}
