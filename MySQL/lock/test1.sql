-- 数据准备
CREATE TABLE user(
    id INT PRIMARY KEY, 
    name VARCHAR(20)
    );

-- 插入数据
INSERT INTO user(id, name) VALUES(1, 'A');
INSERT INTO user(id, name) VALUES(2, 'B');
INSERT INTO user(id, name) VALUES(3, 'C');

-- 模拟死锁
-- session1
begin;
UPDATE user SET name='AA' WHERE id = 1;

-- session0
SELECT * FROM information_schema.innodb_trx \G
*************************** 1. row ***************************
                    trx_id: 7695
                 trx_state: RUNNING
               trx_started: 2018-09-04 10:29:09
     trx_requested_lock_id: NULL
          trx_wait_started: NULL
                trx_weight: 3
       trx_mysql_thread_id: 31078
                 trx_query: NULL
       trx_operation_state: NULL
         trx_tables_in_use: 0
         trx_tables_locked: 1
          trx_lock_structs: 2
     trx_lock_memory_bytes: 1136
           trx_rows_locked: 1
         trx_rows_modified: 1
   trx_concurrency_tickets: 0
       trx_isolation_level: REPEATABLE READ
         trx_unique_checks: 1
    trx_foreign_key_checks: 1
trx_last_foreign_key_error: NULL
 trx_adaptive_hash_latched: 0
 trx_adaptive_hash_timeout: 0
          trx_is_read_only: 0
trx_autocommit_non_locking: 0

SELECT * FROM information_schema.innodb_locks \G
Empty set, 1 warning (0.00 sec)

SELECT * FROM information_schema.innodb_lock_waits \G
Empty set, 1 warning (0.00 sec)

-- session2
-- 锁超时等待时间
SHOW VARIABLES LIKE '%innodb_lock_wait%';
+--------------------------+-------+
| Variable_name            | Value |
+--------------------------+-------+
| innodb_lock_wait_timeout | 50    |
+--------------------------+-------+

UPDATE user SET name='AAA' WHERE id = 1;
ERROR 1205 (HY000): Lock wait timeout exceeded; try restarting transaction  --超过50秒

-- session0
SELECT * FROM information_schema.innodb_trx \G
*************************** 1. row ***************************
                    trx_id: 7696
                 trx_state: LOCK WAIT
               trx_started: 2018-09-04 10:32:49
     trx_requested_lock_id: 7696:53:3:5
          trx_wait_started: 2018-09-04 10:32:49
                trx_weight: 2
       trx_mysql_thread_id: 31084
                 trx_query: UPDATE user SET name='AAA' WHERE id = 1
       trx_operation_state: starting index read
         trx_tables_in_use: 1
         trx_tables_locked: 1
          trx_lock_structs: 2
     trx_lock_memory_bytes: 1136
           trx_rows_locked: 1
         trx_rows_modified: 0
   trx_concurrency_tickets: 0
       trx_isolation_level: REPEATABLE READ
         trx_unique_checks: 1
    trx_foreign_key_checks: 1
trx_last_foreign_key_error: NULL
 trx_adaptive_hash_latched: 0
 trx_adaptive_hash_timeout: 0
          trx_is_read_only: 0
trx_autocommit_non_locking: 0
*************************** 2. row ***************************
                    trx_id: 7695
                 trx_state: RUNNING
               trx_started: 2018-09-04 10:29:09
     trx_requested_lock_id: NULL
          trx_wait_started: NULL
                trx_weight: 3
       trx_mysql_thread_id: 31078
                 trx_query: SELECT * FROM information_schema.innodb_trx
       trx_operation_state: NULL
         trx_tables_in_use: 0
         trx_tables_locked: 1
          trx_lock_structs: 2
     trx_lock_memory_bytes: 1136
           trx_rows_locked: 1
         trx_rows_modified: 1
   trx_concurrency_tickets: 0
       trx_isolation_level: REPEATABLE READ
         trx_unique_checks: 1
    trx_foreign_key_checks: 1
trx_last_foreign_key_error: NULL
 trx_adaptive_hash_latched: 0
 trx_adaptive_hash_timeout: 0
          trx_is_read_only: 0
trx_autocommit_non_locking: 0

-- 查看锁
select * from information_schema.innodb_locks;
+-------------+-------------+-----------+-----------+------------------+------------+------------+-----------+----------+-----------+
| lock_id     | lock_trx_id | lock_mode | lock_type | lock_table       | lock_index | lock_space | lock_page | lock_rec | lock_data |
+-------------+-------------+-----------+-----------+------------------+------------+------------+-----------+----------+-----------+
| 7700:53:3:5 | 7700        | X         | RECORD    | `testrep`.`user` | PRIMARY    |         53 |         3 |        5 | 1         |
| 7695:53:3:5 | 7695        | X         | RECORD    | `testrep`.`user` | PRIMARY    |         53 |         3 |        5 | 1         |
+-------------+-------------+-----------+-----------+------------------+------------+------------+-----------+----------+-----------+

-- 查看锁等待
select * from information_schema.innodb_lock_waits;
+-------------------+-------------------+-----------------+------------------+
| requesting_trx_id | requested_lock_id | blocking_trx_id | blocking_lock_id |
+-------------------+-------------------+-----------------+------------------+
| 7700              | 7700:53:3:5       | 7695            | 7695:53:3:5      |
+-------------------+-------------------+-----------------+------------------+

-- 查看锁等待的查询
select 
r.trx_id waiting_trx_id,
r.trx_mysql_thread_id waiting_thread,
r.trx_query waiting_query,
b.trx_id blocking_trx_id,
b.trx_mysql_thread_id blocking_thread,
b.trx_query blocking_query
from information_schema.innodb_lock_waits w
inner join information_schema.innodb_trx b 
on b.trx_id = w.blocking_trx_id
inner join information_schema.innodb_trx r
on r.trx_id = w.requesting_trx_id\G
*************************** 1. row ***************************
 waiting_trx_id: 7700
 waiting_thread: 31084
  waiting_query: UPDATE user SET name='AAA' WHERE id = 1
blocking_trx_id: 7695
blocking_thread: 31078
 blocking_query: NULL