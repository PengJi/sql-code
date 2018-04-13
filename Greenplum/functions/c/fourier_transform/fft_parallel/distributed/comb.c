#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_LENGTH 20
int c=0;
int result[13000][MAX_LENGTH];

/**
 * 递归求组合
 * @param ori   [description]
 * @param res   [description]
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

int get_comb(){
	int a[MAX_LENGTH]; //存储初始字符串
	int r[MAX_LENGTH]; //存储组合结果
	int n=16,m=8;

	for(int i=0;i<n;i++){
		a[i]=i+1;
	}

    combination(a,r,n,m, 0, 0);

	for(int i=0; i<c; i++){
		for(int j=0; j<m; j++){
			printf("%d ",result[i][j]);
		}
		printf("\n");
	}
	printf("The sum of combination: %d\n",c);	
}

/**
 * [run_master description]
 * @return [description]
 */
int run_master(){
	int seg[16]={0};	
    FILE *fstream=NULL;      
    char buff[100];
	int seg_id,seg_count,count_num;

    memset(buff,0,sizeof(buff));

	if((fstream=popen("psql -d testDB -c 'select count(*) from (select gp_segment_id,count(*) from test2 group by gp_segment_id) as foo;'","r")) == NULL){
		fprintf(stderr,"execute command failed: %s",strerror(errno));
		return -1;
	}
	while(fgets(buff, sizeof(buff), fstream) != NULL){
		sscanf(buff, "%d", &count_num);
		//printf("%d\n", count_num);
	}

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

	for(int i=0;i<16;i++){
		printf("%d - %d\n",i,seg[i]);
	}

    return 0;     
}

int main()
{
	get_comb();

	run_master();

    return 0;
}
