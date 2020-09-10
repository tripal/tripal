<?php
namespace Drupal\tripal_chado\TypedData;

use Drupal\Core\TypedData\DataDefinition;
use Drupal\Core\TypedData;

class ChadoOrganismDataDefinition extends ChadoComplexDataDefinition {
  /**
   * Creates a new chado data definition.
   *
   * @param string $type
   *   (optional) The data type of the chado data. Defaults to 'chado'.
   *
   * @return static
   */
  public static function create($type = 'chado_organism') {
    $definition['type'] = $type;
    return new static($definition);
  }


  public function getPropertyDefinitions() {
    // TODO: Change the values in the properties to be CV terms
    if (!isset($this->propertyDefinitions)) {
      $this->propertyDefinitions['scientific_name'] = DataDefinition::create('string')
        ->setLabel('Scientific Name')
        ->setComputed(TRUE)
        // TODO: move these from settings to ->setSearchable(), ->setSearchOperations(), ->setSortable()
        // in a new TripalComplexDataDefinition class.
        ->setSettings([
          'searchable' => TRUE,
          'operations' => ['eq', 'ne', 'contains', 'starts'],
          'sortable' => TRUE
        ])
        ->setReadOnly(TRUE)
        ->setRequired(TRUE);

      $this->propertyDefinitions['genus'] = DataDefinition::create('string')
        ->setLabel('Genus')
        ->setComputed(TRUE)
        ->setSettings([
          'name' => 'genus',
          'searchable' => TRUE,
          'operations' => ['eq', 'ne', 'contains', 'starts'],
          'sortable' => TRUE
        ])
        ->setReadOnly(FALSE)
        ->setRequired(TRUE);

      $this->propertyDefinitions['species'] = DataDefinition::create('string')
        ->setLabel('Species')
        ->setComputed(TRUE)
        ->setSettings([
          'name' => 'species',
          'searchable' => TRUE,
          'operations' => ['eq', 'ne', 'contains', 'starts'],
          'sortable' => TRUE
        ])
        ->setReadOnly(FALSE)
        ->setRequired(TRUE);

      $this->propertyDefinitions['infraspecies'] = DataDefinition::create('string')
        ->setLabel('Infraspecific Name')
        ->setComputed(TRUE)
        ->setSettings([
          'name' => 'infraspecies',
          'searchable' => TRUE,
          'operations' => ['eq', 'ne', 'contains', 'starts'],
          'sortable' => TRUE
        ])
        ->setReadOnly(FALSE)
        ->setRequired(FALSE);

      $this->propertyDefinitions['infraspecific_type'] = DataDefinition::create('integer')
        ->setLabel('Infraspecific Type')
        ->setComputed(TRUE)
        ->setSettings([
          'name' => 'infraspecific_type',
          'searchable' => TRUE,
          'operations' => ['eq', 'ne', 'contains', 'starts'],
          'sortable' => TRUE
        ])
        ->setReadOnly(FALSE)
        ->setRequired(FALSE);
    }
    return $this->propertyDefinitions;
  }
}