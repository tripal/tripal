<?php

function tripal_natural_diversity_chado_nd_reagent_schema () {
  $description = array();
  
  $description = array(
    'table' => 'nd_reagent',
    'fields' => array(
      'nd_reagent_id' => array(
        'type' => 'serial',
        'not null' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
      'feature_id' => array(
        'type' => 'int',
      ),
    ),
    'primary key' => array(
      '0' => 'nd_reagent_id',
    ),
    'foreign keys' => array(
      'feature' => array(
        'table' => 'feature',
        'columns' => array(
          'feature_id' => 'feature_id',
        ),
      ),
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
  );
  
  return $description;
}

function tripal_natural_diversity_chado_nd_reagent_relationship_schema () {
  $description = array();
  
  $description = array(
    'table' => 'nd_reagent_relationship',
    'fields' => array(
      'nd_reagent_relationship_id' => array(
        'type' => 'serial',
        'not null' => '1',
      ),
      'subject_reagent_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
      'object_reagent_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'nd_reagent_relationship_id',
    ),
    'foreign keys' => array(
      'nd_reagent' => array(
        'table' => 'feature',
        'columns' => array(
          'subject_reagent_id' => 'nd_reagent_id',
          'object_reagent_id' => 'nd_reagent_id',
        ),
      ),
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
  );
  
  return $description;
}
