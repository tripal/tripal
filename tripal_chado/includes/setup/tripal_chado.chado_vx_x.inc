<?php
function tripal_chado_add_tripal_gff_temp_table() {
  $schema = array(
    'table' => 'tripal_gff_temp',
    'fields' => array(
      'feature_id' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
      'organism_id' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
      'uniquename' => array(
        'type' => 'text',
        'not null' => TRUE,
      ),
      'type_name' => array(
        'type' => 'varchar',
        'length' => '1024',
        'not null' => TRUE,
      ),
    ),
    'indexes' => array(
      'tripal_gff_temp_idx0' => array('feature_id'),
      'tripal_gff_temp_idx0' => array('organism_id'),
      'tripal_gff_temp_idx1' => array('uniquename'),
    ),
    'unique keys' => array(
      'tripal_gff_temp_uq0' => array('feature_id'),
      'tripal_gff_temp_uq1' => array('uniquename', 'organism_id', 'type_name'),
    ),
  );
  chado_create_custom_table('tripal_gff_temp', $schema, TRUE, NULL, FALSE);
}

/**
 *
 */
function tripal_chado_add_tripal_gffcds_temp_table() {
  $schema = array(
    'table' => 'tripal_gffcds_temp',
    'fields' => array(
      'feature_id' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
      'parent_id' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
      'phase' => array(
        'type' => 'int',
        'not null' => FALSE,
      ),
      'strand' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
      'fmin' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
      'fmax' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
    ),
    'indexes' => array(
      'tripal_gff_temp_idx0' => array('feature_id'),
      'tripal_gff_temp_idx0' => array('parent_id'),
    ),
  );
  chado_create_custom_table('tripal_gffcds_temp', $schema, TRUE, NULL, FALSE);
}

/**
 *
 */
function tripal_chado_add_tripal_gffprotein_temp_table() {
  $schema = array(
    'table' => 'tripal_gffprotein_temp',
    'fields' => array(
      'feature_id' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
      'parent_id' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
      'fmin' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
      'fmax' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
    ),
    'indexes' => array(
      'tripal_gff_temp_idx0' => array('feature_id'),
      'tripal_gff_temp_idx0' => array('parent_id'),
    ),
    'unique keys' => array(
      'tripal_gff_temp_uq0' => array('feature_id'),
    ),
  );
  chado_create_custom_table('tripal_gffprotein_temp', $schema, TRUE, NULL, FALSE);
}

/**
 * Creates a temporary table to store obo details while loading an obo file
 *
 */
function tripal_chado_add_tripal_obo_temp_table() {
  // the tripal_obo_temp table is used for temporary housing of records when loading OBO files
  // we create it here using plain SQL because we want it to be in the chado schema but we
  // do not want to use the Tripal Custom Table API because we don't want it to appear in the
  // list of custom tables.  It needs to be available for the Tripal Chado API so we create it
  // here and then define it in the tripal_cv/api/tripal_cv.schema.api.inc
  if (!chado_table_exists('tripal_obo_temp')) {
    $sql = "
      CREATE TABLE {tripal_obo_temp} (
        id character varying(255) NOT NULL,
        stanza text NOT NULL,
        type character varying(50) NOT NULL,
        CONSTRAINT tripal_obo_temp_uq0 UNIQUE (id)
      );
    ";
    chado_query($sql);
    $sql = "CREATE INDEX tripal_obo_temp_idx0 ON {tripal_obo_temp} USING btree (id)";
    chado_query($sql);
    $sql = "CREATE INDEX tripal_obo_temp_idx1 ON {tripal_obo_temp} USING btree (type)";
    chado_query($sql);
  }
}



/**
 * Creates a materialized view that stores the type & number of stocks per organism
 *
 * @ingroup tripal_stock
 */
function tripal_chado_add_organism_stock_count_mview() {
  $view_name = 'organism_stock_count';
  $comment = 'Stores the type and number of stocks per organism';

  $schema = array(
    'description' => $comment,
    'table' => $view_name,
    'fields' => array(
      'organism_id' => array(
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'genus' => array(
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ),
      'species' => array(
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ),
      'common_name' => array(
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
      'num_stocks' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
      'cvterm_id' => array(
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'stock_type' => array(
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ),
    ),
    'indexes' => array(
      'organism_stock_count_idx1' => array('organism_id'),
      'organism_stock_count_idx2' => array('cvterm_id'),
      'organism_stock_count_idx3' => array('stock_type'),
    ),
  );

  $sql = "
    SELECT
        O.organism_id, O.genus, O.species, O.common_name,
        count(S.stock_id) as num_stocks,
        CVT.cvterm_id, CVT.name as stock_type
     FROM organism O
        INNER JOIN stock S  ON O.Organism_id = S.organism_id
        INNER JOIN cvterm CVT ON S.type_id     = CVT.cvterm_id
     GROUP BY
        O.Organism_id, O.genus, O.species, O.common_name, CVT.cvterm_id, CVT.name
  ";

  tripal_add_mview($view_name, 'tripal_stock', $schema, $sql, $comment, FALSE);
}




/**
 * Adds a materialized view keeping track of the type of features associated with each library
 *
 * @ingroup tripal_library
 */
function tripal_chado_add_library_feature_count_mview(){
  $view_name = 'library_feature_count';
  $comment = 'Provides count of feature by type that are associated with all libraries';

  $schema = array(
    'table' => $view_name,
    'description' => $comment,
    'fields' => array(
      'library_id' => array(
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'name' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
      ),
      'num_features' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
      'feature_type' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
      ),
    ),
    'indexes' => array(
      'library_feature_count_idx1' => array('library_id'),
    ),
  );

  $sql = "
    SELECT
      L.library_id, L.name,
      count(F.feature_id) as num_features,
      CVT.name as feature_type
    FROM library L
      INNER JOIN library_feature LF  ON LF.library_id = L.library_id
      INNER JOIN feature F           ON LF.feature_id = F.feature_id
      INNER JOIN cvterm CVT          ON F.type_id     = CVT.cvterm_id
    GROUP BY L.library_id, L.name, CVT.name
  ";

  tripal_add_mview($view_name, 'tripal_library', $schema, $sql, $comment, FALSE);
}






/**
 *
 */





/**
 * Creates a materialized view that stores the type & number of features per organism
 *
 * @ingroup tripal_feature
 */
function tripal_chado_add_organism_feature_count_mview() {
  $view_name = 'organism_feature_count';
  $comment = 'Stores the type and number of features per organism';

  $schema = array(
    'description' => $comment,
    'table' => $view_name,
    'fields' => array(
      'organism_id' => array(
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'genus' => array(
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ),
      'species' => array(
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ),
      'common_name' => array(
        'type' => 'varchar',
        'length' => '255',
        'not null' => FALSE,
      ),
      'num_features' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
      'cvterm_id' => array(
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'feature_type' => array(
        'type' => 'varchar',
        'length' => '255',
        'not null' => TRUE,
      ),
    ),
    'indexes' => array(
      'organism_feature_count_idx1' => array('organism_id'),
      'organism_feature_count_idx2' => array('cvterm_id'),
      'organism_feature_count_idx3' => array('feature_type'),
    ),
  );

  $sql = "
    SELECT
        O.organism_id, O.genus, O.species, O.common_name,
        count(F.feature_id) as num_features,
        CVT.cvterm_id, CVT.name as feature_type
     FROM organism O
        INNER JOIN feature F  ON O.Organism_id = F.organism_id
        INNER JOIN cvterm CVT ON F.type_id     = CVT.cvterm_id
     GROUP BY
        O.Organism_id, O.genus, O.species, O.common_name, CVT.cvterm_id, CVT.name
  ";

  tripal_add_mview($view_name, 'tripal_feature', $schema, $sql, $comment, FALSE);
}


/**
 * Creates a view showing the link between an organism & it's analysis through associated features.
 *
 */
function tripal_chado_add_analysis_organism_mview() {
  $view_name = 'analysis_organism';
  $comment = t('This view is for associating an organism (via it\'s associated features) to an analysis.');

  // this is the SQL used to identify the organism to which an analsysis
  // has been used.  This is obtained though the analysisfeature -> feature -> organism
  // joins
  $sql = "
    SELECT DISTINCT A.analysis_id, O.organism_id
    FROM analysis A
      INNER JOIN analysisfeature AF ON A.analysis_id = AF.analysis_id
      INNER JOIN feature F          ON AF.feature_id = F.feature_id
      INNER JOIN organism O         ON O.organism_id = F.organism_id
  ";

  // the schema array for describing this view
  $schema = array(
    'table' => $view_name,
    'description' => $comment,
    'fields' => array(
      'analysis_id' => array(
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'organism_id' => array(
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ),
    ),
    'indexes' => array(
      'networkmod_qtl_indx0' => array('analysis_id'),
      'networkmod_qtl_indx1' => array('organism_id'),
    ),
    'foreign keys' => array(
      'analysis' => array(
        'table' => 'analysis',
        'columns' => array(
          'analysis_id' => 'analysis_id',
        ),
      ),
      'organism' => array(
        'table' => 'organism',
        'columns' => array(
          'organism_id' => 'organism_id',
        ),
      ),
    ),
  );

  // add the view
  tripal_add_mview($view_name, 'tripal_analysis', $schema, $sql, $comment, FALSE);
}

/**
 * Add a materialized view that maps cv to db records.
 *
 * This is needed for viewing cv trees
 *
 */
function tripal_chado_add_db2cv_mview_mview() {
  $mv_name = 'db2cv_mview';
  $comment = 'A table for quick lookup of the vocabularies and the databases they are associated with.';
  $schema = array(
    'table' => $mv_name,
    'description' => $comment,
    'fields' => array (
      'cv_id' => array (
        'type' => 'int',
        'not null' => true,
      ),
      'cvname' => array (
        'type' => 'varchar',
        'length' => '255',
        'not null' => true,
      ),
      'db_id' => array (
        'type' => 'int',
        'not null' => true,
      ),
      'dbname' => array (
        'type' => 'varchar',
        'length' => '255',
        'not null' => true,
      ),
      'num_terms' => array (
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'indexes' => array (
      'cv_id_idx'  => array ('cv_id'),
      'cvname_idx'    => array ('cvname'),
      'db_id_idx' => array ('db_id'),
      'dbname_idx'    => array ('db_id'),
    ),
  );

  $sql = "
   SELECT DISTINCT CV.cv_id, CV.name as cvname, DB.db_id, DB.name as dbname,
     COUNT(CVT.cvterm_id) as num_terms
   FROM cv CV
     INNER JOIN cvterm CVT on CVT.cv_id = CV.cv_id
     INNER JOIN dbxref DBX on DBX.dbxref_id = CVT.dbxref_id
     INNER JOIN db DB on DB.db_id = DBX.db_id
   GROUP BY CV.cv_id, CV.name, DB.db_id, DB.name
   ORDER BY DB.name
  ";

  // Create the MView
  tripal_add_mview($mv_name, 'tripal_chado', $schema, $sql, $comment, FALSE);
}
/**
 * Add a materialized view of root terms for all chado cvs.
 *
 * This is needed for viewing cv trees
 *
 */
function tripal_chado_add_cv_root_mview_mview() {
  $mv_name = 'cv_root_mview';
  $comment = 'A list of the root terms for all controlled vocabularies. This is needed for viewing CV trees';
  $schema = array(
    'table' => $mv_name,
    'description' => $comment,
    'fields' => array(
      'name' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
      ),
      'cvterm_id' => array(
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'cv_id' => array(
        'size' => 'big',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'cv_name' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
      ),
    ),
    'indexes' => array(
      'cv_root_mview_indx1' => array('cvterm_id'),
      'cv_root_mview_indx2' => array('cv_id'),
    ),
  );

  $sql = "
    SELECT DISTINCT CVT.name,CVT.cvterm_id, CV.cv_id, CV.name
    FROM cvterm_relationship CVTR
      INNER JOIN cvterm CVT on CVTR.object_id = CVT.cvterm_id
      INNER JOIN cv CV on CV.cv_id = CVT.cv_id
    WHERE CVTR.object_id not in
      (SELECT subject_id FROM cvterm_relationship)
  ";

  // Create the MView
  tripal_add_mview($mv_name, 'tripal_chado', $schema, $sql, $comment, FALSE);
}

