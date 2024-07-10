SET search_path = "_test_biodb_réservé";

CREATE FUNCTION dummy(bigint) RETURNS integer
  LANGUAGE plpgsql
  AS $_$
DECLARE
  param alias for $1;
BEGIN
  RETURN 42 - param;
END;
$_$;

CREATE TABLE othertesttable (
  id serial NOT NULL,
  fk int NULL,
  CONSTRAINT othertesttable_pkey PRIMARY KEY (id)
);

INSERT INTO othertesttable SELECT generate_series, NULL FROM generate_series(0, 10000);

CREATE TABLE testtable (
  id serial NOT NULL,
  foreign_id int NULL,
  fieldbigint bigint NULL,
  fieldsmallint smallint NULL,
  fieldbool boolean NOT NULL DEFAULT false,
  fieldreal real NULL DEFAULT 1.0,
  fielddouble double precision NULL DEFAULT NULL,
  fieldchar character varying(255) NULL,
  fieldtext text NOT NULL,
  fieldbytea bytea NULL DEFAULT 'x',
  CONSTRAINT testtable_pkey PRIMARY KEY (id),
  CONSTRAINT testtable_c1 UNIQUE (fieldbigint, fieldsmallint),
  CONSTRAINT testtable_foreign_id_fkey FOREIGN KEY (foreign_id) REFERENCES othertesttable(id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED
);
CREATE UNIQUE INDEX testtable_c2 ON testtable USING btree (fieldbigint, fieldsmallint);
CREATE INDEX testtable_idx1 ON testtable USING btree (foreign_id);
COMMENT ON TABLE testtable IS 'Some long description
on multiple lines.';

ALTER TABLE ONLY othertesttable
  ADD CONSTRAINT othertesttable_fk_fkey FOREIGN KEY (fk) REFERENCES testtable(id) ON DELETE CASCADE;

CREATE VIEW dummy_test_view AS
  SELECT id AS dumid,
    fieldtext AS dumtext
  FROM testtable
  WHERE fieldbool = TRUE;

INSERT INTO testtable (
  foreign_id, fieldbigint, fieldsmallint, fieldbool, fieldreal, fielddouble,
  fieldchar, fieldtext, fieldbytea
) SELECT
  id, id*2048, id%127, (id%3!=1), id*random(), id*random()*1E10,
  to_hex(id*1024), 'This is a test data', 'Voilà, c\047est remplaçable.'::bytea
FROM othertesttable
WHERE id > 3000 AND id < 6000;
