<?php
/**
 * Create a legacy custom chado table (analysisfeatureprop) to store properties
 * of analysisfeature links.
 *
 */
function tripal_chado_add_analysisfeatureprop_table() {

  // Create analysisfeatureprop table in chado.  This is needed for Chado
  // version 1.11, the table exists in Chado 1.2.
  if (!db_table_exists('chado.analysisfeatureprop')) {
    $sql = "
      CREATE TABLE {analysisfeatureprop} (
        analysisfeatureprop_id SERIAL PRIMARY KEY,
        analysisfeature_id     INTEGER NOT NULL,
        type_id                INTEGER NOT NULL,
        value                  TEXT,
        rank                   INTEGER NOT NULL,
        CONSTRAINT analysisfeature_id_type_id_rank UNIQUE (analysisfeature_id, type_id, rank),
        CONSTRAINT analysisfeatureprop_analysisfeature_id_fkey FOREIGN KEY (analysisfeature_id) REFERENCES {analysisfeature}(analysisfeature_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED,
        CONSTRAINT analysisfeatureprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES {cvterm}(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED
      )
    ";
    chado_query($sql);
  }
}