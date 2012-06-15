<?php

/**
 *  @file
 *  This file defines the data array for a given chado table. This array
 *  is merged into a larger array containing definitions of all tables associated
 *  with this module in:
 *  @see tripal_project.views.inc --in tripal_project_views_data()
 *
 *  Documentation on views integration can be found at
 *  http://views2.logrus.com/doc/html/index.html.
 */

/*************************************************************************
 * Purpose: this function returns the portion of the data array
 *   which describes the project table, it's fields and any joins between it and other tables
 * @see tripal_project_views_data() --in tripal_project.views.inc
 *
 * Table: project
 * @code
 * project-Copy/Paste Table SQL code here-project
 * @endcode
 */
 function retrieve_project_views_data() {
  global $db_url;
  $data = array();

  // if the chado database is not local to the drupal database
  // then we need to set the database name.  This should always
  // be 'chado'.
  if (is_array($db_url) and array_key_exists('chado', $db_url)) {
    $database = 'chado';
  }

  //Basic table definition-----------------------------------
  $data['project']['table']['group'] = t('Chado Project');

  $data['project']['table']['base'] = array(
    'field' => 'project_id',
    'title' => t('Chado Project'),
    'help' => t('Another way of grouping chado content together.'),
  );
  if ($database) {
     $data['project']['table']['database'] = $database;
  }


  //Relationship Definitions---------------------------------
  //Join: YYY => project
  // Notice that this relationship tells the primary table to show it's fields to the
  // table referencing it by a foreign key and thus the relationship is from
  // primary table to table referenceing it (ie: cvterm => feature)
  /**
  $data['project']['table']['join']['YYY'] = array(
    'left_field' => 'foreign key in YYY table',
    'field' => 'primary key in project table',
  );
  */

  //Join: project => XY => YYY
  // This relationship should be described in both directions
  // in the appropriate files (ie: for feature => library
  // describe in both feature.views.inc and library.views.inc)
  /**
  $data['project']['table']['join']['XY'] = array(
    'left_field' => 'matching project key in the XY table',
    'field' => 'primary key in project table',
  );
  $data['project']['table']['join']['YYY'] = array(
    'left_table' => 'XY',
    'left_field' => 'matching project key in the XY table',
    'field' => 'primary key in project table',
  );
  $data['XY']['table']['join']['YYY'] = array(
    'left_field' => 'primary key in YYY table',
    'field' => 'matching YYY key in the XY table',
  );
  */

  //Table Field Definitions----------------------------------

  //Field: project_id (primary key)
  $data['project']['project_id'] = array(
    'title' => t('Project Primary Key'),
    'help' => t('A unique index for every project.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
     ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );

  // Calculated Field: Node ID
  //  use custom field handler to query drupal for the node ID
  //  this is only needed if chado is in a separate database from drupal
  if ($database) {

  }
  else {
    // Add relationship between chado_project and project
    $data['project']['project_nid'] = array(
      'group' => 'project',
      'title' => 'project Node',
      'help' => 'Links Chado project Fields/Data to the Nodes in the current View.',
      'real field' => 'project_id',
      'relationship' => array(
        'handler' => 'views_handler_relationship',
        'title' => t('project => Chado'),
        'label' => t('project => Chado'),
        'real field' => 'project_id',
        'base' => 'chado_project',
        'base field' => 'project_id'
      ),
    );

  }
  //Field: name (varchar -255)
  $data['project']['name'] = array(
    'title' => t('Name'),
    'help' => t('The name of the project.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
     ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );
  // if joined to the node table add a "Link to Node" option for the field
  if (!$database) {
    $data['project']['name']['field']['handler'] = 'views_handler_field_node_optional';
  }

  //Field: description (varchar -255)
  $data['project']['description'] = array(
    'title' => t('Description'),
    'help' => t('A short description of the project'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
     ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  /*.......................................................
   * Beginning of Example Field definitions
   * Remove this section when done

  //Field: plain_text_field (chado datatype)
  $data['project']['plain_text_field'] = array(
    'title' => t('Human-Readable Name'),
    'help' => t('Description of this field.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
     ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  //Field: numeric_field (chado datatype)
  $data['project']['numeric_field'] = array(
    'title' => t('Human-Readable Name'),
    'help' => t('Description of this field.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
     ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );

  //Field: boolean_field (chado datatype)
  $data['project']['boolean_field'] = array(
    'title' => t('Human-Readable Name'),
    'help' => t('Description of this field.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
     ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
    ),
  );

  //Field: unix_timestamp (chado datatype)
  $data['project']['unix_timestamp'] = array(
    'title' => t('Human-Readable Name'),
    'help' => t('Description of this field.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
     ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  //Field: human_readable_date (chado datatype)
  $data['project']['human_readable_date'] = array(
    'title' => t('Human-Readable Name'),
    'help' => t('Description of this field.'),
    'field' => array(
      'handler' => 'views_handler_field_readble_date',
      'click sortable' => TRUE,
     ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
  );

    * End of Example Field definitions
    */

  return $data;
}
