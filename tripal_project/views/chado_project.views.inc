<?php

/**
 *  @file
 *  This file defines the data array for a given chado table. This array
 *  is merged into a larger array containing definitions of all tables associated
 *  with this module in:
 *  @see tripal_project.views.inc --in tripal_project_views_data()
 *
 *  Note: All chado tables are joined to their drupal nodes through the chado_project linking table.
 *        This file simply defines this linking table and joins the three tables together.
 *        No modification of project.views.inc is needed.
 *
 *  Documentation on views integration can be found at
 *  http://views2.logrus.com/doc/html/index.html.
 */

/**
 * Purpose: this function returns the portion of the data array
 *   which describes the chado_project drupal table, it's fields and any joins between it
 *   and other tables
 * @see tripal_project_views_data() --in tripal_project.views.inc
 *
 * The main need for description of this table to views is to join chado data with drupal nodes
 *
 */
function retrieve_chado_project_views_data() {
  global $db_url;
  $data = array();

  // if the chado database is not local to the drupal database
  // then we need to set the database name.  This should always
  // be 'chado'.
  if (is_array($db_url) and array_key_exists('chado', $db_url)) {
     // return empty data array b/c if chado is external then no join to the nodetable can be made
     return $data;
  }

  //Basic table definition-----------------------------------
  $data['chado_project']['table'] = array(
    'field' => 'nid',
  );

  //Relationship Definitions---------------------------------
  // Note: No joins need to be made from $data['project']['table']

  // Join the chado_project table to project
  $data['chado_project']['table']['join']['project'] = array(
    'left_field' => 'project_id',
    'field' => 'project_id',
  );

  // Join the node table to chado_project
  $data['node']['table']['join']['chado_project'] = array(
    'left_field' => 'nid',
    'field' => 'nid',
  );

  // Join the node table to project
  $data['node']['table']['join']['project'] = array(
    'left_table' => 'chado_project',
    'left_field' => 'nid',
    'field' => 'nid',
  );

  // Add relationship between chado_project and project
  $data['chado_project']['project_nid'] = array(
    'group' => 'project',
    'title' => 'Project Node',
    'help' => 'Links Chado project Fields/Data to the Nodes in the current View.',
    'real field' => 'project_id',
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'title' => t('Chado => Project'),
      'label' => t('Chado => Project'),
      'real field' => 'project_id',
      'base' => 'project',
      'base field' => 'project_id'
    ),
  );

  // Add node relationship to project
  $data['chado_project']['project_chado_nid'] = array(
    'group' => 'project',
    'title' => 'Project Node',
    'help' => 'Links Chado project Fields/Data to the Nodes in the current View.',
    'real field' => 'nid',
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'title' => t('Chado => Node'),
      'label' => t('Chado => Node'),
      'real field' => 'nid',
      'base' => 'node',
      'base field' => 'nid'
    ),
  );
  return $data;
}
