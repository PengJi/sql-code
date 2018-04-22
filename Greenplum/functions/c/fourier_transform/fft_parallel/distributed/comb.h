#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include <sys/time.h>

#define MAX_LENGTH 20
int c=0; //计数，组合数
int result[13000][MAX_LENGTH]; //最终结果，存储所有组合结果

//字典表示segment对应的节点
char dict[16][10]={"node1","node1","node1","node1",
					"node2","node2","node2","node2",
					"node3","node3","node3","node3",
					"node5","node5","node5","node5"};

struct Segdata{
    int seg_id;
    int seg_count;
}segdata;

struct Combcost{
	int total_cost;
	int comb[16];
}combcost;

int get_row_num();
int get_row(int segid);
int get_distribution(int cnt, int seg[], struct Segdata segs[]);
void combination(int ori[], int res[], int n, int m, int k, int index);
int get_comb(int ini[],int r[],int n, int m);
