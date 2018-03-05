#include "postgres.h"
#include "funcapi.h"
#include "cdb/cdbvars.h"

#include "rg.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

/**
 * 测试在主节点上执行
 *
 * 只在主节点上执行
 * select hello(1);
 */
PG_FUNCTION_INFO_V1(hello);
Datum 
hello(PG_FUNCTION_ARGS){
	int32 arg = PG_GETARG_INT32(0);
	int a = 1;
	int i,j;

	ereport(INFO,(errmsg("hello")));

	if (Gp_role == GP_ROLE_EXECUTE){
		ereport(INFO,(errmsg("GP_ROLE_EXECUTE")));
		a = a+1;
	}else if(Gp_role == GP_ROLE_DISPATCH){
		ereport(INFO,(errmsg("GP_ROLE_DISPATCH")));
		a = a+2;
	}else if(Gp_role == GP_ROLE_UTILITY){
		ereport(INFO,(errmsg("GP_ROLE_UTILITY")));
	}

	for(i=0;i<1000000;i++){
		for(j=0;j<30000;j++){
		}
	}

	ereport(INFO,(errmsg("a=%d",a)));

	PG_RETURN_INT32(arg);
}

/**
 * 测试在主节点/从节点上执行
 *
 * 函数只在主节点上执行
 * select hello_gprole(1);
 *
 * 函数只在主节点上执行
 * select hello_gprole(1) from test;
 *
 * 函数只在segment上执行
 * select hello_gprole(id) from test;
 */
PG_FUNCTION_INFO_V1(hello_gprole);
Datum 
hello_gprole(PG_FUNCTION_ARGS){
	int32 arg = PG_GETARG_INT32(0);
	int a = 1;
	int i,j;

	if (Gp_role == GP_ROLE_EXECUTE){
		ereport(INFO,(errmsg("GP_ROLE_EXECUTE")));
		a = a+1;

		for(i=0;i<1000000;i++){
			for(j=0;j<20000;j++){
			}
		}
	}else if(Gp_role == GP_ROLE_DISPATCH){
		ereport(INFO,(errmsg("GP_ROLE_DISPATCH")));
		a = a+2;

		for(i=0;i<1000000;i++){
			for(j=0;j<20000;j++){
			}
		}
	}else if(Gp_role == GP_ROLE_UTILITY){
		ereport(INFO,(errmsg("GP_ROLE_UTILITY")));
	}

	/*
	//当在segment上执行的时候，下面语句也只在segment上执行
	for(i=0;i<1000000;i++){
		for(j=0;j<20000;j++){
		}
	}
	*/

	ereport(INFO,(errmsg("a=%d",a)));

	PG_RETURN_INT32(arg);
}

/**
 * 基本加法
 * 对表中的记录相加
 */
PG_FUNCTION_INFO_V1(add_ab);
Datum 
add_ab(PG_FUNCTION_ARGS){
	int32 arg_a = PG_GETARG_INT32(0);
	int32 arg_b = PG_GETARG_INT32(1);

	ereport(INFO,(errmsg("arg1: %d; arg2: %d",arg_a,arg_b)));

	PG_RETURN_INT32(arg_a + arg_b);
}

/** 
 * 对表中的记录相加，重复计算
 *
 * 在主节点上执行
 * select addab(1,2);
 *
 * 在segment上执行
 * select addab(n1,n2) from tb;
 */
PG_FUNCTION_INFO_V1(add_for);
Datum 
add_for(PG_FUNCTION_ARGS){
	int i,j;
	int32 arg_a = PG_GETARG_INT32(0);
	int32 arg_b = PG_GETARG_INT32(1);
	int res;

	//ereport(INFO,(errmsg("arg1: %d; arg2: %d",arg_a,arg_b)));

	for(i=0;i<1000000;i++){
		for(j=0;j<30000;j++){
			res = arg_a + arg_b;
		}
	}

	PG_RETURN_INT32(res);
	//PG_RETURN_NULL();
}

/**
 * 调用自定义函数
 * select call_udf(1);
 */
PG_FUNCTION_INFO_V1(call_udf);
Datum
call_udf(PG_FUNCTION_ARGS){
	int32 arg = PG_GETARG_INT32(0);

	char *command="select addab(n1,n2) from tb order by id";
	SPI_connect();
	SPI_exec(command, 10000);
	SPI_finish();

    //PG_RETURN_INT32(res);
    PG_RETURN_NULL();
}
