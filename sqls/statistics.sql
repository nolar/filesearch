/*
итоги сканирования сети (только ip 10.10, протокол netbios) за эти выходные:
мегабайт                         : 519816
  в т.ч. dvd                     : 171813
  в т.ч. avi,mpg,mpeg            : 195679
  в т.ч. mp3,wav,mid,midi        :  99534
  в т.ч. iso,vcd,mdf             :  13943

количеств файлов и каталогов     : 124130
  в т.ч. dvd                     :     31
  в т.ч. avi,mpg,mpeg            :   1545
  в т.ч. mp3,wav,mid,midi        :  24851
  в т.ч. iso,vcd,mdf             :     27

компьютеров с netbios всего      :    215
  из них с непустыми ресурсами   :    106
ресурсов всего                   :   1000
  из них с непустыми данными     :    212
*/

select count(*) from t_filesearch_file
        where f_stamp_lost is null;
select count(*) from t_filesearch_file
        where f_stamp_lost is null
        and (f_name like 'VIDEO_TS');
select count(*) from t_filesearch_file
        where f_stamp_lost is null
        and (        f_name like '%avi'
                or f_name like '%mpg' or f_name like '%mpeg'
        );
select count(*) from t_filesearch_file
        where f_stamp_lost is null
        and (        f_name like '%mp3' or f_name like '%wav'
                or f_name like '%mid' or f_name like '%midi'
        );
select count(*) from t_filesearch_file
        where f_stamp_lost is null
        and (        f_name like '%vcd' or f_name like '%iso'
                or f_name like '%mdf'
        );



select round(sum(f_size)/1024/1024) from t_filesearch_file
        where f_stamp_lost is null;
select round(sum(f_size)/1024/1024) from t_filesearch_file
        where f_stamp_lost is null
        and (f_path like '%/VIDEO_TS/%');
select round(sum(f_size)/1024/1024) from t_filesearch_file
        where f_stamp_lost is null
        and (        f_name like '%avi'
                or f_name like '%mpg' or f_name like '%mpeg'
        );
select round(sum(f_size)/1024/1024) from t_filesearch_file
        where f_stamp_lost is null
        and (        f_name like '%mp3' or f_name like '%wav'
                or f_name like '%mid' or f_name like '%midi'
        );
select round(sum(f_size)/1024/1024) from t_filesearch_file
        where f_stamp_lost is null
        and (        f_name like '%vcd' or f_name like '%iso'
                or f_name like '%mdf'
        );


select count(distinct f_address) from t_filesearch_resource
        where f_stamp_lost is null;

select count(distinct f_address) from t_filesearch_resource
        where exists (select * from t_filesearch_file
        where f_filesearch_resource=t_filesearch_resource.f_filesearch_resource
                and f_stamp_lost is null
        ) and f_stamp_lost is null;

select count(f_share) from t_filesearch_resource
        where f_stamp_lost is null;

select count(f_share) from t_filesearch_resource
        where exists (select * from t_filesearch_file
        where f_filesearch_resource=t_filesearch_resource.f_filesearch_resource
                and f_stamp_lost is null
        ) and f_stamp_lost is null;

