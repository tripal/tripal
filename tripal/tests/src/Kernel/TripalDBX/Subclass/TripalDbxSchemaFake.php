<?php

namespace Drupal\Tests\tripal\Kernel\TripalDBX\Subclass;

use Drupal\tripal\TripalDBX\TripalDbxSchema;

/**
 * Fake schema class.
 */
class TripalDbxSchemaFake extends TripalDbxSchema {

  /**
   * {@inheritdoc}
   */
  public function getSchemaDef(array $parameters) :array {
    $format = $parameters['format'] ?? '';
    if ($format == 'SQL') {
      $value = [
"CREATE TABLE testtable (
  id integer NOT NULL DEFAULT nextval('testtable_id_seq'::regclass),
  foreign_id int NULL,
  fieldbigint bigint NULL,
  fieldsmallint smallint NULL,
  fieldbool boolean NOT NULL DEFAULT false,
  fieldreal real NULL DEFAULT 1.0,
  fielddouble double precision NULL DEFAULT NULL,
  fieldchar character varying(255) NULL,
  fieldtext text NOT NULL,
  CONSTRAINT testtable_pkey PRIMARY KEY (id),
  CONSTRAINT testtable_c1 UNIQUE (fieldbigint, fieldsmallint),
  CONSTRAINT foreign_id_fkey FOREIGN KEY (foreign_id) REFERENCES othertable(id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED
);
CREATE UNIQUE INDEX testtable_c2 ON testtable USING btree (fieldbigint, fieldsmallint);
CREATE INDEX testtable_idx1 ON testtable USING btree (foreign_id);
COMMENT ON TABLE testtable IS 'Some long description
on multiple lines.';"
      ];
    }
    elseif ($format == 'Drupal') {
      $value = [
        "description" => "Some long description\non multiple lines.",
        "fields" => [
          "id" => [
            "type" => "text",
            "not null" => true,
            "pgsql_type" => "serial",
          ],
          "foreign_id" => [
            "type" => "text",
            "not null" => false,
            "pgsql_type" => "int",
          ],
          "fieldbigint" => [
            "type" => "int",
            "not null" => false,
            "pgsql_type" => "bigint",
            "size" => "big",
          ],
          "fieldsmallint" => [
            "type" => "int",
            "not null" => false,
            "pgsql_type" => "smallint",
            "size" => "small",
          ],
          "fieldbool" => [
            "type" => "int",
            "not null" => true,
            "pgsql_type" => "boolean",
            "size" => "tiny",
            "default" => "false",
          ],
          "fieldreal" => [
            "type" => "float",
            "not null" => false,
            "pgsql_type" => "real",
            "default" => "1.0",
          ],
          "fielddouble" => [
            "type" => "float",
            "not null" => false,
            "pgsql_type" => "double precision",
            "size" => "big",
            "default" => "NULL",
          ],
          "fieldchar" => [
            "type" => "varchar",
            "not null" => false,
            "pgsql_type" => "character varying",
            "length" => 255,
          ],
          "fieldtext" => [
            "type" => "text",
            "not null" => true,
            "pgsql_type" => "text",
          ],
        ],
        "primary key" => [
          "id",
        ],
        "unique keys" => [
          "testtable_c1" => [
            "fieldbigint",
            "fieldsmallint",
          ],
        ],
        "foreign keys" => [
          "foreign_id_fkey" => [
            "table" => "othertable",
            "columns" => [
              "foreign_id" => "id",
            ],
          ],
        ],
      ];
    }
    else {
      $value = [
        "columns" => [
          "id" => [
            "type" => "integer",
            "not null" => true,
            "default" => "nextval('testtable_id_seq'::regclass)",
          ],
          "foreign_id" => [
            "type" => "int",
            "not null" => false,
            "default" => "",
          ],
          "fieldbigint" => [
            "type" => "bigint",
            "not null" => false,
            "default" => "",
          ],
          "fieldsmallint" => [
            "type" => "smallint",
            "not null" => false,
            "default" => "",
          ],
          "fieldbool" => [
            "type" => "boolean",
            "not null" => true,
            "default" => "false",
          ],
          "fieldreal" => [
            "type" => "real",
            "not null" => false,
            "default" => "1.0",
          ],
          "fielddouble" => [
            "type" => "double precision",
            "not null" => false,
            "default" => "NULL",
          ],
          "fieldchar" => [
            "type" => "character varying(255)",
            "not null" => false,
            "default" => "",
          ],
          "fieldtext" => [
            "type" => "text",
            "not null" => true,
            "default" => "",
          ],
        ],
        "constraints" => [
          "testtable_pkey" => "PRIMARY KEY (id)",
          "testtable_c1" => "UNIQUE (fieldbigint, fieldsmallint)",
          "foreign_id_fkey" => "FOREIGN KEY (foreign_id) REFERENCES othertable(id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED",
        ],
        "indexes" => [
          "testtable_c2" => [
            "query" => "CREATE UNIQUE INDEX testtable_c2 ON testtable USING btree (fieldbigint, fieldsmallint);",
            "name" => "testtable_c2",
            "table" => "testtable",
            "using" => "btree (fieldbigint, fieldsmallint)",
          ],
          "testtable_idx1" => [
            "query" => "CREATE INDEX testtable_idx1 ON testtable USING btree (foreign_id);",
            "name" => "testtable_idx1",
            "table" => "testtable",
            "using" => "btree (foreign_id)",
          ],
        ],
        "dependencies" => [
          "othertable" => [
            "foreign_id" => "id",
          ],
        ],
        "comment" => "Some long description\non multiple lines.",
      ];
    }
    return $value;
  }

  /**
   * Returns defaultSchema protected member value.
   */
  public function getDefaultSchema() {
    return $this->defaultSchema;
  }

}
