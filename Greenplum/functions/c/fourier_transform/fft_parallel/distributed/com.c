#include <stdio.h>
#include <stdlib.h>

#define MAX_LENGTH 20

int c=0;
int result[13000][MAX_LENGTH];

//n, 初始字符串的长度
//m, 所求组合的长度
//k, 初始集合中当前处理位置, a[k]
//index, 所求组合结果数组的索引, r[index]
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

int main()
{
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

    return 0;
}
