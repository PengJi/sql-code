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
	int32 arg_a = PG_GETARG_INT32(0);
	int32 arg_b = PG_GETARG_INT32(1);

	ereport(INFO,(errmsg("arg1: %d; arg2: %d",arg_a,arg_b)));

	PG_RETURN_INT32(arg_a + arg_b);
}

/*
编译
cc -fpic -c add_ereport.c -I/home/gpdba/greenplum/include/postgresql/server;
cc -shared -o add_ereport.so add_ereport.o;

复制到各节点
scp add_ereport.so gpdba@node1:/home/gpdba/sql_prac/Greenplum/functions/c;
scp add_ereport.so gpdba@node2:/home/gpdba/sql_prac/Greenplum/functions/c;
scp add_ereport.so gpdba@node3:/home/gpdba/sql_prac/Greenplum/functions/c;
scp add_ereport.so gpdba@node5:/home/gpdba/sql_prac/Greenplum/functions/c;

创建
create or replace function add(int,int)
returns int
as '/home/gpdba/sql_prac/Greenplum/functions/c/add_ereport', 'add_ab'
language c strict;
 */
