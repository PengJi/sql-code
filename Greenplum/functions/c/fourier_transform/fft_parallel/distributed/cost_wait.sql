SELECT
  -- procpid,
  -- START,
  now() - START AS lap
  -- current_query,
  -- count() over() count_num,
  -- t2.rolname,
  -- t3.rsqname
FROM
    (
        SELECT
            backendid,
			pg_stat_get_backend_userid(S.backendid) as uid,
			pg_stat_get_backend_client_addr(S.backendid) as ip,
            pg_stat_get_backend_pid (S.backendid) AS procpid,
            pg_stat_get_backend_activity_start (S.backendid) AS START,
            pg_stat_get_backend_activity (S.backendid) AS current_query
        FROM
            (
                SELECT
                    pg_stat_get_backend_idset () AS backendid
            ) AS S
    ) AS t1 left join pg_authid  t2 on t1.uid=t2.oid
    left join pg_resqueue t3 on t2.rolresqueue=t3.oid
where current_query != '<IDLE>'
ORDER BY lap DESC;

/*
psql -d testDB -c "SELECT
  procpid,
  START,
  now() - START AS lap,
  current_query,
  -- count() over() count_num,
  t2.rolname,
  t3.rsqname
FROM
    (
        SELECT
            backendid,
			pg_stat_get_backend_userid(S.backendid) as uid,
			pg_stat_get_backend_client_addr(S.backendid) as ip,
            pg_stat_get_backend_pid (S.backendid) AS procpid,
            pg_stat_get_backend_activity_start (S.backendid) AS START,
            pg_stat_get_backend_activity (S.backendid) AS current_query
        FROM
            (
                SELECT
                    pg_stat_get_backend_idset () AS backendid
            ) AS S
    ) AS t1 left join pg_authid  t2 on t1.uid=t2.oid
    left join pg_resqueue t3 on t2.rolresqueue=t3.oid
where current_query != '<IDLE>'
ORDER BY lap DESC;" -xt
*/
