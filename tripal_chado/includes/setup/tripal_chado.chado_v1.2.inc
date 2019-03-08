<?php
/**
 * Add any custom tables needed by this module.
 * - Contactprop: keep track of properties of contact
 *
 * @ingroup tripal_contact
 */
function tripal_chado_add_contactprop_table() {
  $schema = [
    'table' => 'contactprop',
    'fields' => [
      'contactprop_id' => [
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'contact_id' => [
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'primary key' => [
      0 => 'contactprop_id',
    ],
    'unique keys' => [
      'contactprop_c1' => [
        0 => 'contact_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'contactprop_idx1' => [
        0 => 'contact_id',
      ],
      'contactprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'contact' => [
        'table' => 'contact',
        'columns' => [
          'contact_id' => 'contact_id',
        ],
      ],
    ],
  ];
  chado_create_custom_table('contactprop', $schema, TRUE, NULL, FALSE);
}

/**
 * Adds the featuremap_dbxref table to Chado v1.2.
 */
function tripal_chado_add_featuremap_dbxref_table() {

  // add the featuremap_dbxref table to Chado
  $schema = [
    'table' => 'featuremap_dbxref',
    'fields' => [
      'featuremap_dbxref_id' => [
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'featuremap_id' => [
        'type' => 'int',
        'not null' => TRUE,
      ],
      'dbxref_id' => [
        'type' => 'int',
        'not null' => TRUE,
      ],
      'is_current' => [
        'type' => 'int',
        'size' => 'tiny',
        'not null' => TRUE,
        'default' => 1,
      ],
    ],
    'primary key' => [
      0 => 'featuremap_dbxref_id',
    ],
    'unique keys' => [
      'featuremap_dbxref_c1' => [
        0 => 'featuremap_id',
        1 => 'dbxref_id',
      ],
    ],
    'indexes' => [
      'featuremap_dbxref_idx1' => [
        0 => 'featuremap_dbxref_id',
      ],
      'featuremap_dbxref_idx2' => [
        0 => 'dbxref_id',
      ],
    ],
    'foreign keys' => [
      'dbxref' => [
        'table' => 'dbxref',
        'columns' => [
          'dbxref_id' => 'dbxref_id',
        ],
      ],
      'featuremap' => [
        'table' => 'featuremap',
        'columns' => [
          'featuremap_id' => 'featuremap_id',
        ],
      ],
    ],
    'referring_tables' => NULL,
  ];

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
function tripal_chado_add_featuremapprop_table() {
  // add the featuremaprop table to Chado
  $schema = [
    'table' => 'featuremapprop',
    'fields' => [
      'featuremapprop_id' => [
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'featuremap_id' => [
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'primary key' => [
      0 => 'featuremapprop_id',
    ],
    'unique keys' => [
      'featuremapprop_c1' => [
        0 => 'featuremap_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'featuremapprop_idx1' => [
        0 => 'featuremap_id',
      ],
      'featuremapprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'featuremap' => [
        'table' => 'featuremap',
        'columns' => [
          'featuremap_id' => 'featuremap_id',
        ],
      ],
    ],
  ];
  chado_create_custom_table('featuremapprop', $schema, TRUE, NULL, FALSE);
}

/**
 *
 */
function tripal_chado_add_featureposprop_table() {
  $schema = [
    'table' => 'featureposprop',
    'fields' => [
      'featureposprop_id' => [
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'featurepos_id' => [
        'type' => 'int',
        'not null' => TRUE,
      ],
      'type_id' => [
        'type' => 'int',
        'not null' => TRUE,
      ],
      'value' => [
        'type' => 'text',
        'not null' => FALSE,
      ],
      'rank' => [
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ],
    'primary key' => [
      0 => 'featureposprop_id',
    ],
    'unique keys' => [
      'featureposprop_id' => [
        0 => 'featurepos_id',
        1 => 'type_id',
        2 => 'rank',
      ],
    ],
    'indexes' => [
      'featureposprop_c1' => [
        0 => 'featurepos_id',
      ],
      'featureposprop_idx2' => [
        0 => 'type_id',
      ],
    ],
    'foreign keys' => [
      'cvterm' => [
        'table' => 'cvterm',
        'columns' => [
          'type_id' => 'cvterm_id',
        ],
      ],
      'featurepos' => [
        'table' => 'featurepos',
        'columns' => [
          'featurepos_id' => 'featurepos_id',
        ],
      ],
    ],
  ];
  chado_create_custom_table('featureposprop', $schema, TRUE, NULL, FALSE);
}


/**
 * Add custom table related to publications
 *  - pubauthor_contact
 *
 * @ingroup tripal_pub
 */
function tripal_chado_add_pubauthor_contact_table() {
  $schema = [
    'table' => 'pubauthor_contact',
    'fields' => [
      'pubauthor_contact_id' => [
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'contact_id' => [
        'type' => 'int',
        'not null' => TRUE,
      ],
      'pubauthor_id' => [
        'type' => 'int',
        'not null' => TRUE,
      ],
    ],
    'primary key' => [
      0 => 'pubauthor_contact_id',
    ],
    'unique keys' => [
      'pubauthor_contact_c1' => [
        0 => 'contact_id',
        1 => 'pubauthor_id',
      ],
    ],
    'foreign keys' => [
      'contact' => [
        'table' => 'contact',
        'columns' => [
          'contact_id' => 'contact_id',
        ],
      ],
      'pubauthor' => [
        'table' => 'pubauthor',
        'columns' => [
          'pubauthor_id' => 'pubauthor_id',
        ],
      ],
    ],
  ];
  chado_create_custom_table('pubauthor_contact', $schema, TRUE, NULL, FALSE);
}