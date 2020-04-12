/* For load_gff3.pl */

insert into contact (name,description) values ('null','null');
insert into cv (name) values ('null');
insert into cv (name,definition) values ('local','Locally created terms');
insert into cv (name,definition) values ('Statistical Terms','Locally created terms for statistics');
insert into db (name, description) values ('null', 'Use when a database is not available.');

insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'local:null');
insert into cvterm (name,cv_id,dbxref_id) values ('null',(select cv_id from cv where name = 'null'),(select dbxref_id from dbxref where accession='local:null'));

insert into pub (miniref,uniquename,type_id) values ('null','null',(select cvterm_id from cvterm where name = 'null'));

insert into cv (name,definition) values ('chado_properties','Terms that are used in the chadoprop table to describe the state of the database');

insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'chado_properties:version');
insert into cvterm (name,definition,cv_id,dbxref_id) values ('version','Chado schema version',(select cv_id from cv where name = 'chado_properties'),(select dbxref_id from dbxref where accession='chado_properties:version'));


--this table will probably end up in general.sql
 CREATE TABLE public.materialized_view   (       
                                materialized_view_id SERIAL,
                                last_update TIMESTAMP,
                                refresh_time INT,
                                name VARCHAR(64) UNIQUE,
                                mv_schema VARCHAR(64),
                                mv_table VARCHAR(128),
                                mv_specs TEXT,
                                indexed TEXT,
                                query TEXT,
                                special_index TEXT
                                );