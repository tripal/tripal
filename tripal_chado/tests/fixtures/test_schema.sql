CREATE FUNCTION create_point(bigint, bigint) RETURNS point
    LANGUAGE sql
    AS $_$SELECT point ($1, $2)$_$;

CREATE FUNCTION boxrange(bigint, bigint) RETURNS box
    LANGUAGE sql IMMUTABLE
    AS $_$SELECT box (create_point(0, $1), create_point($2,500000000))$_$;

CREATE TABLE db (
  db_id serial,
  name character varying(255) NOT NULL,
  description character varying(255) NULL,
  urlprefix character varying(255) NULL,
  url character varying(255) NULL,
  CONSTRAINT db_pkey PRIMARY KEY (db_id),
  CONSTRAINT db_c1 UNIQUE (name)
);

INSERT INTO db (name) VALUES ('test db');

CREATE TABLE dbxref (
  dbxref_id serial,
  db_id integer NOT NULL,
  accession character varying(255) NOT NULL,
  version character varying(255) NOT NULL DEFAULT '',
  description text NULL,
  CONSTRAINT dbxref_pkey PRIMARY KEY (dbxref_id),
  CONSTRAINT dbxref_c1 UNIQUE (db_id, accession, version)
);

INSERT INTO dbxref (db_id, accession) VALUES (1, 'test_dbxref');

CREATE TABLE cv (
  cv_id serial,
  name character varying(255) NOT NULL,
  definition text NULL,
  CONSTRAINT cv_pkey PRIMARY KEY (cv_id),
  CONSTRAINT cv_c1 UNIQUE (name)
);

INSERT INTO cv (name, definition) VALUES ('test_cv', 'CV for testing');

CREATE TABLE cvterm (
  cvterm_id serial,
  cv_id integer NOT NULL,
  name character varying(1024) NOT NULL,
  definition text NULL,
  dbxref_id integer NOT NULL,
  is_obsolete integer NOT NULL DEFAULT 0,
  is_relationshiptype integer NOT NULL DEFAULT 0,
  CONSTRAINT cvterm_pkey PRIMARY KEY (cvterm_id),
  CONSTRAINT cvterm_c1 UNIQUE (name, cv_id, is_obsolete),
  CONSTRAINT cvterm_c2 UNIQUE (dbxref_id)
);

INSERT INTO cvterm (cv_id, name, definition, dbxref_id) VALUES (1, 'test_cvterm', 'CV term for testing', 1);

CREATE TABLE organism (
  organism_id serial,
  abbreviation character varying(255) NULL,
  genus character varying(255) NOT NULL,
  species character varying(255) NOT NULL,
  common_name character varying(255) NULL,
  comment text NULL,
  CONSTRAINT organism_pkey PRIMARY KEY (organism_id),
  CONSTRAINT organism_c1 UNIQUE (genus, species)
);

INSERT INTO organism (genus, species) VALUES ('test genus', 'test sepcies');

CREATE TABLE feature (
  feature_id serial NOT NULL,
  dbxref_id integer NULL,
  organism_id integer NOT NULL,
  name character varying(255) NULL,
  uniquename text NOT NULL,
  residues text NULL,
  seqlen integer NULL,
  -- Mising column: md5checksum character(32) NULL,
  -- Extra column:
  testsum character(32) NULL,
  type_id integer NOT NULL,
  is_analysis boolean NOT NULL DEFAULT false,
  is_obsolete boolean NOT NULL DEFAULT false,
  timeaccessioned timestamp without time zone NOT NULL DEFAULT now(),
  timelastmodified timestamp without time zone NOT NULL DEFAULT now(),
  CONSTRAINT feature_pkey PRIMARY KEY (feature_id),
  CONSTRAINT feature_c1 UNIQUE (organism_id, uniquename, type_id)
);
CREATE INDEX feature_name_ind1 ON feature USING btree (name);
CREATE INDEX feature_idx1 ON feature USING btree (dbxref_id);
-- Will be missing: CREATE INDEX feature_idx2 ON feature USING btree (organism_id);
CREATE INDEX feature_idx3 ON feature USING btree (type_id);
CREATE INDEX feature_idx4 ON feature USING btree (uniquename);
CREATE INDEX feature_idx5 ON feature USING btree (lower(name));
COMMENT ON TABLE feature IS 'Some old description.';

INSERT INTO feature
  (dbxref_id,
  organism_id,
  name,
  uniquename,
  residues,
  seqlen,
  testsum,
  type_id)
SELECT
  NULL,
  1,
  to_hex(generate_series),
  'UN_' || to_hex(generate_series),
  to_hex((random()*10000)::int),
  123,
  to_hex((random()*10000)::int),
  1
FROM generate_series(0, 10000);

CREATE TABLE featureloc (
  featureloc_id serial,
  feature_id integer NOT NULL,
  srcfeature_id integer NULL,
  fmin integer NULL,
  is_fmin_partial boolean NOT NULL DEFAULT false,
  fmax integer NULL,
  is_fmax_partial boolean NOT NULL DEFAULT false,
  strand smallint NULL,
  phase integer NULL,
  residue_info text NULL,
  locgroup integer NOT NULL DEFAULT 0,
  rank integer NOT NULL DEFAULT 0,
  CONSTRAINT featureloc_pkey PRIMARY KEY (featureloc_id),
  CONSTRAINT featureloc_c1 UNIQUE (feature_id, locgroup, rank),
  CONSTRAINT featureloc_c2 CHECK ((fmin <= fmax))
);
CREATE INDEX binloc_boxrange ON featureloc USING gist (boxrange(fmin, fmax));
CREATE INDEX featureloc_idx1 ON featureloc USING btree (feature_id);
CREATE INDEX featureloc_idx2 ON featureloc USING btree (srcfeature_id);
CREATE INDEX featureloc_idx3 ON featureloc USING btree (srcfeature_id, fmin, fmax);
COMMENT ON TABLE featureloc IS 'The location of a feature relative to..';

CREATE TABLE chadoprop (
    chadoprop_id serial,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

CREATE TABLE feature_dbxref (
  feature_dbxref_id bigserial not null,
  primary key (feature_dbxref_id),
  feature_id bigint not null,
  foreign key (feature_id) references feature (feature_id) on delete cascade INITIALLY DEFERRED,
  dbxref_id bigint not null,
  foreign key (dbxref_id) references dbxref (dbxref_id) on delete cascade INITIALLY DEFERRED,
  is_current boolean not null default 'true',
  CONSTRAINT feature_dbxref_c1 UNIQUE (feature_id,dbxref_id)
);
CREATE INDEX feature_dbxref_idx1 ON feature_dbxref USING btree (feature_id);
CREATE INDEX feature_dbxref_idx2 ON feature_dbxref USING btree (dbxref_id);
COMMENT ON TABLE feature_dbxref IS 'Links a feature to dbxrefs.';
COMMENT ON COLUMN feature_dbxref.is_current IS 'True if this secondary dbxref is the most up to date accession in the corresponding db. Retired accessions should set this field to false';

CREATE TABLE feature_cvterm (
  feature_cvterm_id bigserial NOT NULL,
  primary key (feature_cvterm_id),
  feature_id bigint NOT NULL,
  foreign key (feature_id) references feature (feature_id) on delete cascade INITIALLY DEFERRED,
  cvterm_id bigint NOT NULL,
  foreign key (cvterm_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
  -- Mising column: pub_id bigint not null,
  is_not boolean NOT NULL DEFAULT FALSE,
  rank integer NOT NULL DEFAULT 0,
  CONSTRAINT feature_cvterm_c1 UNIQUE (feature_id,cvterm_id,rank)
);
CREATE INDEX feature_cvterm_idx1 ON feature_cvterm (feature_id);
CREATE INDEX feature_cvterm_idx2 ON feature_cvterm (cvterm_id);

CREATE TABLE featureprop (
  featureprop_id bigint NOT NULL,
  primary key (featureprop_id),
  feature_id bigint NOT NULL,
  foreign key (feature_id) references feature (feature_id) on delete cascade INITIALLY DEFERRED,
  type_id bigint NOT NULL,
  foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
  value text,
  rank integer DEFAULT 0 NOT NULL
  CONSTRAINT featureprop_c1 UNIQUE (feature_id,type_id,rank)
);
CREATE INDEX featureprop_idx1 ON featureprop (feature_id);
CREATE INDEX featureprop_idx2 ON featureprop (type_id);

COMMENT ON TABLE featureprop IS 'A feature can have any number of slot-value property tags attached to it. This is an alternative to hardcoding a list of columns in the relational schema, and is completely extensible.';
