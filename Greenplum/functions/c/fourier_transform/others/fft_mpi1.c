#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

/*
 * 并行FFT（快速傅里叶变换）程序  
 * http://blog.163.com/ld081055@126/blog/static/11818691520100894549728
 *
 */

#define intsize sizeof(int)
#define complexsize sizeof(complex)
#define PI 3.1415926

int *a,*b;
int nLen,init_nLen,M,m;
int myid,p;
double starttime,endtime,midtime;
FILE *dataFile;
MPI_Status status;

typedef struct{
     float real;
     float image;
}complex;

complex *A_In,*A_Out,*W,*P_In;

complex Add(complex, complex);
complex Sub(complex, complex);
complex Mul(complex, complex);
int calculate_M(int);
void reverse(int, int);
void readData();
void fft(int, int);
void printResult();

void readData()
{
     int i;
 
     starttime = MPI_Wtime();
     dataFile = fopen("dataIn.txt","r");
     fscanf(dataFile,"%d",&init_nLen);
     M = calculate_M(init_nLen);
     nLen = (int)pow(2,M);
    
     A_In = (complex *)malloc(complexsize*nLen);

     for(i=0; i<init_nLen; i++)
     {
          fscanf(dataFile,"%f",&A_In[i].real);
          A_In[i].image = 0.0;
     }
 
     fclose(dataFile);

     for(i=init_nLen; i<nLen; i++)
     {
          A_In[i].real = 0.0;
          A_In[i].image = 0.0;
     }

     printf("Reading initial datas:\n");
     for(i=0; i<nLen; i++)
     { 
          if(A_In[i].image < 0)
          { 
               printf("A_In[%d] = %f%fi\n",i,A_In[i].real,A_In[i].image);
          }
          else
          {
               printf("A_In[%d] = %f+%fi\n",i,A_In[i].real,A_In[i].image);
          }
     }
     printf("\n");
}

void printResult()
{
     int i;
 
     A_Out = (complex *)malloc(complexsize*nLen);
 
     reverse(nLen,M);
     for(i=0; i<nLen; i++)
     {
          A_Out[i].real = A_In[b[i]].real;
          A_Out[i].image = A_In[b[i]].image;
     }
     free(a);
     free(b);
  
     printf("Output results:\n");
     for(i=0; i<nLen; i++)
     {
          if(A_Out[i].image < 0)
          {
               printf("A_Out[%d] = %f%fi\n",i,A_Out[i].real,A_Out[i].image);
          }
          else
          {
               printf("A_Out[%d] = %f+%fi\n",i,A_Out[i].real,A_Out[i].image);
          }
     }
 
     endtime = MPI_Wtime();
     printf("Whole running time    = %f seconds\n",endtime-starttime);
     printf("Distribute data time  = %f seconds\n",midtime-starttime);
     printf("Parallel compute time = %f seconds\n",endtime-midtime);

     free(A_In);
     free(A_Out);
}


int calculate_M(int init_nLen)
{
     int i;
     int k;
 
     i = 0;
     k = 1;
     while(k < init_nLen)
     {
          k = k*2;
          i++;
     }

     return i;
}


void reverse(int nLen, int M)
{
     int i,j;
 
     a = (int *)malloc(intsize*M);
     b = (int *)malloc(intsize*nLen);
 
     for(i=0; i<M; i++)
     {
          a[i] = 0;
     }

     b[0] = 0;
     for(i=1; i<nLen; i++)
     {
          j = 0;
          while(a[j] != 0)
          {
               a[j] = 0;
               j++;
          }
  
          a[j] = 1;
          b[i] = 0;
          for(j=0; j<M; j++)
          {
               b[i] = b[i]+a[j]*(int)pow(2,M-1-j);
          }
     }
}

complex Add(complex c1, complex c2)
{
     complex c;
     c.real = c1.real+c2.real;
     c.image = c1.image+c2.image;
     return c;
}

complex Sub(complex c1, complex c2)
{
     complex c;
     c.real = c1.real-c2.real;
     c.image = c1.image-c2.image;
     return c;
}

complex Mul(complex c1, complex c2)
{
     complex c;
     c.real = c1.real*c2.real-c1.image*c2.image;
     c.image = c1.real*c2.image+c2.real*c1.image;
     return c;
}

int main(int argc,char **argv)
{
     int i,j,k,t;
     int lev,dist,s,c;
     int group_size;
     complex B;
 
     MPI_Init(&argc,&argv);
     MPI_Comm_size(MPI_COMM_WORLD,&group_size);
	 printf("process num: %d\n",group_size);

     MPI_Comm_rank(MPI_COMM_WORLD,&myid);
     p = group_size;
 
     if(myid == 0)
     {
          readData();
          m = nLen/p;

          MPI_Bcast(&M,1,MPI_INT,0,MPI_COMM_WORLD);
          MPI_Bcast(&nLen,1,MPI_INT,0,MPI_COMM_WORLD);
          MPI_Bcast(&m,1,MPI_INT,0,MPI_COMM_WORLD);
     }

     if(myid < p)
     {
          P_In = (complex *)malloc(complexsize*m);
     }

     if(myid == 0)
     {
          midtime = MPI_Wtime();
     }

     if(myid == 0)
     {
          for(i=0; i<m; i++)
          {
               P_In[i] = A_In[i*p];
          }

          for(i=1; i<p; i++)
          {
               for(j=0; j<m; j++)
               {
                    MPI_Send(&A_In[j*p+i],1,MPI_FLOAT,i,i,MPI_COMM_WORLD);
               }
          }
     }

     else
     {
          MPI_Recv(P_In,m,MPI_FLOAT,0,myid,MPI_COMM_WORLD,&status);
     }

     if(myid < p)
     {
          W = (complex *)malloc(complexsize*m/2);

          for(lev=1; lev<=log(m)/log(2); lev++)
          {
               dist = (int)pow(2,log(m)/log(2)-lev);
               for(t=0; t<dist; t++)
               {
                    c = 0;
                    s = 0;
                    for(k=t; k<m; k=k+2*dist)
                    {
                         s = c*(int)pow(2,log(nLen)/log(2)-lev);
                         W[s].real = (float)cos(2*PI*s/nLen);
                         W[s].image = (float)(-sin(2*PI*s/nLen));

                         B = Add(P_In[k],Mul(P_In[k+dist],W[s]));
                         P_In[k+dist] = Sub(P_In[k],Mul(P_In[k+dist],W[s]));
                         P_In[k].real = B.real;
                         P_In[k].image = B.image;

                         c = c+1;
                    }
               }
          }

     }

     if(myid != 0)
     {
          MPI_Send(&P_In[0],m,MPI_FLOAT,0,myid,MPI_COMM_WORLD);
     }

     if(myid == 0)
     {
          for(i=0; i<m; i++)
          {
               A_In[i*p] = P_In[i];
          }

          for(i=1; i<p; i++)
          {
               for(j=0; j<m; j++)
               {
                    MPI_Recv(&A_In[j*p+i],1,MPI_FLOAT,i,i,MPI_COMM_WORLD,&status);
               }
          }

          for(i=0; i<m; i++)
          {
               P_In[i] = A_In[i];
          }

          for(i=1; i<p; i++)
          {
               MPI_Send(&A_In[i*m],m,MPI_FLOAT,i,i,MPI_COMM_WORLD);
          }
     }

     if(myid != 0)
     {
          MPI_Recv(&P_In[0],m,MPI_FLOAT,0,myid,MPI_COMM_WORLD,&status);
     }

     if(myid < p)
     {
          for(lev=(int)(log(m)/log(2)+1); lev<=(int)(log(nLen)/log(2)); lev++)
          {
               dist = (int)pow(2,log(nLen)/log(2)-lev);
               for(t=0; t<dist; t++)
               {
                    c = 0;
                    s = myid*nLen/(2*p);
                    for(k=t; k<m; k=k+2*dist)
                    {
                         s = s+c*(int)pow(2,log(nLen)/log(2)-lev);
                         W[s].real = (float)cos(2*PI*s/nLen);
                         W[s].image = (float)(-sin(2*PI*s/nLen));

                         B = Add(P_In[k],Mul(P_In[k+dist],W[s]));
                         P_In[k+dist] = Sub(P_In[k],Mul(P_In[k+dist],W[s]));
                         P_In[k].real = B.real;
                         P_In[k].image = B.image;

                         c = c+1;
                    }
               }
          }
     }

     if(myid != 0)
     {
          MPI_Send(&P_In[0],m,MPI_FLOAT,0,myid,MPI_COMM_WORLD);
     }

     if(myid == 0)
     {
          for(i=0; i<m; i++)
          {
               A_In[i] = P_In[i];
          }

          for(i=1; i<p; i++)
          {
               MPI_Recv(&A_In[i*m],m,MPI_FLOAT,i,i,MPI_COMM_WORLD,&status);
          }
   
          printResult();
     }
 
     MPI_Finalize();
 
     if(myid < p)
     {
          free(P_In);
          free(W);
     }
 
     return 0;
}

/* 
 * 编译执行
 * mpicc fft_mpi.c -lm
 * ./a.out
 *
 */
