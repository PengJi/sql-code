#include "comb.h"

//�����
#define MAX_LENGTH 20
int c=0; //�����������
int result[13000][MAX_LENGTH]; //���ս�����洢������Ͻ��

/**
 * �õ��������ݵ�segment����
 * @return ���ش������ݵ�segment����
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
 * �õ�segid�ļ�¼��
 * @return ����segid�ļ�¼��
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
 * �õ����ݷֲ�
 * @param  cnt  �������ݵ�segment����
 * @param  seg  �洢ÿ��segment����������
 * @param  segs �������ݵ�segment
 * @return      
 */
int get_distribution(int cnt, int seg[], struct Segdata segs[]){
    FILE *fstream=NULL;      
    char buff[100];
	int seg_id,seg_count,count_data=0;

	//���¼
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
 * �ݹ������
 * @param ori   ԭʼ����
 * @param res   ���
 * @param n     ��ʼ��������
 * @param m     ÿ����ϵĸ���
 * @param k     ��ʼ�����е�ǰ�����λ��������ori[k]
 * @param index ������Ͻ�����ݵ�������res[index]
 */
void combination(int ori[], int res[], int n, int m, int k, int index){
    int i;
    if(index == m){ //�����Ͻ��
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
        combination(ori,res,n, m, i + 1, index + 1);//ע�������������i+1
    }
}

/**
 * �õ���Ͻ��
 * @param  ini �洢��ʼ�ַ���
 * @param  r   �洢��ʱ��Ͻ��
 * @param  n   �ܸ���
 * @param  m   ÿ������еĸ���
 * @return     [description]
 */
int get_comb(int ini[],int r[],int n, int m){
    combination(ini,r,n,m, 0, 0);

	for(int i=0; i<c; i++){ //һ����c�����
		for(int j=0; j<m; j++){ //ÿ���������m����
			printf("%d ",result[i][j]);
		}
		printf("\n");
	}
	printf("The sum of combination: %d\n",c);
	c = 0;
	
	return 0;
}

/**
 * ȷ�����ݷֲ�
 * @return [description]
 */
int judge_seg(){
	int ini[MAX_LENGTH];
	int idx=0, count_num;
	struct Segdata *segs=NULL; //���飬�洢���к��м�¼��segment

	struct timeval start_total, end_total;
	struct timeval start, end;
	unsigned long duration,duration_total;
	gettimeofday(&start_total,NULL);

	//�������ݷֲ�
	gettimeofday(&start,NULL);
	int seg[16]={0};
	printf("���ݷֲ�:\n");
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

	printf("�������ݵ�segment:\n");
	for(int i=0; i<count_num; i++){
		printf("%d,%d\n",segs[i].seg_id, segs[i].seg_count);
	}

	//��ÿ�����м�¼��segment�����
	int r[MAX_LENGTH]; //�洢�ݹ�����е��м���
	int n=idx+1,m=count_num-1;
	for(int i=0; i<count_num; i++){ //��ÿ�����м�¼��segmentѭ��
		printf("��Լ�¼:\n");
		printf("%d,%d\n",segs[i].seg_id, segs[i].seg_count);
		printf("���:\n");
		ini[idx] = segs[i].seg_id; //��segment�д��м�¼

		//���������
		for(int j=1; j<=n; j++){
			printf("current gp_segment_id: %d\n",segs[i].seg_id);
			get_comb(ini,r,n,j);
			break;
		}

		//for(int j=0;j<idx+1;j++) printf("%d\n", ini[j]);
		//get_comb(ini,r,n,m);
	}

	gettimeofday(&end_total,NULL);
	duration_total = 1000000*(end_total.tv_sec-start_total.tv_sec) + end_total.tv_usec-start_total.tv_usec;
	printf("%ld ms.\n",duration_total/1000);

	return 0;
}

/**
 * ����ÿ��segment��cpu����
 * @param  segid [description]
 * @return       [description]
 */
int cost_cpu(segid){
	//�õ�ÿ��segment��CPU����
	
	//�õ�CPU�����¼�ķ�

	return 0;
}

/**
 * ����ÿ��segment��io����
 * @param  segid [description]
 * @return       [description]
 */
int cost_io(segid){
	//�õ�ÿ��segment��I/O����
	
	//�õ�IO�����¼��ʱ��

	return 0;
}

/**
 * �����������
 * @param  from_segid [description]
 * @param  to_segid   [description]
 * @return            [description]
 */
int cost_net(int from_segid, int to_segid){
	//���縺��
	
	//���ݴ���ʱ��

	return 0;
}

/**
 * ���������ƽ���ȴ�ʱ��
 * @param  from_segid [description]
 * @param  to_segid   [description]
 * @return            [description]
 */
int cost_wait(int from_segid, int to_segid){
	//����ÿ��segment��ִ����־���ж������ƽ���ȴ�ʱ��

	return 0;
}

/**
 * �����ܴ���
 * @param  from_segid [description]
 * @param  to_segid   [description]
 * @return            [description]
 */
int cost_sum(int from_segid,int to_segid){

	return 0; 
}

/**
 * Ǩ������
 * @param segid [description]
 */
int move_row(int segid){
	return 0;
}

int main(){
	printf("%d\n",get_row(int segid));
	judge_seg();

    return 0;
}
