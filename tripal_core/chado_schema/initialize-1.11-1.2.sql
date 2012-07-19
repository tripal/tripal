insert into cv (name,definition) values ('chado_properties','Terms that are used in the chadoprop table to describe the state of the database');
insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'chado_properties:version');
insert into cvterm (name,definition,cv_id,dbxref_id) values ('version','Chado schema version',(select cv_id from cv where name = 'chado_properties'),(select dbxref_id from dbxref where accession='chado_properties:version'));

INSERT INTO chadoprop (type_id, value) VALUES 
  ((SELECT cvterm_id 
   FROM cvterm CVT 
     INNER JOIN cv CV on CVT.cv_id = CV.cv_id 
   WHERE CV.name = 'chado_properties' AND CVT.name = 'version'),
   '1.2')
