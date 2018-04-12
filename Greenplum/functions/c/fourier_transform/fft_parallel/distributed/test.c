#include <stdio.h>    
#include <string.h>
#include <errno.h>

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

int main(){
	run_master();
	
	return 0;
}
