#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

//求组合
#define MAX_LENGTH 20
int c=0; //计数
int result[13000][MAX_LENGTH]; //存储组合结果

struct Segdata{
	int seg_id;
	int seg_count;
}segdata;

/**
 * 得到存有数据的segment总数
 * @return 返回存有数据的segment总数
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
 * @param  cnt  存有记录的segment个数
 * @param  seg  保存所有的segment分布
 * @param  segs 保存存有记录的segment
 * @return      [description]
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
void combination(int ori[], int res[], int n, int m, int k, int index)
{
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
 * @param  n   [description]
 * @param  m   [description]
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
}

int main()
{
	int ini[MAX_LENGTH];
	int idx = 0,count_num;
	struct Segdata *segs=NULL;

	//计算数据分布
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

	/*
	printf("idx=%d\n",idx);
	for(int j=0; j<idx; j++){
		printf("%d\n",ini[j]);
	}
	*/

	printf("存有数据segment:\n");
	for(int i=0; i<count_num; i++){
		printf("%d,%d\n",segs[i].seg_id, segs[i].seg_count);
	}

	//得到组合
	int r[MAX_LENGTH];
	int n=idx, m=2;
	printf("组合:\n");
	get_comb(ini,r,n,m);

    return 0;
}
