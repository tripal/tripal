<?php
/**
 * Add any custom tables needed by this module.
 * - Contactprop: keep track of properties of contact
 *
 * @ingroup tripal_contact
 */
function tripal_chado_add_contactprop_table(){
  $schema = array (
    'table' => 'contactprop',
    'fields' => array (
      'contactprop_id' => array (
        'type' => 'serial',
        'not null' => true,
      ),
      'contact_id' => array (
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'contactprop_id',
    ),
    'unique keys' => array (
      'contactprop_c1' => array (
        0 => 'contact_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'contactprop_idx1' => array (
        0 => 'contact_id',
      ),
      'contactprop_idx2' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'contact' => array (
        'table' => 'contact',
        'columns' => array (
          'contact_id' => 'contact_id',
        ),
      ),
    ),
  );
  chado_create_custom_table('contactprop', $schema, TRUE, NULL, FALSE);
}

/**
 * Adds the featuremap_dbxref table to Chado v1.2.
 */
function tripal_chado_add_featuremap_dbxref_table(){

  // add the featuremap_dbxref table to Chado
  $schema = array (
    'table' => 'featuremap_dbxref',
    'fields' => array (
      'featuremap_dbxref_id' => array (
        'type' => 'serial',
        'not null' => true,
      ),
      'featuremap_id' => array (
        'type' => 'int',
        'not null' => true,
      ),
      'dbxref_id' => array (
        'type' => 'int',
        'not null' => true,
      ),
      'is_current' => array (
        'type' => 'int',
        'size' => 'tiny',
        'not null' => true,
        'default' => 1,
      ),
    ),
    'primary key' => array (
      0 => 'featuremap_dbxref_id',
    ),
    'unique keys' => array (
      'featuremap_dbxref_c1' => array (
        0 => 'featuremap_id',
        1 => 'dbxref_id',
      ),
    ),
    'indexes' => array (
      'featuremap_dbxref_idx1' => array (
        0 => 'featuremap_dbxref_id',
      ),
      'featuremap_dbxref_idx2' => array (
        0 => 'dbxref_id',
      ),
    ),
    'foreign keys' => array (
      'dbxref' => array (
        'table' => 'dbxref',
        'columns' => array (
          'dbxref_id' => 'dbxref_id',
        ),
      ),
      'featuremap' => array (
        'table' => 'featuremap',
        'columns' => array (
          'featuremap_id' => 'featuremap_id',
        ),
      ),
    ),
    'referring_tables' => NULL,
  );

  chado_create_custom_table('featuremap_dbxref', $schema, TRUE, NULL, FALSE);

}

/**
 * Add custom tables needed by the feature map module
 *  - featuremapprop
 *  - featuremap_dbxref
 *  - featureposprop
 *
 * @ingroup tripal_featuremap
 */
function tripal_chado_add_featuremapprop_table(){
  // add the featuremaprop table to Chado
  $schema = array (
    'table' => 'featuremapprop',
    'fields' => array (
      'featuremapprop_id' => array (
        'type' => 'serial',
        'not null' => true,
      ),
      'featuremap_id' => array (
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'featuremapprop_id',
    ),
    'unique keys' => array (
      'featuremapprop_c1' => array (
        0 => 'featuremap_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'featuremapprop_idx1' => array (
        0 => 'featuremap_id',
      ),
      'featuremapprop_idx2' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'featuremap' => array (
        'table' => 'featuremap',
        'columns' => array (
          'featuremap_id' => 'featuremap_id',
        ),
      ),
    ),
  );
  chado_create_custom_table('featuremapprop', $schema, TRUE, NULL, FALSE);
}

/**
 *
 */
function tripal_chado_add_featureposprop_table(){
  $schema = array (
    'table' => 'featureposprop',
    'fields' => array (
      'featureposprop_id' => array (
        'type' => 'serial',
        'not null' => true,
      ),
      'featurepos_id' => array (
        'type' => 'int',
        'not null' => true,
      ),
      'type_id' => array (
        'type' => 'int',
        'not null' => true,
      ),
      'value' => array (
        'type' => 'text',
        'not null' => false,
      ),
      'rank' => array (
        'type' => 'int',
        'not null' => true,
        'default' => 0,
      ),
    ),
    'primary key' => array (
      0 => 'featureposprop_id',
    ),
    'unique keys' => array (
      'featureposprop_id' => array (
        0 => 'featurepos_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array (
      'featureposprop_c1' => array (
        0 => 'featurepos_id',
      ),
      'featureposprop_idx2' => array (
        0 => 'type_id',
      ),
    ),
    'foreign keys' => array (
      'cvterm' => array (
        'table' => 'cvterm',
        'columns' => array (
          'type_id' => 'cvterm_id',
        ),
      ),
      'featurepos' => array (
        'table' => 'featurepos',
        'columns' => array (
          'featurepos_id' => 'featurepos_id',
        ),
      ),
    ),
  );
  chado_create_custom_table('featureposprop', $schema, TRUE, NULL, FALSE);
}


/**
 * Add custom table related to publications
 *  - pubauthor_contact
 *
 * @ingroup tripal_pub
 */
function tripal_chado_add_pubauthor_contact_table() {
  $schema = array (
    'table' => 'pubauthor_contact',
    'fields' => array (
      'pubauthor_contact_id' => array (
        'type' => 'serial',
        'not null' => true,
      ),
      'contact_id' => array (
        'type' => 'int',
        'not null' => true,
      ),
      'pubauthor_id' => array (
        'type' => 'int',
        'not null' => true,
      ),
    ),
    'primary key' => array (
      0 => 'pubauthor_contact_id',
    ),
    'unique keys' => array (
      'pubauthor_contact_c1' => array (
        0 => 'contact_id',
        1 => 'pubauthor_id',
      ),
    ),
    'foreign keys' => array (
      'contact' => array (
        'table' => 'contact',
        'columns' => array (
          'contact_id' => 'contact_id',
        ),
      ),
      'pubauthor' => array (
        'table' => 'pubauthor',
        'columns' => array (
          'pubauthor_id' => 'pubauthor_id',
        ),
      ),
    ),
  );
  chado_create_custom_table('pubauthor_contact', $schema, TRUE, NULL, FALSE);
}