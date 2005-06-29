character set koi8r;


insert into t_filesearch_index (f_filesearch_file, f_text) select f_filesearch_file, substr(f_name from   1) from t_filesearch_file;
insert into t_filesearch_index (f_filesearch_file, f_text) select f_filesearch_file, substr(f_name from   2) from t_filesearch_file;
insert into t_filesearch_index (f_filesearch_file, f_text) select f_filesearch_file, substr(f_name from   3) from t_filesearch_file;
insert into t_filesearch_index (f_filesearch_file, f_text) select f_filesearch_file, substr(f_name from   4) from t_filesearch_file;
insert into t_filesearch_index (f_filesearch_file, f_text) select f_filesearch_file, substr(f_name from   5) from t_filesearch_file;
insert into t_filesearch_index (f_filesearch_file, f_text) select f_filesearch_file, substr(f_name from   6) from t_filesearch_file;
insert into t_filesearch_index (f_filesearch_file, f_text) select f_filesearch_file, substr(f_name from   7) from t_filesearch_file;
insert into t_filesearch_index (f_filesearch_file, f_text) select f_filesearch_file, substr(f_name from   8) from t_filesearch_file;
insert into t_filesearch_index (f_filesearch_file, f_text) select f_filesearch_file, substr(f_name from   9) from t_filesearch_file;


select r.f_filesearch_resource as R, r.f_address,r.f_share, f.f_filesearch_file as F,f.f_fil
esearch_resource,f.f_path,i.f_text from t_filesearch_resource r join t_filesearch_file f using (f_f
ilesearch_resource) join t_filesearch_index i using (f_filesearch_file) where i.f_text like 'гаска%';

select R.f_resource as RES, R.f_ipaddr, R.f_share, F.f_file as FILE, F.f_resource, F.f_path, F.f_size, I.f_key
from t_resource R join t_file F using (f_resource) join t_file_index I using (f_file)
where I.f_key like 'гаска%';
