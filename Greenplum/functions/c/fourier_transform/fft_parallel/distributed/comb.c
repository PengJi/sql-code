#include "comb.h"

//求组合
#define MAX_LENGTH 20
int c=0; //计数，组合数
int result[13000][MAX_LENGTH]; //最终结果，存储所有组合结果

/**
 * 得到存有数据的segment个数
 * @return 返回存有数据的segment个数
 */
int get_row_num(){
	FILE *fstream=NULL;
	char buff[100];
	int count_num;
	memset(buff,0,sizeof(buff));

	if((fstream=popen("psql -d testDB -c 'select count(*) from (select gp_segment_id,count(*) from test2 group by gp_segment_id) as foo;'","r")) == NULL){
		fprintf(stderr,"execute command failed: %s",strerror(errno));
		return -1;
	}

	while(fgets(buff, sizeof(buff), fstream) != NULL){
		sscanf(buff, "%d", &count_num);
		//printf("%d\n", count_num);
	}

    pclose(fstream);
	return count_num;
}

/**
 * 得到segid的记录数
 * @param  segid gp_segment_id
 * @return       返回segid的记录数
 */
int get_row(int segid){
	FILE *fstream=NULL;
	char buff[100];
	int count_num;
	memset(buff,0,sizeof(buff));

	if((fstream=popen("psql -d testDB -c 'select count(*) from test2 where gp_segment_id=4'","r")) == NULL){
		fprintf(stderr,"execute command failed: %s",strerror(errno));
		return -1;
	}

	while(fgets(buff, sizeof(buff), fstream) != NULL){
		sscanf(buff, "%d", &count_num);
		printf("%d\n",count_num);
	}

    pclose(fstream);
	return count_num;
}

/**
 * 得到数据分布
 * @param  cnt  存有数据的segment个数
 * @param  seg  存储每个segment的数据条数
 * @param  segs 存有数据的segment
 * @return      结果
 */
int get_distribution(int cnt, int seg[], struct Segdata segs[]){
    FILE *fstream=NULL;      
    char buff[100];
	int seg_id,seg_count,count_data=0;

	//求记录
	memset(buff,0,sizeof(buff));
    if((fstream=popen("psql -d testDB -c 'select gp_segment_id,count(*) from test2 group by gp_segment_id;'","r")) == NULL){
        fprintf(stderr,"execute command failed: %s",strerror(errno));
        return -1;
	}
	for(int i=0; i<cnt+2; i++){
		fgets(buff, sizeof(buff), fstream);
		//printf("%s",buff);
		if(i<2) continue;
		sscanf(buff,"%d | %d",&seg_id,&seg_count);
		//printf("%d,%d\n",seg_id,seg_count);
		seg[seg_id] = seg_count;
		if(seg_count != 0){
			segs[count_data].seg_id = seg_id;
			segs[count_data].seg_count = seg_count;
			count_data++;
		}
	}

    pclose(fstream);
    return 0;     
}

/**
 * 递归求组合
 * @param ori   原始集合
 * @param res   组合
 * @param n     初始集合总数
 * @param m     每个组合的个数
 * @param k     初始集合中当前处理的位置索引，ori[k]
 * @param index 所求组合结果数据的索引，res[index]
 */
void combination(int ori[], int res[], int n, int m, int k, int index){
    int i;
    if(index == m){ //输出组合结果
        for(i = 0; i < m; ++i){
			//printf("%d ", res[i]);
			result[c][i] = res[i];
		}
		//printf("\n");
		
		c++;

        return;
    }

    for(i = k; i < n; ++i){
        res[index] = ori[i];
        combination(ori,res,n, m, i + 1, index + 1);//注意第三个参数是i+1
    }
}

/**
 * 得到组合结果
 * @param  ini 存储初始字符串
 * @param  r   存储临时组合结果
 * @param  n   总个数
 * @param  m   每个组合中的个数
 * @return     [description]
 */
int get_comb(int ini[],int r[],int n, int m){
    combination(ini,r,n,m, 0, 0);

    /*
    //打印结果
	for(int i=0; i<c; i++){ //一共有c种组合
		for(int j=0; j<m; j++){ //每种组合中有m个项
			printf("%d ",result[i][j]);
		}
		printf("\n");
	}
	printf("The sum of combination: %d\n",c);
	c = 0;
	*/
	
	return 0;
}

/**
 * 确定数据分布
 * @return [description]
 */
int judge_seg(){
	int ini[MAX_LENGTH];
	int idx=0, count_num;
	struct Segdata *segs=NULL; //数组，存储所有含有记录的segment

	struct timeval start_total, end_total;
	struct timeval start, end;
	unsigned long duration,duration_total;
	gettimeofday(&start_total,NULL);

	//计算数据分布
	gettimeofday(&start,NULL);
	int seg[16]={0};
	printf("数据分布:\n");
	count_num = get_row_num();

	segs = (struct Segdata*) malloc(sizeof(segdata)*count_num);
	get_distribution(count_num,seg,segs);
	for(int i=0;i<16;i++){
		printf("%d - %d\n",i,seg[i]);
		if(seg[i] == 0){
			ini[idx++] = i; 
		}
	}
	gettimeofday(&end,NULL);
	duration = 1000000*(end.tv_sec-start.tv_sec) + end.tv_usec-start.tv_usec;
	printf("%ld ms\n",duration/1000);

	/*
	printf("idx=%d\n",idx);
	for(int j=0; j<idx; j++){
		printf("%d\n",ini[j]);
	}
	*/

	printf("存有数据的segment:\n");
	for(int i=0; i<count_num; i++){
		printf("%d,%d\n",segs[i].seg_id, segs[i].seg_count);
	}

	//对每个存有记录的segment求组合
	int r[MAX_LENGTH]; //存储递归过程中的中间结果
	int n=idx+1, m=count_num-1;
	int move_row_count;//需要迁移的记录数
	combcost.total_cost = 0;
	//combcost.comb[16]={0};
	for(int i=0; i<count_num; i++){ //对每个存有记录的segment循环
		printf("针对记录:\n");
		printf("%d,%d\n",segs[i].seg_id, segs[i].seg_count);
		printf("组合:\n");
		ini[idx] = segs[i].seg_id; //该segment中存有记录

		//依次求组合，组合中的segment个数分别为：1,2,3,...,j
		for(int j=1; j<=n; j++){
			printf("current gp_segment_id: %d\n",segs[i].seg_id);
			get_comb(ini,r,n,j);//计算所有组合

			move_row_count = segs[i].seg_count/j; //计算需要迁移的记录数

			int *tmp = (int *) malloc(sizeof(int) * c);
			for(int a=0; a<c; a++){ //一共有c种组合
				for(int b=0; b<j; b++){ //每种组合中有j个项
					printf("%d ",result[a][b]); //全局变量
					tmp[b] = result[a][b];
				}

				//计算seg[i].seg_id迁移到result[a][b]的代价
				if(abs(combcost.total_cost - 
					cost_sum(segs[i].seg_id, tmp, tmp, move_row_count)) >= 10){ //找到代价更小的方案
				}else{ //进一步判断等待时间
				}
				printf("\n");
			}
			printf("The sum of combination: %d\n",c);
			c = 0; //全局变量

			break;
		}

	}

	gettimeofday(&end_total,NULL);
	duration_total = 1000000*(end_total.tv_sec-start_total.tv_sec) + end_total.tv_usec-start_total.tv_usec;
	printf("%ld ms.\n",duration_total/1000);

	return 0;
}

/**
 * 计算每个segment的cpu代价
 * @param  segid    gp_segment_id
 * @param  row_size 记录条数
 * @return          CPU代价
 */
int cost_cpu(int segid, int row_size){
	FILE *fstream=NULL;      
	char buff[100];
	float load_max = 4.0; //每个节点的最大负载
	int row_max = 12; //每个segment的最大记录条数
	float arg_load, arg_time;
	int imp = 100; //影响因子

	//得到每个segment的CPU负载，uptime
	memset(buff,0,sizeof(buff));
	if((fstream=popen("uptime","r")) == NULL){
		fprintf(stderr,"execute command failed: %s",strerror(errno));
		return -1;
	}
	fgets(buff, sizeof(buff), fstream);
	printf("the cpu cost is: %s\n",buff);

	//1分钟内的负载
	float cp1 = (buff[54]-'0') + (buff[56]-'0')*0.1 + (buff[57]-'0')*0.01;
	printf("load of 1 minute: %f\n", cp1);

	//5分钟内的负载
	float cp5 = (buff[60]-'0') + (buff[62]-'0')*0.1 + (buff[63]-'0')*0.01;
	printf("load of 5 minutes: %f\n", cp5);

	//15分钟内的负载
	float cp15 = (buff[66]-'0') + (buff[68]-'0')*0.1 + (buff[69]-'0')*0.1;
	printf("load of 15 minutes: %f\n", cp15);

	arg_load = (cp5/load_max) * imp;

	//得到CPU处理记录耗费
	arg_time = row_size/row_max;
	printf("cpu cost is: %f\n", arg_load + arg_time);

	pclose(fstream);
	return arg_load + arg_time;
}

/**
 * 计算每个segment的io代价
 * @param  segid    gp_segment_id
 * @param  flag     当为0时，表示读；当为1时，表示写；当为2时，表示既有读也有写
 * @param  row_size 记录条数
 * @return          IO代价
 */
int cost_io(int segid, int flag,int row_size){
	FILE *fstream=NULL;
	char buff[100];
	float arg_load, arg_time=0.0;
	float writeMB = 50; //平均写速率MB/s
	float readMB = 200; //平均读速率MB/s
	float row_bytes = 32; //每条记录的大小，KB
	int imp = 100; //影响因子

	//得到每个segment的I/O负载，iostat得到磁盘使用率
    memset(buff,0,sizeof(buff));
    if((fstream=popen("iostat -x","r")) == NULL){
        fprintf(stderr,"execute command failed: %s",strerror(errno));
        return -1;
    }
	//获取负载(磁盘使用率: %util)
   	fgets(buff, sizeof(buff), fstream);
	for(int i=0; i<8; i++){
    	fgets(buff, sizeof(buff), fstream);
		if(i==7){
    		//printf("the io cost is: %s\n",buff);
			arg_load = ((buff[18]-'0') + (buff[20]-'0')*0.1 + (buff[21]-'0')*0.01) * imp;
		}
	}
	printf("IO load is: %f\n", arg_load);
	
	//得到IO处理记录的时间
	if(flag == 0){ //读时间
		arg_time += (row_size*row_bytes/1024)/readMB;
	}else if(flag == 1){ //写时间
		arg_time += (row_size*row_bytes/1024)/writeMB;
	}else{ //读写事件
		arg_time += (row_size*row_bytes/1024)/readMB;
		arg_time += (row_size*row_bytes/1024)/writeMB;
	}

	printf("IO cost is: %f\n", arg_load + arg_time);

	return arg_load + arg_time;
}

/**
 * 计算网络代价
 * @param  from_segid [description]
 * @param  to_segid   [description]
 * @param  row_size   [description]
 * @return            网络代价
 */
int cost_net(int from_segid, int to_segid, int row_size){
	if(from_segid == to_segid){
		return 0;
	}

	FILE *fstream=NULL;
	char buff[100];
	int imp=100;

	//网络负载，netstat
	
	//数据传输时间

	return 0;
}

/**
 * 计算每个分布的代价
 * @param  from_segid [description]
 * @param  to_segid   [description]
 * @return            返回代价
 */
int cost_per(int from_segid, int to_segid, int row_size){
	return cost_cpu(to_segid, row_size) + cost_io(to_segid, 1, row_size) +
	cost_net(from_segid, to_segid, row_size); 
}

/**
 * 计算每一种方案的总代价
 * @param  from_segid 从哪个segment节点发送
 * @param  to_segs    要发送到的segment
 * @param  row_num    每一种方案中的segment个数
 * @param  row_size   分发的记录条数
 * @return            返回方案的总代价
 */
int cost_sum(int from_segid, int to_segs[], int row_num, int row_size){
	int total=0;

	for(int i=0; i<row_num; i++){
		if(from_segid == to_segs[i]) continue; //代价放到for外面计算
		total += cost_per(from_segid, to_segs[i], row_size);
	}
	total += cost_cpu(from_segid, row_size);
	total += cost_io(from_segid, 0, row_size);

	return total; 
}

/**
 * 计算任务的平均等待时间
 * @param  from_segid [description]
 * @param  to_segid   [description]
 * @return            [description]
 */
int cost_wait(int from_segid, int to_segid){
	//分析每个segment的执行日志，判断任务的平均等待时间

	return 0;
}

/**
 * 迁移数据
 * @param segid [description]
 */
int move_row(int segid){
	return 0;
}

int main(){
	printf("%d\n",get_row(1));
	//judge_seg();
	cost_cpu(1,2);
	cost_io(1,1,2);

    return 0;
}
