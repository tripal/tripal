<?php

/****************************************************************************
 * @section Chado Table Descriptions
 ****************************************************************************/
 
 /**
 * Implements hook_chado_contact_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the contact table
 */
function tripal_stock_chado_contact_schema() {
  $description = array();

  $description['foreign keys']['cvterm'] = array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
  );

  return $description;
}

 /**
 * Implements hook_chado_contact_relationship_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the contact_relationship table
 */
function tripal_stock_chado_contact_relationship_schema() {
  $description = array();

  $description['foreign keys']['cvterm'] = array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
  );

  $description['foreign keys']['contact'] = array(
        'table' => 'contact',
        'columns' => array(
          'subject_id' => 'contact_id',
          'object_id' => 'contact_id',
        ),
  );
  
  return $description;
}
