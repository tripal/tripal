<?php

function tripal_natural_diversity_chado_nd_experiment_schema () {
  $description = array();
  
  $description = array(
    'table' => 'nd_experiment',
    'fields' => array(
      'nd_experiment_id' => array(
        'type' => 'serial',
        'not null' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
      'nd_geolocation_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'nd_experiment_id',
    ),
    'foreign keys' => array(
      'nd_geolocation' => array(
        'table' => 'nd_geolocation',
        'columns' => array(
          'nd_geolocation_id' => 'nd_geolocation_id',
        ),
      ),
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
    'referring_tables' => array(
      'nd_experiment_contact',
      'nd_experiment_dbxref',
      'nd_experiment_genotype',
      'nd_experiment_phenotype',
      'nd_experiment_project',
      'nd_experiment_protocol',
      'nd_experiment_pub',
      'nd_experiment_stock',
      'nd_experimentprop'
    ),
  );
  
  return $description;
}

function tripal_natural_diversity_chado_nd_experiment_contact_schema () {
  $description = array();
  
  $description = array(
    'table' => 'nd_experiment_contact',
    'fields' => array(
      'nd_experiment_contact_id' => array(
        'type' => 'serial',
        'not null' => '1',
      ),
      'contact_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
      'nd_experiment_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'nd_experiment_contact_id',
    ),
    'foreign keys' => array(
      'nd_experiment' => array(
        'table' => 'nd_experiment',
        'columns' => array(
          'nd_experiment_id' => 'nd_experiment_id',
        ),
      ),
      'contact' => array(
        'table' => 'contact',
        'columns' => array(
          'contact_id' => 'contact_id',
        ),
      ),
    ),
    'referring_tables' => array(

    ),
  );
  
  return $description;
}

function tripal_natural_diversity_chado_nd_experiment_dbxref_schema () {
  $description = array();
  
  $description = array(
    'table' => 'nd_experiment_dbxref',
    'fields' => array(
      'nd_experiment_dbxref_id' => array(
        'type' => 'serial',
        'not null' => '1',
      ),
      'dbxref_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
      'nd_experiment_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'nd_experiment_dbxref_id',
    ),
    'foreign keys' => array(
      'nd_experiment' => array(
        'table' => 'nd_experiment',
        'columns' => array(
          'nd_experiment_id' => 'nd_experiment_id',
        ),
      ),
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
    ),
    'referring_tables' => array(

    ),
  );
  
  return $description;
}

function tripal_natural_diversity_chado_nd_experiment_genotype_schema () {
  $description = array();
  
  $description = array(
    'table' => 'nd_experiment_genotype',
    'fields' => array(
      'nd_experiment_genotype_id' => array(
        'type' => 'serial',
        'not null' => '1',
      ),
      'genotype_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
      'nd_experiment_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'nd_experiment_genotype_id',
    ),
    'foreign keys' => array(
      'nd_experiment' => array(
        'table' => 'nd_experiment',
        'columns' => array(
          'nd_experiment_id' => 'nd_experiment_id',
        ),
      ),
      'genotype' => array(
        'table' => 'genotype',
        'columns' => array(
          'genotype_id' => 'genotype_id',
        ),
      ),
    ),
    'referring_tables' => array(

    ),
  );
  
  return $description;
}

function tripal_natural_diversity_chado_nd_experiment_phenotype_schema () {
  $description = array();
  
  $description = array(
    'table' => 'nd_experiment_phenotype',
    'fields' => array(
      'nd_experiment_phenotype_id' => array(
        'type' => 'serial',
        'not null' => '1',
      ),
      'phenotype_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
      'nd_experiment_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'nd_experiment_phenotype_id',
    ),
    'foreign keys' => array(
      'nd_experiment' => array(
        'table' => 'nd_experiment',
        'columns' => array(
          'nd_experiment_id' => 'nd_experiment_id',
        ),
      ),
      'phenotype' => array(
        'table' => 'phenotype',
        'columns' => array(
          'phenotype_id' => 'phenotype_id',
        ),
      ),
    ),
    'referring_tables' => array(

    ),
  );
  
  return $description;
}

function tripal_natural_diversity_chado_nd_experiment_project_schema () {
  $description = array();
  
  $description = array(
    'table' => 'nd_experiment_project',
    'fields' => array(
      'nd_experiment_project_id' => array(
        'type' => 'serial',
        'not null' => '1',
      ),
      'project_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
      'nd_experiment_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'nd_experiment_project_id',
    ),
    'foreign keys' => array(
      'nd_experiment' => array(
        'table' => 'nd_experiment',
        'columns' => array(
          'nd_experiment_id' => 'nd_experiment_id',
        ),
      ),
      'project' => array(
        'table' => 'project',
        'columns' => array(
          'project_id' => 'project_id',
        ),
      ),
    ),
    'referring_tables' => array(

    ),
  );
  
  return $description;
}

function tripal_natural_diversity_chado_nd_experiment_protocol_schema () {
  $description = array();
  
  $description = array(
    'table' => 'nd_experiment_protocol',
    'fields' => array(
      'nd_experiment_protocol_id' => array(
        'type' => 'serial',
        'not null' => '1',
      ),
      'protocol_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
      'nd_experiment_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'nd_experiment_protocol_id',
    ),
    'foreign keys' => array(
      'nd_experiment' => array(
        'table' => 'nd_experiment',
        'columns' => array(
          'nd_experiment_id' => 'nd_experiment_id',
        ),
      ),
      'protocol' => array(
        'table' => 'protocol',
        'columns' => array(
          'protocol_id' => 'protocol_id',
        ),
      ),
    ),
    'referring_tables' => array(

    ),
  );
  
  return $description;
}

function tripal_natural_diversity_chado_nd_experiment_pub_schema () {
  $description = array();
  
  $description = array(
    'table' => 'nd_experiment_pub',
    'fields' => array(
      'nd_experiment_pub_id' => array(
        'type' => 'serial',
        'not null' => '1',
      ),
      'pub_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
      'nd_experiment_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'nd_experiment_pub_id',
    ),
    'foreign keys' => array(
      'nd_experiment' => array(
        'table' => 'nd_experiment',
        'columns' => array(
          'nd_experiment_id' => 'nd_experiment_id',
        ),
      ),
      'pub' => array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
      ),
    ),
    'referring_tables' => array(

    ),
  );
  
  return $description;
}

function tripal_natural_diversity_chado_nd_experiment_stock_schema () {
  $description = array();
  
  $description = array(
    'table' => 'nd_experiment_stock',
    'fields' => array(
      'nd_experiment_stock_id' => array(
        'type' => 'serial',
        'not null' => '1',
      ),
      'stock_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
      'nd_experiment_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'nd_experiment_stock_id',
    ),
    'foreign keys' => array(
      'nd_experiment' => array(
        'table' => 'nd_experiment',
        'columns' => array(
          'nd_experiment_id' => 'nd_experiment_id',
        ),
      ),
      'stock' => array(
        'table' => 'stock',
        'columns' => array(
          'stock_id' => 'stock_id',
        ),
      ),
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
    'referring_tables' => array(
      'nd_experiment_stock_dbxref',
      'nd_experiment_stockprop',
    ),
  );
  
  return $description;
}

function tripal_natural_diversity_chado_nd_experiment_stockprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'nd_experiment_stockprop',
    'fields' => array(
      'nd_experiment_stockprop_id' => array(
        'type' => 'serial',
        'not null' => '1',
      ),
      'nd_experiment_stock_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
        'type' => 'varchar',
      ),
      'rank' => array(
        'type' => 'int',
        'not null' => 1,
        'default' => 1
      ),
    ),
    'primary key' => array(
      '0' => 'nd_experiment_stockprop_id',
    ),
    'foreign keys' => array(
      'nd_experiment_stock' => array(
        'table' => 'nd_experiment_stock',
        'columns' => array(
          'nd_experiment_stock_id' => 'nd_experiment_stock_id',
        ),
      ),
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
    'referring_tables' => array(
    ),
  );
  
  return $description;
}

function tripal_natural_diversity_chado_nd_experiment_stock_dbxref_schema () {
  $description = array();
  
  $description = array(
    'table' => 'nd_experiment_stock_dbxref',
    'fields' => array(
      'nd_experiment_stock_dbxref_id' => array(
        'type' => 'serial',
        'not null' => '1',
      ),
      'nd_experiment_stock_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
      'dbxref_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'nd_experiment_stock_dbxref_id',
    ),
    'foreign keys' => array(
      'nd_experiment_stock' => array(
        'table' => 'nd_experiment_stock',
        'columns' => array(
          'nd_experiment_stock_id' => 'nd_experiment_stock_id',
        ),
      ),
      'dbxref' => array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
      ),
    ),
    'referring_tables' => array(

    ),
  );
  
  return $description;
}

function tripal_natural_diversity_chado_nd_experimentprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'nd_experimentprop',
    'fields' => array(
      'nd_experimentprop_id' => array(
        'type' => 'serial',
        'not null' => '1',
      ),
      'nd_experiment_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
        'type' => 'varchar',
      ),
      'rank' => array(
        'type' => 'int',
        'not null' => 1,
        'default' => 1
      ),
    ),
    'primary key' => array(
      '0' => 'nd_experimentprop_id',
    ),
    'foreign keys' => array(
      'nd_experiment' => array(
        'table' => 'nd_experiment',
        'columns' => array(
          'nd_experiment_id' => 'nd_experiment_id',
        ),
      ),
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
    'referring_tables' => array(
    ),
  );
  
  return $description;
}

function tripal_natural_diversity_chado_nd_geolocation_schema () {
  $description = array();
  
  $description = array(
    'table' => 'nd_reagent',
    'fields' => array(
      'nd_geolocation_id' => array(
        'type' => 'serial',
        'not null' => '1',
      ),
      'description' => array(
        'type' => 'varchar'
      ),
      'latitude' => array(
        'type' => 'real',
      ),
      'longitude' => array(
        'type' => 'real',
      ),
      'altitude' => array(
        'type' => 'real',
      ),
      'geodetic_datum' => array(
        'type' => 'varchar',
      ),
    ),
    'primary key' => array(
      '0' => 'nd_geolocation_id',
    ),
    'referring_tables' => array(
      'nd_experiment',
      'nd_geolocationprop'
    ),
  );
  
  return $description;
}

function tripal_natural_diversity_chado_nd_geolocationprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'nd_geolocationprop',
    'fields' => array(
      'nd_geolocationprop_id' => array(
        'type' => 'serial',
        'not null' => '1',
      ),
      'nd_geolocation_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
        'type' => 'varchar',
      ),
      'rank' => array(
        'type' => 'int',
        'not null' => 1,
        'default' => 1
      ),
    ),
    'primary key' => array(
      '0' => 'nd_geolocationprop_id',
    ),
    'foreign keys' => array(
      'nd_geolocation' => array(
        'table' => 'nd_geolocation',
        'columns' => array(
          'nd_geolocation_id' => 'nd_geolocation_id',
        ),
      ),
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
    'referring_tables' => array(
    ),
  );
  
  return $description;
}

function tripal_natural_diversity_chado_nd_protocol_schema () {
  $description = array();
  
  $description = array(
    'table' => 'nd_protocol',
    'fields' => array(
      'nd_protocol_id' => array(
        'type' => 'serial',
        'not null' => 1,
      ),
      'name' => array(
        'type' => 'varchar',
        'not null' => 1,
      ),
    ),
    'primary key' => array(
     '0' => 'nd_protocol_id',
    ),
    'referring_tables' => array(
      'nd_experiment_protocol',
      'nd_protocol_reagent',
      'nd_protocolprop'  
    ),
  );
  
  return $description;
}

function tripal_natural_diversity_chado_nd_protocol_reagent_schema () {
  $description = array();
  
  $description = array(
    'table' => 'nd_protocol_reagent',
    'fields' => array(
      'nd_protocol_reagent_id' => array(
        'type' => 'serial',
        'not null' => 1,
      ),    
      'nd_protocol_id' => array(
        'type' => 'int',
        'not null' => 1,
      ),
      'nd_reagent_id' => array(
        'type' => 'int',
        'not null' => 1,
      ),
      'type_id' => array(
        'type' => 'int',
        'not null' => 1,
      ),
    ),
    'primary key' => array(
      '0' => 'nd_protocol_reagent_id',   
    ),
    'foreign keys' => array(
      'nd_protocol' => array(
        'table' => 'nd_protocol',
        'columns' => array(
          'nd_protocol_id' => 'nd_protocol_id',
        ),
      ),
      'nd_reagent' => array(
        'table' => 'nd_reagent',
        'columns' => array(
          'nd_reagent_id' => 'nd_reagent_id',
        ),
      ),
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
    'referring_tables' => array(
    
    ),
  );
  
  return $description;
}

function tripal_natural_diversity_chado_nd_protocolprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'nd_protocolprop',
    'fields' => array(
      'nd_protocolprop_id' => array(
        'type' => 'serial',
        'not null' => '1',
      ),
      'nd_protocol_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
        'type' => 'varchar',
      ),
      'rank' => array(
        'type' => 'int',
        'not null' => 1,
        'default' => 1
      ),
    ),
    'primary key' => array(
      '0' => 'nd_protocolprop_id',
    ),
    'foreign keys' => array(
      'nd_protocol' => array(
        'table' => 'nd_protocol',
        'columns' => array(
          'nd_protocol_id' => 'nd_protocol_id',
        ),
      ),
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
    'referring_tables' => array(
    ),
  );
  
  return $description;
}

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

function tripal_natural_diversity_chado_nd_reagentprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'nd_reagentprop',
    'fields' => array(
      'nd_reagentprop_id' => array(
        'type' => 'serial',
        'not null' => '1',
      ),
      'nd_reagent_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
        'type' => 'varchar',
      ),
      'rank' => array(
        'type' => 'int',
        'not null' => 1,
        'default' => 1
      ),
    ),
    'primary key' => array(
      '0' => 'nd_reagentprop_id',
    ),
    'foreign keys' => array(
      'nd_reagent' => array(
        'table' => 'nd_reagent',
        'columns' => array(
          'nd_reagent_id' => 'nd_reagent_id',
        ),
      ),
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
    ),
    'referring_tables' => array(
    ),
  );
  
  return $description;
}
