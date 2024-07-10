<?php
$feature_drupal = [
  "description" => "A feature is a biological sequence or a
section of a biological sequence, or a collection of such
sections. Examples include genes, exons, transcripts, regulatory
regions, polypeptides, protein domains, chromosome sequences, sequence
variations, cross-genome match regions such as hits and HSPs and so
on; see the Sequence Ontology for more. The combination of
organism_id, uniquename and type_id should be unique.",
  "fields" => [
    "feature_id" => [
      "type" => "serial",
      "not null" => true,
      "pgsql_type" => "serial",
      "size" => "medium",
    ],
    "dbxref_id" => [
      "type" => "int",
      "not null" => false,
      "pgsql_type" => "integer",
      "size" => "medium",
    ],
    "organism_id" => [
      "type" => "int",
      "not null" => true,
      "pgsql_type" => "integer",
      "size" => "medium",
    ],
    "name" => [
      "type" => "varchar",
      "not null" => false,
      "pgsql_type" => "character varying",
      "length" => 255,
    ],
    "uniquename" => [
      "type" => "text",
      "not null" => true,
      "pgsql_type" => "text",
    ],
    "residues" => [
      "type" => "text",
      "not null" => false,
      "pgsql_type" => "text",
    ],
    "seqlen" => [
      "type" => "int",
      "not null" => false,
      "pgsql_type" => "integer",
      "size" => "medium",
    ],
    "md5checksum" => [
      "type" => "char",
      "not null" => false,
      "pgsql_type" => "character",
      "length" => 32,
    ],
    "type_id" => [
      "type" => "int",
      "not null" => true,
      "pgsql_type" => "integer",
      "size" => "medium",
    ],
    "is_analysis" => [
      "type" => "text",
      "not null" => true,
      "pgsql_type" => "boolean",
      "default" => "false",
    ],
    "is_obsolete" => [
      "type" => "text",
      "not null" => true,
      "pgsql_type" => "boolean",
      "default" => "false",
    ],
    "timeaccessioned" => [
      "type" => "text",
      "not null" => true,
      "pgsql_type" => "timestamp without time zone",
      "default" => "now()",
    ],
    "timelastmodified" => [
      "type" => "text",
      "not null" => true,
      "pgsql_type" => "timestamp without time zone",
      "default" => "now()",
    ],
  ],
  "primary key" => [
    "feature_id",
  ],
  "unique keys" => [
    "feature_c1" => [
      "organism_id",
      "uniquename",
      "type_id",
    ],
  ],
  "foreign keys" => [
    "feature_dbxref_id_fkey" => [
      "table" => "dbxref",
      "columns" => [
        "dbxref_id" => "dbxref_id",
      ],
    ],
    "feature_organism_id_fkey" => [
      "table" => "organism",
      "columns" => [
        "organism_id" => "organism_id",
      ],
    ],
    "feature_type_id_fkey" => [
      "table" => "cvterm",
      "columns" => [
        "type_id" => "cvterm_id",
      ],
    ],
  ],
];
