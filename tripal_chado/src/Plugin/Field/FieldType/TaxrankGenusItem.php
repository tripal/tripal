<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Field\FieldDefinitionInterface;
use Drupal\Core\Field\FieldItemBase;
use Drupal\Core\Field\FieldStorageDefinitionInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\Core\TypedData\DataDefinition;
use Drupal\Component\Utility\Random;
use Drupal\tripal_chado\Plugin\Field\ChadoFieldItemBase;
use Drupal\tripal_chado\TypedData\ChadoDataDefinition;
use Drupal\tripal_chado\TypedData\ChadoLinkerDataDefinition;

/**
 * Plugin implementation of the 'taxrank__genus' field type.
 *
 * @FieldType(
 *   id = "taxrank__genus",
 *   label = @Translation("genus"),
 *   module = "tripal_chado",
 *   category = @Translation("Tripal: Chado"),
 *   description = @Translation("A taxonomic category ranking below a family (or Subfamily) and above a species and generally consisting of a group of species exhibiting similar characteristics."),
 *   default_widget = "taxrank_genus_default_widget",
 *   default_formatter = "taxrank_genus_default_formatter",
 *   cardinality = 1,
 * )
 */
class TAXRANKGenusItem extends ChadoFieldItemBase {

  /**
   * {@inheritdoc}
   */
  static $propertyDefinitions;

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $settings = parent::defaultFieldSettings();

    // -- Define the Vocabulary.
    // The short name for the vocabulary (e.g. shcema, SO, GO, PATO, etc.).
    $settings['term_vocabulary'] = 'TAXRANK';
    // The full name of the vocabulary.
    $settings['vocab_name'] = 'Taxonomic Rank Vocabulary';
    // The description of the vocabulary.
    $settings['vocab_description'] = 'A vocabulary of taxonomic ranks (species, family, phylum, etc).';

    // -- Define the Vocabulary Term.
    // The name of the term.
    $settings['term_name'] = 'genus';
    // The unique ID (i.e. accession) of the term.
    $settings['term_accession'] = '0000005';
    // The definition of the term.
    $settings['term_definition'] = 'A taxonomic category ranking below a family (or Subfamily) and above a species and generally consisting of a group of species exhibiting similar characteristics.';

    // -- Additional Settings.
    // Set to TRUE if the site admin is not allowed to change the term
    // type, otherwise the admin can change the term mapped to a field.
    $settings['term_fixed'] = TRUE;
    // Set to TRUE if the field should be automatically attached to an entity
    // when it is loaded. Otherwise, the callee must attach the field
    // manually.  This is useful to prevent really large fields from slowing
    // down page loads.  However, if the content type display is set to
    // "Hide empty fields" then this has no effect as all fields must be
    // attached to determine which are empty.  It should always work with
    // web services.
    $settings['auto_attach'] = TRUE;

    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $settings = parent::defaultStorageSettings();

    // -- Chado Table.
    // The table in Chado that the field maps to.
    $settings['chado_table'] = 'organism';
    // The column of the table in Chado where the value comes from.
    $settings['chado_column'] = 'genus';
    // The base table.
    $settings['base_table'] = 'organism';

    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function propertyDefinitions(FieldStorageDefinitionInterface $field_definition) {

    // This will contain the nested value structure as in Tripal v3.
    // At the Drupal database level it will cache the chado values.
    // We will use the setClass() method to explain this is a complex datatype.
    $properties['value'] = ChadoDataDefinition::create('chado_record')
      ->setSearchable(TRUE)
      ->setSearchOperations(['eq', 'ne', 'contains', 'starts'])
      ->setSortable(TRUE)
      ->setReadOnly(FALSE)
      ->setRequired(TRUE)
      ->setSetting('record_id', 'record_id')
      ->setSetting('chado_schema', 'chado_schema')
      ->setClass('Drupal\tripal_chado\TypedData\ChadoFieldValueLookup');

    // The following nested data definitions describe the keys for the
    // value array and their contents. These are very important for web services.

    // ---- ADD YOUR IMPLEMENTATION HERE ----

    // For Example,
    // $properties['value']->addPropertyDefinition('scientific_name',
    //   ChadoDataDefinition::create('string')
    //     ->setLabel(new TranslatableMarkup('Scientific Name'))
    //     ->setComputed(TRUE)
    //     ->setSearchable(TRUE)
    //     ->setSearchOperations(['eq', 'ne', 'contains', 'starts'])
    //     ->setSortable(TRUE)
    //     ->setReadOnly(TRUE)
    //     ->setRequired(TRUE)
    // );

    $properties['chado_schema'] = DataDefinition::create('string')
      ->setLabel(new TranslatableMarkup('Chado Schema Name'))
      ->setDescription(new TranslatableMarkup('The name of the chado schema this record resides in.'));

    $properties['record_id'] = DataDefinition::create('integer')
      ->setLabel(new TranslatableMarkup('Chado Record ID'))
      ->setDescription(new TranslatableMarkup('The primary key of this record in chado.'));

    return $properties;
  }

  /**
   * Compiles the values array for this field. Analogous to the load function.
   *
   * @param int $record_id
   *   The chado record ID for the values to load.
   * @return array
   *   An array of data matching the data definition laid out in
   *   propertyDefinitions(). All data to be used in display of the field must
   *   be included here.
   */
  public function selectChadoValue($record_id) {

    // SELECT the values from chado for the specific record passed in.
    // For example, if this is an organism field then the record=organism_id
    // ---- ADD YOUR IMPLEMENTATION HERE ----

    // Now compile the value array following the keys defined in
    // the propertyDefinitions() method above.
    $value = [];
    // ---- ADD YOUR IMPLEMENTATION HERE ----

    return $value;
  }

  /**
   * {@inheritdoc}
   */
  public static function generateSampleValue(FieldDefinitionInterface $field_definition) {
    $random = new Random();

    // Create a value array as above with the values generated using $random.
    $values['value'] = [];
    // ---- ADD YOUR IMPLEMENTATION HERE ----

    // Don't forget to add the record_id and schema!
    $values['record_id'] = mt_rand(1, 25555);
    $values['chado_schema'] = 'chado';

    return $values;
  }
}
