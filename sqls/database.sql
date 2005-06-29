set character set koi8r;

/* таблица заданий: какие блоки адресов или конкретные адреса сканировать */
create table t_query (
	f_query		integer		not null	auto_increment,
	f_active	bit		not null	default 0,
	/* идентифицирующие поля: их вариация может давать доступ к разным данным */
	f_protocol	integer		not null	default 0,
	f_isnetwork	bit		not null	default 0,
	f_ipaddr	varchar(250)	not null	default '0.0.0.0',
	f_ipmask	integer		null		default null,
	f_ipport	integer		null		default null,
	f_share		varchar(250)	null		default null,
	f_username	varchar(250)	null		default null,
	/* поля данных: параметры подключения к указанным идентифицирующим параметрам */
	f_password	varchar(250)	null		default null,
	f_workgroup	varchar(250)	null		default null,
	f_selfname	varchar(250)	null		default null,
	f_timeout	integer		null		default null,
	f_depth		integer		null		default null,
	primary key (f_query)) type=innodb;

/* таблица состояния просканированности - временные данные для сканера */
create table t_status (
	/* process group identifier */
	f_startup	datetime	not null	,
	f_process	integer		not null	,
	/* host status identifiers */
	f_protocol	integer		not null	,
	f_ipaddr	varchar(250)	not null	,
	f_ipport	integer		not null	,
	f_share		varchar(250)	not null	,
	f_username	varchar(250)	not null	
	) type=innodb;

create table t_statistics (
	/* process group identifier */
	f_startup	datetime	not null	,
	f_process	integer		not null	,
	/* statistics records (counts) */
	f_count_status_check
	f_count_status_renew
	f_count_status_clean
	f_count_resource_find
	f_count_resource_add
	f_count_resource_index
	f_count_resource_loose
	f_count_resource_loosf
	f_count_resource_touch
	f_count_file_find
	f_count_file_add
	f_count_file_index
	f_count_file_loose
	f_count_file_touch
	f_count_file_change
	/* statistics records (utimes) */
	f_utime_status_check
	f_utime_status_renew
	f_utime_status_clean
	f_utime_resource_find
	f_utime_resource_add
	f_utime_resource_index
	f_utime_resource_loose
	f_utime_resource_loosf
	f_utime_resource_touch
	f_utime_file_find
	f_utime_file_add
	f_utime_file_index
	f_utime_file_loose
	f_utime_file_touch
	f_utime_file_change
	/* threads */
	f_ctl_count
	f_ctl_utime
	f_smb_count
	f_smb_utime
	f_ftp_count
	f_ftp_utime
	primary key (f_startup, f_process)) type=innodb;

/* таблица обнаруженных хостов - управляется сканером автоматически */
create table t_resource (
	f_resource	integer		not null	auto_increment,
	/* ссылка на запрос для получения параметров подключения, и конкретные значения вариативных полей запроса */
	f_query		integer		not null	references t_query(f_query) on update cascade on delete cascade,
	f_ipaddr	varchar(250)	not null	,
	f_share		varchar(250)	not null	,
	/* данные о ресурсе */
	f_found		datetime	null		default null,
	f_seen		datetime	null		default null,
	f_lost		datetime	null		default null,

	primary key (f_resource)) type=innodb;

/* таблица обнаруженных файлов - управляется сканером автоматически */
create table t_file (
	f_file		integer		not null	auto_increment,
	f_resource	integer		not null	references t_resource(f_resource) on update cascade on delete cascade,

	f_found		datetime	null		default null,
	f_seen		datetime	null		default null,
	f_lost		datetime	null		default null,

	f_container	bit		not null	,
	f_size		bigint		not null	,
	f_ctime		datetime	not null	,
	f_mtime		datetime	not null	,

	f_path		text		not null	,
	f_name		text		not null	,

	primary key (f_file)) type=innodb;

CREATE TABLE t_resource_index (
	f_resource	integer		not null	references t_resource(f_resource) on update cascade on delete cascade,
	f_key		text		not null	
	) type=innodb;

CREATE TABLE t_file_index (
	f_file		integer		not null	references t_file(f_file) on update cascade on delete cascade,
	f_key		text		not null	
	) type=innodb;




create index i_query_active on t_query (f_active);
create index i_query_shareipmask on t_query (f_share, f_ipmask);
create index i_status_all on t_status (f_startup, f_process, f_protocol, f_ipaddr, f_ipport, f_share, f_username);

create index i_resource_ipaddrshare on t_resource (f_ipaddr, f_share);
create index i_query_protocolipportusername on t_query (f_protocol, f_ipport, f_username);

create index i_resource_lostseenipaddr on t_resource (f_lost, f_seen, f_ipaddr);
create index i_resource_lostipaddr on t_resource (f_lost, f_ipaddr);
create index i_file_lostresource on t_file (f_lost, f_resource);
create index i_file_resourcepath on t_file (f_resource, f_path(500));
create index i_file_lostseenresource on t_file (f_lost, f_seen, f_resource);

create index i_resource_index_resource on t_resource_index (f_resource);
create index i_resource_index_key on t_resource_index (f_key(500));
create index i_file_index_file on t_file_index (f_file);
create index i_file_index_key  on t_file_index (f_key(500));

/* начальные данные */
insert into t_query (f_active, f_protocol, f_ipaddr   , f_ipmask, f_isnetwork, f_share, f_timeout)
             values (0       , 1         , '10.0.1.10', 25      , 1          , NULL   , 5        )
                  , (1       , 1         , '10.10.1.0', 24      , 1          , NULL   , 10       )
                  , (1       , 1         , '10.10.2.0', 24      , 1          , NULL   , 10       )
                  , (1       , 1         , '10.10.3.0', 24      , 1          , NULL   , 20       )
                  , (0       , 1         , '10.10.4.0', 24      , 1          , NULL   , 30       )
                  , (1       , 1         , '10.10.5.0', 24      , 1          , NULL   , 10       )
                  , (1       , 1         , '10.10.6.0', 24      , 1          , NULL   , 20       )
                  , (1       , 1         , '10.10.7.0', 24      , 1          , NULL   , 10       )
                  , (1       , 1         , '10.10.8.0', 24      , 1          , NULL   , 20       )
                  , (1       , 1         , '10.10.9.0', 24      , 1          , NULL   , 10       )
;

