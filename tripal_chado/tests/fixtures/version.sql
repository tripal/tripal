-- Adds version information to the `chadoprop` table so that tests
-- don't fail the schema check.
INSERT INTO cv (name, definition) 
  VALUES ('chado_properties', 
          'Terms that are used in the chadoprop table to describe the state of the database');
  
INSERT INTO db (name, description, urlprefix, url) 
  VALUES ('null', 
          'No online database.', 
          '/cv/lookup/{db}/{accession}', 
          '/cv/lookup/null');

INSERT INTO dbxref (db_id, accession) 
  SELECT db_id, 'chado_properties:version' as accession 
  FROM db 
  WHERE name = 'null';
  
INSERT INTO cvterm (cv_id, dbxref_id, name, definition) 
  SELECT cv_id, 
    (SELECT dbxref_id 
     FROM dbxref 
     WHERE accession = 'chado_properties:version') as dbxref_id,
    'version' as name, 
    'Chado schema version' as definition
  FROM cv 
  WHERE  name = 'chado_properties';

INSERT INTO chadoprop (type_id, value, rank) 
  SELECT cvterm_id as type_id, '1.3' as value, 0 as rank
  FROM cvterm
  WHERE name = 'version';
