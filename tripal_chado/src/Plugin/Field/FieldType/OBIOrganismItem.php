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
 * Plugin implementation of the 'obi__organism' field type.
 *
 * @FieldType(
 *   id = "obi__organism",
 *   label = @Translation("Organism"),
 *   module = "tripal_chado",
 *   category = @Translation("Tripal: Chado"),
 *   description = @Translation("The organism to which this resource is associated."),
 *   default_widget = "obi__organism_default_widget",
 *   default_formatter = "obi__organism_default_formatter",
 *   cardinality = 1,
 * )
 */
class OBIOrganismItem extends ChadoFieldItemBase {

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
    $settings['term_vocabulary'] = 'OBI';
    // The full name of the vocabulary.
    $settings['vocab_name'] = 'Ontology for Biomedical Investigations';
    // The description of the vocabulary.
    $settings['vocab_description'] = 'The Ontology for Biomedical Investigations (OBI) is build in a collaborative, international effort and will serve as a resource for annotating biomedical investigations, including the study design, protocols and instrumentation used, the data generated and the types of analysis performed on the data.';

    // -- Define the Vocabulary Term.
    // The name of the term.
    $settings['term_name'] = 'organism';
    // The unique ID (i.e. accession) of the term.
    $settings['term_accession'] = '0100026';
    // The definition of the term.
    $settings['term_definition'] = 'A material entity that is an individual living system, such as animal, plant, bacteria or virus, that is capable of replicating or reproducing, growth and maintenance in the right environment. An organism may be unicellular or made up, like humans, of many billions of cells divided into specialized tissues and organs.';

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
    $settings['chado_column'] = 'organism_id';
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
    // TODO: Change the values in the properties to be CV terms.
    $properties['value']->addPropertyDefinition('scientific_name',
      ChadoDataDefinition::create('string')
        ->setLabel(new TranslatableMarkup('Scientific Name'))
        ->setComputed(TRUE)
        ->setSearchable(TRUE)
        ->setSearchOperations(['eq', 'ne', 'contains', 'starts'])
        ->setSortable(TRUE)
        ->setReadOnly(TRUE)
        ->setRequired(TRUE)
    );

    $properties['value']->addPropertyDefinition('genus',
      ChadoDataDefinition::create('string')
        ->setLabel(new TranslatableMarkup('Genus'))
        ->setComputed(TRUE)
        ->setSearchable(TRUE)
        ->setSearchOperations(['eq', 'ne', 'contains', 'starts'])
        ->setSortable(TRUE)
        ->setReadOnly(FALSE)
        ->setRequired(TRUE)
    );

    $properties['value']->addPropertyDefinition('species',
      ChadoDataDefinition::create('string')
        ->setLabel(new TranslatableMarkup('Species'))
        ->setComputed(TRUE)
        ->setSearchable(TRUE)
        ->setSearchOperations(['eq', 'ne', 'contains', 'starts'])
        ->setSortable(TRUE)
        ->setReadOnly(FALSE)
        ->setRequired(TRUE)
    );

    $properties['value']->addPropertyDefinition('infraspecies',
      ChadoDataDefinition::create('string')
        ->setLabel(new TranslatableMarkup('Infraspecies'))
        ->setComputed(TRUE)
        ->setSearchable(TRUE)
        ->setSearchOperations(['eq', 'ne', 'contains', 'starts'])
        ->setSortable(TRUE)
        ->setReadOnly(FALSE)
        ->setRequired(FALSE)
    );

    $properties['value']->addPropertyDefinition('infraspecific_type',
      ChadoDataDefinition::create('string')
        ->setLabel(new TranslatableMarkup('Infraspecies Type'))
        ->setComputed(TRUE)
        ->setSearchable(TRUE)
        ->setSearchOperations(['eq', 'ne', 'contains', 'starts'])
        ->setSortable(TRUE)
        ->setReadOnly(FALSE)
        ->setRequired(FALSE)
    );

    $properties['value']->addPropertyDefinition('common_name',
      ChadoDataDefinition::create('string')
        ->setLabel(new TranslatableMarkup('Common Name'))
        ->setComputed(TRUE)
        ->setSearchable(TRUE)
        ->setSearchOperations(['eq', 'ne', 'contains', 'starts'])
        ->setSortable(TRUE)
        ->setReadOnly(FALSE)
        ->setRequired(FALSE)
    );

    $properties['value']->addPropertyDefinition('abbreviation',
      ChadoDataDefinition::create('string')
        ->setLabel(new TranslatableMarkup('Abbreviation'))
        ->setComputed(TRUE)
        ->setSearchable(TRUE)
        ->setSearchOperations(['eq', 'ne', 'contains', 'starts'])
        ->setSortable(TRUE)
        ->setReadOnly(FALSE)
        ->setRequired(FALSE)
    );

    $properties['chado_schema'] = DataDefinition::create('string')
      ->setLabel(new TranslatableMarkup('Chado Schema Name'))
      ->setDescription(new TranslatableMarkup('The name of the chado schema this record resides in.'));

    $properties['record_id'] = DataDefinition::create('integer')
      ->setLabel(new TranslatableMarkup('Chado Record ID'))
      ->setDescription(new TranslatableMarkup('The primary key of this record in chado.'));

    // @todo add back in the chado linker here.
    // $properties['linker_field'] = ChadoLinkerDataDefinition::create('chado_linker')
    //  ->setComputed(TRUE)
    //  ->setReadOnly(TRUE)
    //  ->setRequired(TRUE);

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

    $orgs = chado_query('SELECT o.*, cvt.name as infraspecific_type
      FROM {organism} o
      LEFT JOIN {cvterm} cvt ON cvt.cvterm_id=o.type_id
      WHERE organism_id=:id',
      [':id' => $record_id]);
    // @todo make sure we use the chado_schema

    // Now overwrite the old values (i.e. cache the new organism).
    foreach ($orgs as $organism) {
      $value = [
        'scientific_name' => $organism->genus . ' ' . $organism->species,
        'genus' => $organism->genus,
        'species' => $organism->species,
        'infraspecific' => $organism->infraspecific_name,
        'infraspecific_type' => $organism->infraspecific_type,
        'common_name' => $organism->common_name,
        'abbreviation' => $organism->abbreviation,
      ];
      if ($organism->infraspecific_type) {
        $value['scientific_name'] .= ' ' . $organism->infraspecific_type;
      }
      if ($organism->infraspecific_name) {
        $value['scientific_name'] .= ' ' . $organism->infraspecific_name;
      }
    }

    return $value;
  }

  /**
   * {@inheritdoc}
   */
  public static function generateSampleValue(FieldDefinitionInterface $field_definition) {

    $random = new Random();
    $values['value'] = $random->word(mt_rand(1, 255));
    $values['value'] = [
      'genus' => $random->word(mt_rand(1, 255)),
      'species' => $random->word(mt_rand(1, 255)),
      'infraspecific' => $random->word(mt_rand(1, 255)),
      'infraspecific_type' => $random->word(mt_rand(1, 255)),
      'common_name' => $random->word(mt_rand(1, 255)),
      'abbreviation' => $random->word(mt_rand(1, 255)),
    ];
    $values['value']['scientific_name'] = $values['value']['genus']
      . ' ' . $values['value']['species']
      . ' ' . $values['value']['infraspecific_type']
      . ' ' . $values['value']['infraspecific_name'];

    $values['record_id'] = mt_rand(1, 25555);
    $values['chado_schema'] = 'chado';

    return $values;
  }
}
