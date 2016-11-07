CREATE ARRAY left_array<val1:double>[k=0:5,3,0];
store(build(left_array,k+1),left_array);

CREATE ARRAY right_array<val2:double>[k=0:5,3,0];  
store(build(right_array,k+2),right_array); 

select * from left_array as l,right_array as r where l.val1=r.val2;      
select * from left_array as l,right_array as r where l.val1=r.val2 and l.val1>3; 

select * from left_array as l join right_array as r on l.val1=r.val2;
select * from left_array as l join right_array as r on l.val1=r.val2 where l.val1>3;
