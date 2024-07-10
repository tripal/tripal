<?php
$feature_basic = [
  "columns" => [
    "feature_id" => [
      "type" => "integer",
      "not null" => true,
      "default" => "nextval('feature_feature_id_seq'::regclass)",
    ],
    "dbxref_id" => [
      "type" => "integer",
      "not null" => false,
      "default" => "",
    ],
    "organism_id" => [
      "type" => "integer",
      "not null" => true,
      "default" => "",
    ],
    "name" => [
      "type" => "character varying(255)",
      "not null" => false,
      "default" => "",
    ],
    "uniquename" => [
      "type" => "text",
      "not null" => true,
      "default" => "",
    ],
    "residues" => [
      "type" => "text",
      "not null" => false,
      "default" => "",
    ],
    "seqlen" => [
      "type" => "integer",
      "not null" => false,
      "default" => "",
    ],
    "md5checksum" => [
      "type" => "character(32)",
      "not null" => false,
      "default" => "",
    ],
    "type_id" => [
      "type" => "integer",
      "not null" => true,
      "default" => "",
    ],
    "is_analysis" => [
      "type" => "boolean",
      "not null" => true,
      "default" => "false",
    ],
    "is_obsolete" => [
      "type" => "boolean",
      "not null" => true,
      "default" => "false",
    ],
    "timeaccessioned" => [
      "type" => "timestamp without time zone",
      "not null" => true,
      "default" => "now()",
    ],
    "timelastmodified" => [
      "type" => "timestamp without time zone",
      "not null" => true,
      "default" => "now()",
    ],
  ],
  "constraints" => [
    "feature_pkey" => "PRIMARY KEY (feature_id)",
    "feature_c1" => "UNIQUE (organism_id, uniquename, type_id)",
    "feature_dbxref_id_fkey" => "FOREIGN KEY (dbxref_id) REFERENCES dbxref(dbxref_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED",
    "feature_organism_id_fkey" => "FOREIGN KEY (organism_id) REFERENCES organism(organism_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED",
    "feature_type_id_fkey" => "FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED",
  ],
  "indexes" => [
    "feature_c1" => [
      "query" => "CREATE UNIQUE INDEX feature_c1 ON chado.feature USING btree (organism_id, uniquename, type_id);",
      "name" => "feature_c1",
      "table" => "chado.feature",
      "using" => "btree (organism_id, uniquename, type_id)",
    ],
    "feature_c1_audit" => [
      "query" => "CREATE INDEX feature_c1_audit ON chado.feature USING btree (organism_id, uniquename, type_id);",
      "name" => "feature_c1_audit",
      "table" => "chado.feature",
      "using" => "btree (organism_id, uniquename, type_id)",
    ],
    "feature_idx1" => [
      "query" => "CREATE INDEX feature_idx1 ON chado.feature USING btree (dbxref_id);",
      "name" => "feature_idx1",
      "table" => "chado.feature",
      "using" => "btree (dbxref_id)",
    ],
    "feature_idx1_audit" => [
      "query" => "CREATE INDEX feature_idx1_audit ON chado.feature USING btree (dbxref_id);",
      "name" => "feature_idx1_audit",
      "table" => "chado.feature",
      "using" => "btree (dbxref_id)",
    ],
    "feature_idx2" => [
      "query" => "CREATE INDEX feature_idx2 ON chado.feature USING btree (organism_id);",
      "name" => "feature_idx2",
      "table" => "chado.feature",
      "using" => "btree (organism_id)",
    ],
    "feature_idx2_audit" => [
      "query" => "CREATE INDEX feature_idx2_audit ON chado.feature USING btree (organism_id);",
      "name" => "feature_idx2_audit",
      "table" => "chado.feature",
      "using" => "btree (organism_id)",
    ],
    "feature_idx3" => [
      "query" => "CREATE INDEX feature_idx3 ON chado.feature USING btree (type_id);",
      "name" => "feature_idx3",
      "table" => "chado.feature",
      "using" => "btree (type_id)",
    ],
    "feature_idx3_audit" => [
      "query" => "CREATE INDEX feature_idx3_audit ON chado.feature USING btree (type_id);",
      "name" => "feature_idx3_audit",
      "table" => "chado.feature",
      "using" => "btree (type_id)",
    ],
    "feature_idx4" => [
      "query" => "CREATE INDEX feature_idx4 ON chado.feature USING btree (uniquename);",
      "name" => "feature_idx4",
      "table" => "chado.feature",
      "using" => "btree (uniquename)",
    ],
    "feature_idx4_audit" => [
      "query" => "CREATE INDEX feature_idx4_audit ON chado.feature USING btree (uniquename);",
      "name" => "feature_idx4_audit",
      "table" => "chado.feature",
      "using" => "btree (uniquename)",
    ],
    "feature_idx5" => [
      "query" => "CREATE INDEX feature_idx5 ON chado.feature USING btree (lower((name)::text));",
      "name" => "feature_idx5",
      "table" => "chado.feature",
      "using" => "btree (lower((name)::text))",
    ],
    "feature_idx5_audit" => [
      "query" => "CREATE INDEX feature_idx5_audit ON chado.feature USING btree (lower((name)::text));",
      "name" => "feature_idx5_audit",
      "table" => "chado.feature",
      "using" => "btree (lower((name)::text))",
    ],
    "feature_name_ind1" => [
      "query" => "CREATE INDEX feature_name_ind1 ON chado.feature USING btree (name);",
      "name" => "feature_name_ind1",
      "table" => "chado.feature",
      "using" => "btree (name)",
    ],
    "feature_name_ind1_audit" => [
      "query" => "CREATE INDEX feature_name_ind1_audit ON chado.feature USING btree (name);",
      "name" => "feature_name_ind1_audit",
      "table" => "chado.feature",
      "using" => "btree (name)",
    ],
    "feature_pkey_audit" => [
      "query" => "CREATE INDEX feature_pkey_audit ON chado.feature USING btree (feature_id);",
      "name" => "feature_pkey_audit",
      "table" => "chado.feature",
      "using" => "btree (feature_id)",
    ],
  ],
  "dependencies" => [
    "dbxref" => [
      "dbxref_id" => "dbxref_id",
    ],
    "organism" => [
      "organism_id" => "organism_id",
    ],
    "cvterm" => [
      "type_id" => "cvterm_id",
    ],
  ],
  "comment" => "A feature is a biological sequence or a
section of a biological sequence, or a collection of such
sections. Examples include genes, exons, transcripts, regulatory
regions, polypeptides, protein domains, chromosome sequences, sequence
variations, cross-genome match regions such as hits and HSPs and so
on; see the Sequence Ontology for more. The combination of
organism_id, uniquename and type_id should be unique.",
  ]
;
