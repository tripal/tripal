/* For load_gff3.pl */
SET search_path = chado,public;

INSERT INTO contact (name, description) VALUES ('null', 'null')
  ON CONFLICT DO NOTHING;
INSERT INTO cv (name) VALUES ('null')
  ON CONFLICT DO NOTHING;
INSERT INTO cv (name, definition) VALUES ('local', 'Locally created terms')
  ON CONFLICT DO NOTHING;
INSERT INTO cv (name, definition) VALUES ('Statistical Terms', 'Locally created terms for statistics')
  ON CONFLICT DO NOTHING;
INSERT INTO db (name, description) VALUES ('null', 'Use when a database is not available.')
  ON CONFLICT DO NOTHING;

INSERT INTO dbxref (db_id, accession) VALUES (
  (SELECT db_id FROM db WHERE name = 'null'),
  'local:null'
) ON CONFLICT DO NOTHING;
INSERT INTO cvterm (name, cv_id, dbxref_id) VALUES (
  'null',
  (SELECT cv_id FROM cv WHERE name = 'null'),
  (SELECT dbxref_id FROM dbxref WHERE accession = 'local:null')
) ON CONFLICT DO NOTHING;

INSERT INTO pub (miniref, uniquename, type_id) VALUES (
  'null',
  'null',
  (SELECT cvterm_id FROM cvterm WHERE name = 'null')
) ON CONFLICT DO NOTHING;

INSERT INTO cv (name, definition) VALUES ('chado_properties', 'Terms that are used in the chadoprop table to describe the state of the database')
  ON CONFLICT DO NOTHING;

INSERT INTO dbxref (db_id, accession) VALUES (
  (SELECT db_id FROM db WHERE name = 'null'),
  'chado_properties:version'
) ON CONFLICT DO NOTHING;
INSERT INTO cvterm (name, definition, cv_id,dbxref_id) VALUES (
  'version',
  'Chado schema version',
  (SELECT cv_id FROM cv WHERE name = 'chado_properties'),
  (SELECT dbxref_id FROM dbxref WHERE accession = 'chado_properties:version')
) ON CONFLICT DO NOTHING;

-- This table will probably end up in general.sql.
CREATE TABLE IF NOT EXISTS materialized_view (
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
