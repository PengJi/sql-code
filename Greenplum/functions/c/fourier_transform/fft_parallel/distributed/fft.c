#include "postgres.h"
#include "funcapi.h"

#include "cdb/cdbvars.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

int read_data(){
	/*
	//从数据库表中读取数据
	char *command="select val from test order by id";
	int ret;
	uint64 proc;
	float r;

	//command = text_to_cstring(PG_GETARG_TEXT_P(0));

    SPI_connect();
	    ret = SPI_exec(command, MAX_LINE);
	    variableNum=SPI_processed;
	    proc = SPI_processed;

		if((variableNum<1)||(variableNum>MAX_N))
		{
			ereport(INFO,(errmsg("variableNum out of range!")));
			return(FALSE);
		}
		ereport(INFO,(errmsg("variableNum=%d",variableNum)));

	    if (ret > 0 && SPI_tuptable != NULL){
	        TupleDesc tupdesc = SPI_tuptable->tupdesc;
	        SPITupleTable *tuptable = SPI_tuptable;
	        char buf[10];
	        uint64 j;

	        for (j = 0; j < proc; j++) //proc为表的行数
	        {
	            HeapTuple tuple = tuptable->vals[j];

	            for (i = 1, buf[0] = 0; i <= tupdesc->natts; i++){
	                snprintf(buf + strlen (buf), sizeof(buf) - strlen(buf), " %s%s",
	                        SPI_getvalue(tuple, tupdesc, i),
	                        (i == tupdesc->natts) ? " " : " |");
	            }

	            ereport(INFO,(errmsg("ROW: %s",buf))); //输出一行数据
	            sscanf(buf,"%f",&r);
				//准备数据
	            p[j].r = r;
	            p[j].i = 0.0f;
	        }
	    }

		// 打印原始数组
		// ereport(INFO,(errmsg("p(t)=")));
		// print_ereport(p,variableNum);

		SPI_finish();
		*/
	return 0;
}

/*
 * 测试在主节点上执行
 *
 * select fft(1);
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
 * 对表中的记录相加
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

/*
 * 调用自定义函数
 * select call_udf(1);
 */
PG_FUNCTION_INFO_V1(call_udf);
Datum
call_udf(PG_FUNCTION_ARGS){
	int32 arg = PG_GETARG_INT32(0);
	int MAX_LINE = 16384;

	char *command="select addab(n1,n2) from tb order by id";
	SPI_connect();
	SPI_exec(command, MAX_LINE);
	SPI_finish();

    //PG_RETURN_INT32(res);
    PG_RETURN_NULL();
}
