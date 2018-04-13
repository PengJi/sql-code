#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

//求组合
#define MAX_LENGTH 20
int c=0; //计数
int result[13000][MAX_LENGTH]; //存储组合结果

/**
 * [run_master description]
 * @param  seg [description]
 * @return     [description]
 */
int get_distribution(int seg[]){
    FILE *fstream=NULL;      
    char buff[100];
	int seg_id,seg_count,count_num;

	//求记录个数
    memset(buff,0,sizeof(buff));
	if((fstream=popen("psql -d testDB -c 'select count(*) from (select gp_segment_id,count(*) from test2 group by gp_segment_id) as foo;'","r")) == NULL){
		fprintf(stderr,"execute command failed: %s",strerror(errno));
		return -1;
	}
	while(fgets(buff, sizeof(buff), fstream) != NULL){
		sscanf(buff, "%d", &count_num);
		//printf("%d\n", count_num);
	}

	//求记录
	memset(buff,0,sizeof(buff));
    if((fstream=popen("psql -d testDB -c 'select gp_segment_id,count(*) from test2 group by gp_segment_id;'","r")) == NULL){
        fprintf(stderr,"execute command failed: %s",strerror(errno));
        return -1;
	}
	for(int i=0; i<count_num+2; i++){
		fgets(buff, sizeof(buff), fstream);
		//printf("%s",buff);
		sscanf(buff,"%d | %d",&seg_id,&seg_count);
		//printf("%d,%d\n",seg_id,seg_count);
		if(seg_count != 0){
			seg[seg_id] = seg_count;
		}
	}

    pclose(fstream);

	// for(int i=0;i<16;i++){
	// 	printf("%d - %d\n",i,seg[i]);
	// }

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
	for(int i=0;i<n;i++){
		ini[i]=i+1;
	}

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
	int idx = 0;

	//计算数据分布
	int seg[16]={0};
	run_master(seg);
	for(int i=0;i<16;i++){
		printf("%d - %d\n",i,seg[i]);
		if(seg[i] == 0){
			ini[idx++] = i; 
		}
	}

	for(int i=0; i<idx; i++){
		printf("%d\n",ini[idx]);
	}

	//得到组合
	int r[MAX_LENGTH];
	int n=idx, m=2;
	get_comb(ini,r,n,m);

    return 0;
}
