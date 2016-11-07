create table left_table (tid1 int) distributed by (tid1);
create table right_table (tid2 int) distributed by (tid2);

insert into left_table values (1);
insert into left_table values (2);
insert into left_table values (3);
insert into left_table values (4);
insert into left_table values (5);
insert into left_table values (6);

insert into right_table values (2);
insert into right_table values (3);
insert into right_table values (4);
insert into right_table values (5);
insert into right_table values (6);
insert into right_table values (7);

explain  select * from left_table as l ,right_table as r where l.tid1=r.tid2;    
explain select * from left_table as l ,right_table as r where l.tid1=r.tid2 and l.tid1>3;

explain select * from left_table as l join right_table as r on l.tid1=r.tid2;
explain select * from left_table as l join right_table as r on l.tid1=r.tid2 where l.tid1>3;
