#include "postgres.h"
#include "funcapi.h"
#include "cdb/cdbvars.h"

#include "rg.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

/**
 * 测试并发查询个数
 *
 * select testconcur(1) from test;
 */
PG_FUNCTION_INFO_V1(test_concur);
Datum test_concur(PG_FUNCTION_ARGS){
	ereport(INFO,(errmsg("test_concur")));

	int32 arg = PG_GETARG_INT32(0);
	int a = 1;
	int i,j;
    clock_t start, end;
    double total_time;

	start = clock();

	for(i=0;i<100000;i++){
		for(j=0;j<100000;j++){
			a = a + 1;
			a = a - 1;
		}
	}

	end = clock();
	total_time = (double)(end - start) / CLOCKS_PER_SEC;
	ereport(INFO,(errmsg("total_time:%f secs",total_time)));

	PG_RETURN_INT32(arg);
}

/**
 * 测试CPU
 *
 * select testcpu(1) from test;
 */
PG_FUNCTION_INFO_V1(test_cpu);
Datum test_cpu(PG_FUNCTION_ARGS){
    ereport(INFO,(errmsg("test_cpu")));

    int32 arg = PG_GETARG_INT32(0);
    int a = 1;
    int i,j;
    clock_t start, end;
    double total_time;

    start = clock();

    /*
    for(i=0;i<100000;i++){
        for(j=0;j<100000;j++){
            a = a + 1;
            a = a - 1;
        }
    }
    */

    double input[SIZE];
    Complex dst[SIZE];

	for(i=0; i<5; i++){
    	setInput(input,SIZE);
	    DFT(input, dst, SIZE);
	    FFT(input, dst, SIZE);
	}

    end = clock();
    total_time = (double)(end - start) / CLOCKS_PER_SEC;
    ereport(INFO,(errmsg("total_time:%f secs",total_time)));

    PG_RETURN_INT32(arg);
}

/**
 * 测试I/O带宽
 *
 * select testio(1) from test;
 */
PG_FUNCTION_INFO_V1(test_io);
Datum test_io(PG_FUNCTION_ARGS){
    ereport(INFO,(errmsg("test_io")));
    int32 arg = PG_GETARG_INT32(0);

    //测试写
    system("dd if=/dev/zero of=/home/gpdba/test1 oflag=direct count=1024 bs=1M");

    PG_RETURN_INT32(arg);
}

//定义一个复数计算，包括乘法，加法，减法
void Add_Complex(Complex * src1,Complex *src2,Complex *dst){
    dst->imagin=src1->imagin+src2->imagin;
    dst->real=src1->real+src2->real;
}
void Sub_Complex(Complex * src1,Complex *src2,Complex *dst){
    dst->imagin=src1->imagin-src2->imagin;
    dst->real=src1->real-src2->real;
}
void Multy_Complex(Complex * src1,Complex *src2,Complex *dst){
    double r1=0.0,r2=0.0;
    double i1=0.0,i2=0.0;
    r1=src1->real;
    r2=src2->real;
    i1=src1->imagin;
    i2=src2->imagin;
    dst->imagin=r1*i2+r2*i1;
    dst->real=r1*r2-i1*i2;
}

/**
 * 在FFT中有一个WN的n次方项，在迭代中会不断用到，具体见算法说明
 * @param n      [description]
 * @param size_n [description]
 * @param dst    [description]
 */
void getWN(double n,double size_n,Complex * dst){
    double x=2.0*M_PI*n/size_n;
    dst->imagin=-sin(x);
    dst->real=cos(x);
}

/**
 * 随机生成一个输入，显示数据部分已经注释掉了
 * 注释掉的显示部分为数据显示，可以观察结果
 * @param data [description]
 * @param n    [description]
 */
void setInput(double * data,int  n){
    //printf("Setinput signal:\n");
    srand((int)time(0));
    for(int i=0;i<SIZE;i++){
        data[i]=rand()%VALUE_MAX;
        //printf("%lf\n",data[i]);
    }

}

/**
 * 定义DFT函数，其原理为简单的DFT定义，时间复杂度O（n^2）,
 * 下面函数中有两层循环，每层循环的step为1，size为n，故为O（n*n）,
 * 注释掉的显示部分为数据显示，可以观察结果
 * @param src  [description]
 * @param dst  [description]
 * @param size [description]
 */
void DFT(double * src,Complex * dst,int size){
    clock_t start,end;
    start=clock();
    
    for(int m=0;m<size;m++){
        double real=0.0;
        double imagin=0.0;
        for(int n=0;n<size;n++){
            double x=M_PI*2*m*n;
            real+=src[n]*cos(x/size);
            imagin+=src[n]*(-sin(x/size));
        
        }
        dst[m].imagin=imagin;
        dst[m].real=real;
       /* 
		if(imagin>=0.0)
            printf("%lf+%lfj\n",real,imagin);
        else
            printf("%lf%lfj\n",real,imagin);
		*/
    }
    end=clock();
    printf("DFT use time :%lf for Datasize of:%d\n",(double)(end-start)/CLOCKS_PER_SEC,size);

}

/**
 * 定义IDFT函数，其原理为简单的IDFT定义，时间复杂度O（n^2）,
 * 下面函数中有两层循环，每层循环的step为1，size为n，故为O（n*n）,
 * @param src  [description]
 * @param dst  [description]
 * @param size [description]
 */
void IDFT(Complex *src,Complex *dst,int size){
    clock_t start,end;
    start=clock();
    for(int m=0;m<size;m++){
        double real=0.0;
        double imagin=0.0;
        for(int n=0;n<size;n++){
            double x=M_PI*2*m*n/size;
            real+=src[n].real*cos(x)-src[n].imagin*sin(x);
            imagin+=src[n].real*sin(x)+src[n].imagin*cos(x);
            
        }
        real/=SIZE;
        imagin/=SIZE;
        if(dst!=NULL){
            dst[m].real=real;
            dst[m].imagin=imagin;
        }
        if(imagin>=0.0)
            printf("%lf+%lfj\n",real,imagin);
        else
            printf("%lf%lfj\n",real,imagin);
    }
    end=clock();
    printf("IDFT use time :%lfs for Datasize of:%d\n",(double)(end-start)/CLOCKS_PER_SEC,size);
    

}

/**
 * 定义FFT的初始化数据，因为FFT的数据经过重新映射，递归结构
 * @param  src    [description]
 * @param  size_n [description]
 * @return        [description]
 */
int FFT_remap(double * src,int size_n){
    if(size_n==1)
        return 0;
    double * temp=(double *)malloc(sizeof(double)*size_n);

    for(int i=0;i<size_n;i++)
        if(i%2==0)
            temp[i/2]=src[i];
        else
            temp[(size_n+i)/2]=src[i];

    for(int i=0;i<size_n;i++)
        src[i]=temp[i];

    free(temp);
    FFT_remap(src, size_n/2);
    FFT_remap(src+size_n/2, size_n/2);

    return 1;
}

/**
 * 定义FFT，具体见算法说明，注释掉的显示部分为数据显示，可以观察结果
 * @param src    [description]
 * @param dst    [description]
 * @param size_n [description]
 */
void FFT(double * src,Complex * dst,int size_n){
    FFT_remap(src, size_n);

    for(int i=0;i<size_n;i++)
        printf("%lf\n",src[i]);

	clock_t start,end;
    start=clock();
    int k=size_n;
    int z=0;
    while (k/=2) {
        z++;
    }
    k=z;
    if(size_n!=(1<<k))
        exit(0);
    Complex * src_com=(Complex*)malloc(sizeof(Complex)*size_n);
    if(src_com==NULL)
        exit(0);
    for(int i=0;i<size_n;i++){
        src_com[i].real=src[i];
        src_com[i].imagin=0;
    }

    for(int i=0;i<k;i++){
        z=0;
        for(int j=0;j<size_n;j++){
            if((j/(1<<i))%2==1){
                Complex wn;
                getWN(z, size_n, &wn);
                Multy_Complex(&src_com[j], &wn,&src_com[j]);
                z+=1<<(k-i-1);
                Complex temp;
                int neighbour=j-(1<<(i));
                temp.real=src_com[neighbour].real;
                temp.imagin=src_com[neighbour].imagin;
                Add_Complex(&temp, &src_com[j], &src_com[neighbour]);
                Sub_Complex(&temp, &src_com[j], &src_com[j]);
            }
            else
                z=0;
        }
    
    }

    /*
	for(int i=0;i<size_n;i++)
    	if(src_com[i].imagin>=0.0){
            printf("%lf+%lfj\n",src_com[i].real,src_com[i].imagin);
        }
        else
            printf("%lf%lfj\n",src_com[i].real,src_com[i].imagin);
	*/

	for(int i=0;i<size_n;i++){
		dst[i].imagin=src_com[i].imagin;
		dst[i].real=src_com[i].real;
	}

	free(src_com);
	end=clock();
    printf("FFT use time :%lfs for Datasize of:%d\n",(double)(end-start)/CLOCKS_PER_SEC,size_n);
    
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

	for(i=0;i<10000;i++){
		for(j=0;j<10000;j++){
			a = a + 1;
			a = a - 1;
		}
	}

	/*
	while(1){
		a++;
		a--;
	}
	*/

	ereport(INFO,(errmsg("a=%d",a)));

	PG_RETURN_INT32(arg);
}

/**
 * 测试在主节点/从节点上执行
 *
 * 函数只在主节点上执行
 * select hello_gprole(1);
 *
 * 函数只在segment上执行
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

		for(i=0;i<100000;i++){
			for(j=0;j<100000;j++){
				a++;
				a--;;
			}
		}
	}else if(Gp_role == GP_ROLE_DISPATCH){
		ereport(INFO,(errmsg("GP_ROLE_DISPATCH")));
		a = a+2;

		for(i=0;i<100000;i++){
			for(j=0;j<100000;j++){
				a++;
				a--;
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

	for(i=0;i<10000;i++){
		for(j=0;j<10000;j++){
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
