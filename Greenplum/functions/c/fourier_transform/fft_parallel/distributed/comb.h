#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int get_row_num();
int get_distribution(int cnt, int seg[], struct Segdata segs[]);
void combination(int ori[], int res[], int n, int m, int k, int index);
int get_comb(int ini[],int r[],int n, int m)