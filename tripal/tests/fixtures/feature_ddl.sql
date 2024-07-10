CREATE TABLE chado.feature (
  feature_id integer NOT NULL DEFAULT nextval('feature_feature_id_seq'::regclass),
  dbxref_id integer NULL,
  organism_id integer NOT NULL,
  name character varying(255) NULL,
  uniquename text NOT NULL,
  residues text NULL,
  seqlen integer NULL,
  md5checksum character(32) NULL,
  type_id integer NOT NULL,
  is_analysis boolean NOT NULL DEFAULT false,
  is_obsolete boolean NOT NULL DEFAULT false,
  timeaccessioned timestamp without time zone NOT NULL DEFAULT now(),
  timelastmodified timestamp without time zone NOT NULL DEFAULT now(),
  CONSTRAINT feature_pkey PRIMARY KEY (feature_id),
  CONSTRAINT feature_c1 UNIQUE (organism_id, uniquename, type_id),
  CONSTRAINT feature_dbxref_id_fkey FOREIGN KEY (dbxref_id) REFERENCES dbxref(dbxref_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED,
  CONSTRAINT feature_organism_id_fkey FOREIGN KEY (organism_id) REFERENCES organism(organism_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED,
  CONSTRAINT feature_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED
);
CREATE UNIQUE INDEX feature_c1 ON chado.feature USING btree (organism_id, uniquename, type_id);
CREATE INDEX feature_c1_audit ON chado.feature USING btree (organism_id, uniquename, type_id);
CREATE INDEX feature_idx1 ON chado.feature USING btree (dbxref_id);
CREATE INDEX feature_idx1_audit ON chado.feature USING btree (dbxref_id);
CREATE INDEX feature_idx2 ON chado.feature USING btree (organism_id);
CREATE INDEX feature_idx2_audit ON chado.feature USING btree (organism_id);
CREATE INDEX feature_idx3 ON chado.feature USING btree (type_id);
CREATE INDEX feature_idx3_audit ON chado.feature USING btree (type_id);
CREATE INDEX feature_idx4 ON chado.feature USING btree (uniquename);
CREATE INDEX feature_idx4_audit ON chado.feature USING btree (uniquename);
CREATE INDEX feature_idx5 ON chado.feature USING btree (lower((name)::text));
CREATE INDEX feature_idx5_audit ON chado.feature USING btree (lower((name)::text));
CREATE INDEX feature_name_ind1 ON chado.feature USING btree (name);
CREATE INDEX feature_name_ind1_audit ON chado.feature USING btree (name);
CREATE INDEX feature_pkey_audit ON chado.feature USING btree (feature_id);
COMMENT ON TABLE chado.feature IS 'A feature is a biological sequence or a
section of a biological sequence, or a collection of such
sections. Examples include genes, exons, transcripts, regulatory
regions, polypeptides, protein domains, chromosome sequences, sequence
variations, cross-genome match regions such as hits and HSPs and so
on; see the Sequence Ontology for more. The combination of
organism_id, uniquename and type_id should be unique.';
