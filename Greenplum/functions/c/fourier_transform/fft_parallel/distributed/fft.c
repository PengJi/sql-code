#include "postgres.h"
#include "funcapi.h"
#include "cdb/cdbvars.h"

#include "fft.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

/**
 * 复数相加
 * @param result [description]
 * @param c1     [description]
 * @param c2     [description]
 */
void comp_add(complex_t* result,const complex_t* c1,const complex_t* c2)
{
    result->r=c1->r+c2->r;
    result->i=c1->i+c2->i;
}

/**
 * 复数相乘
 * @param result [description]
 * @param c1     [description]
 * @param c2     [description]
 */
void comp_multiply(complex_t* result,const complex_t* c1,const complex_t* c2)
{
    result->r=c1->r*c2->r-c1->i*c2->i;
    result->i=c1->r*c2->i+c2->r*c1->i;
}

/**
 * 移动f中从beginPos到endPos位置的元素，使之按位置奇偶
 * 重新排列。举例说明:假设数组f，beginPos=2, endPos=5
 * 则shuffle函数的运行结果为f[2..5]重新排列，排列后各个
 * 位置对应的原f的元素为: f[2],f[4],f[3],f[5]
 * @param f        操作数组首地址
 * @param beginPos 第一个下标
 * @param endPos   最后一个下标
 */
void shuffle(complex_t* f, int beginPos, int endPos)
{
    int i;
    complex_t temp[2*MAX_N];

    for(i = beginPos; i <= endPos; i ++)
    {
        temp[i] = f[i];
    }

    int j = beginPos;
    for(i = beginPos; i <= endPos; i +=2)
    {
        f[j] = temp[i];
        j++;
    }
    for(i = beginPos +1; i <= endPos; i += 2)
    {
        f[j] = temp[i];
        j++;
    }
}

/**
 * 对复数序列f进行FFT或者IFFT(由x决定)，结果序列为y，
 * 产生leftPos 到 rightPos之间的结果元素
 * @param f           原始序列数组首地址
 * @param beginPos    原始序列在数组f中的第一个下标
 * @param endPos      原始序列在数组f中的最后一个下标
 * @param x           存放单位根的数组，其元素为w,w^2,w^3...
 * @param y           输出序列
 * @param leftPos     所负责计算输出的y的片断的起始下标
 * @param rightPos    所负责计算输出的y的片断的终止下标
 * @param totalLength y的长度
 */
void evaluate(complex_t* f, int beginPos, int endPos,const complex_t* x, complex_t* y,
int leftPos, int rightPos, int totalLength)
{
    int i;
    if ((beginPos > endPos)||(leftPos > rightPos))
    {
        printf("Error in use Polynomial!\n");
        exit(-1);
    }
    else if(beginPos == endPos)
    {
        for(i = leftPos; i <= rightPos; i ++)
        {
            y[i] = f[beginPos];
        }
    }
    else if(beginPos + 1 == endPos)
    {
        for(i = leftPos; i <= rightPos; i ++)
        {
            complex_t temp;
            comp_multiply(&temp, &f[endPos], &x[i]);
            comp_add(&y[i], &f[beginPos], &temp);
        }
    }
    else
    {
        complex_t tempX[2*MAX_N],tempY1[2*MAX_N], tempY2[2*MAX_N];
        int midPos = (beginPos + endPos)/2;

        shuffle(f, beginPos, endPos);

        for(i = leftPos; i <= rightPos; i ++)
        {
            comp_multiply(&tempX[i], &x[i], &x[i]);
        }

        evaluate(f, beginPos, midPos, tempX, tempY1,
            leftPos, rightPos, totalLength);
        evaluate(f, midPos+1, endPos, tempX, tempY2,
            leftPos, rightPos, totalLength);

        for(i = leftPos; i <= rightPos; i ++)
        {
            complex_t temp;
            comp_multiply(&temp, &x[i], &tempY2[i]);
            comp_add(&y[i], &tempY1[i], &temp);
        }
    }
}

/**
 * 打印完整实数
 * @param f       f为待打印数组的首地址
 * @param fLength fLength为数组的长度
 */
void print_res(const complex_t* f,int fLength)
{
	int i;

	for(i=0;i<fLength;i+=2)
	{		
		if(f[i].i<0)
			ereport(INFO,(errmsg("%f-%fi\n",f[i].r,-f[i].i)));
		else
			ereport(INFO,(errmsg("%f+%fi\n",f[i].r,f[i].i)));
	}
}

/**
 * 添加运行时间
 * @param toAdd 运行时间
 */
void addTransTime(double toAdd)
{
	transTime+=toAdd;
}

/**
 * 把原始数据发送给其它进程
 * @param size 集群中进程的数目
 */
void sendOrigData(int size)
{
	int i;

	for(i=1;i<size;i++)
	{
		//向所有进程发送数据的总个数
		MPI_Send(&variableNum,1,MPI_INT,i,V_TAG,MPI_COMM_WORLD);
		//向所有进程发送数据
		MPI_Send(p, variableNum * 2, MPI_DOUBLE, i, P_TAG, MPI_COMM_WORLD);
	}
}

/**
 * 接受原始数据，从进程0接收消息
 */
void recvOrigData()
{
	//从进程0接收数据的总个数
	MPI_Recv(&variableNum,1,MPI_INT,0,V_TAG,MPI_COMM_WORLD,&status);
	//从进程0接收所有的数据
	MPI_Recv(p, variableNum * 2, MPI_DOUBLE, 0, P_TAG, MPI_COMM_WORLD, &status);
}

/**
 * 读数据
 * @return [description]
 */
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

/**
 * @brief      计算每个segment的cpu代价
 *
 * @param[in]  segid  The flag
 *
 * @return     { description_of_the_return_value }
 */
int cost_cpu(int segid){
	return 1;
}

/**
 * @brief      计算每个segment的io代价
 *
 * @param[in]  segid     { parameter_description }
 *
 * @return     { description_of_the_return_value }
 */
int cost_io(int segid){
	return 1;
}

/**
 * @brief      { function_description }
 *
 * @param[in]  segid  The segid
 *
 * @return     { description_of_the_return_value }
 */
int cost_net(int segid){
	retunr 1;
}

/**
 * @brief      计算任务的平均等待时间
 *
 * @param[in]  segid  The segid
 *
 * @return     { description_of_the_return_value }
 */
int cost_wait(int segid){
	return 1;
}

/**
 * @brief      计算总代价
 *
 * @param[in]  segid  The segid
 *
 * @return     { description_of_the_return_value }
 */
int cost_sum(int segid){

	return cost_cpu() + cost_io() + cost_net() + cost_wait(); 
}

/**
 * fft主函数
 */
PG_FUNCTION_INFO_V1(fft);
Datum 
fft(PG_FUNCTION_ARGS){
	// 1.初始化旋转因子
	/*
	int wLength=2*variableNum;
	for(i=0;i<wLength;i++)
	{
		w[i].r=cos(i*2*PI/wLength);
		w[i].i=sin(i*2*PI/wLength);
	}
	*/
	
	// 2.各从节点计算部分傅里叶变换
	/*
	// 对p作FFT，输出序列为s，每个进程仅负责计算出序列中位置为 startPos 到 stopPos 的元素
	evaluate(p,0,variableNum-1,w,s,startPos,stopPos,wLength);
	// p 原始序列
	// 0 原始序列在数组f中的第一个下标
	// variableNum-1 原始序列在数组f中的最后一个下标
	// w 存放单位根的数组，其元素为w,w^2,w^3...
	// s 输出序列
	// startPos 所负责计算输出的y的片断的起始下标
	// stopPos 所负责计算输出的y的片断的终止下标
	// wLength s的长度
	*/
	
	// 3.将各部分的计算结果汇总
	/*
	//其他进程都将 s 中自己负责计算出来的部分发送给进程0，并从进程0接收汇总的s
	MPI_Send(s+startPos,everageLength*2,MPI_DOUBLE,0,S_TAG,MPI_COMM_WORLD);
	MPI_Recv(s,wLength*2,MPI_DOUBLE,0,S_TAG2,MPI_COMM_WORLD,&status);

	// 进程0接收s片段
	for(i=1;i<size;i++)
	{
		MPI_Recv(s+moreLength+i*everageLength,everageLength*2,MPI_DOUBLE,i,S_TAG,MPI_COMM_WORLD,&status);
	}

	// 进程0向其余进程发送完整的结果s	
	for(i=1;i<size;i++)
	{
		MPI_Send(s,wLength*2,MPI_DOUBLE,i,S_TAG2,MPI_COMM_WORLD);
	}
	 */
	
	//PG_RETURN_INT32(res);
    PG_RETURN_NULL();
}

/**
 * 从节点上执行fft
 */
PG_FUNCTION_INFO_V1(fft_exec);
Datum 
fft_exec(PG_FUNCTION_ARGS){
	// 先保存从节点上的所有数据，然后再计算
	int32 arg = PG_GETARG_INT32(0);

	char *command="select addab(n1,n2) from tb order by id";
	SPI_connect();
	SPI_exec(command, MAX_LINE);
	SPI_finish();

    //PG_RETURN_INT32(res);
    PG_RETURN_NULL();
}

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
	SPI_exec(command, MAX_LINE);
	SPI_finish();

    //PG_RETURN_INT32(res);
    PG_RETURN_NULL();
}
