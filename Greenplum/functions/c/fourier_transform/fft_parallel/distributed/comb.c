#include "comb.h"

//求组合
#define MAX_LENGTH 20
int c=0; //计数
int result[13000][MAX_LENGTH]; //存储组合结果

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
 * 得到数据分布
 * @param  cnt  存有数据的segment个数
 * @param  seg  存储每个segment的数据条数
 * @param  segs 存有数据的segment
 * @return      
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

	for(int i=0; i<c; i++){
		for(int j=0; j<m; j++){
			printf("%d ",result[i][j]);
		}
		printf("\n");
	}
	printf("The sum of combination: %d\n",c);

	c = 0;
	
	return 0;
}

/**
 * 确定数据分布
 * @return [description]
 */
int judge_seg(){
	int ini[MAX_LENGTH];
	int idx=0, count_num;
	struct Segdata *segs=NULL;

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
	duration = 1000000*(end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;
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
	int r[MAX_LENGTH];
	int n=idx+1,m=count_num-1;
	for(int i=0; i<count_num; i++){
		printf("针对记录:\n");
		printf("%d,%d\n",segs[i].seg_id, segs[i].seg_count);
		printf("组合:\n");
		ini[idx] = segs[i].seg_id; //该segment中存有记录

		//依次求组合
		for(int j=1; j<=n; j++){
			get_comb(ini,r,n,j);
		}

		//for(int j=0;j<idx+1;j++) printf("%d\n", ini[j]);
		//get_comb(ini,r,n,m);
	}

	gettimeofday(&end_total,NULL);
	duration_total = 1000000*(end_total.tv_sec-start_total.tv_sec)+ end_total.tv_usec-start_total.tv_usec;
	printf("%ld ms.\n",duration_total/1000);

	return 0;
}

/**
 * 计算每个segment的cpu代价
 * @param  segid [description]
 * @return       [description]
 */
int cost_cpu(segid){
	//得到每个segment的CPU负载
	
	//得到CPU处理记录耗费

	return 0;
}

/**
 * 计算每个segment的io代价
 * @param  segid [description]
 * @return       [description]
 */
int cost_io(segid){
	//得到每个segment的I/O负载
	
	//得到IO处理记录的时间

	return 0;
}

/**
 * 计算网络代价
 * @param  from_segid [description]
 * @param  to_segid   [description]
 * @return            [description]
 */
int cost_net(int from_segid, int to_segid){

	return 0;
}

/**
 * 计算任务的平均等待时间
 * @param  from_segid [description]
 * @param  to_segid   [description]
 * @return            [description]
 */
int cost_wait(int from_segid, int to_segid){
	/*
	分析每个segment的执行日志，判断任务的平均等待时间
	*/
	return 0;
}

/**
 * 计算总代价
 * @param  from_segid [description]
 * @param  to_segid   [description]
 * @return            [description]
 */
int cost_sum(int from_segid,int to_segid){

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
	judge_seg();

    return 0;
}
