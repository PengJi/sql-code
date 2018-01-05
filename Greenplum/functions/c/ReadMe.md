自定义函数c实现  
[Greenplum自定义函数](http://www.jpblog.cn/greenplum%E8%87%AA%E5%AE%9A%E4%B9%89%E5%87%BD%E6%95%B0.html)  
[扩展SQL](http://www.postgres.cn/docs/9.4/xfunc-c.html)  

生成目标文件  
`cc -fpic -c add_func.c -I/home/gpdba/greenplum/include/postgresql/server;`  
生成共享文件  
`cc -shared -o add_func.so add_func.o;`  
