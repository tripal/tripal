<?php

/**
 * @file
 * Contains \Drupal\tripal_chado\Plugin\Field\FieldType\OBIOrganismItem.
 */

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal_chado\Field\ChadoFieldItemBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Field\FieldDefinitionInterface;
use Drupal\Core\Field\FieldStorageDefinitionInterface;
use Drupal\Core\TypedData\DataDefinition;
use Drupal\tripal_chado\TypedData\ChadoOrganismDataDefinition;
use Drupal\tripal_chado\TypedData\ChadoLinkerDataDefinition;

/**
 * Plugin implementation of the 'obi__organism' field type
 *
 * @FieldType (
 *   id = "obi__organism",
 *   label = @Translation("Organism"),
 *   module = "tripal_chado",
 *   category = @Translation("Chado"),
 *   description = @Translation("The organism to which this resource is associated."),
 *   default_widget = "obi__organism_default",
 *   default_formatter = "obi__organism_default"
 * )
 */
class OBIOrganismItem extends ChadoFieldItemBase  {
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

    // -- Chado Table.
    // The table in Chado that the field maps to.
    $settings['chado_table'] = 'organism';
    // The column of the table in Chado where the value comes from.
    $settings['chado_column'] = 'organism_id';
    // The base table.
    $settings['base_table'] = 'organism';

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
    // The format for display of the organism.
    $settings['field_display_string'] = '<i>[organism.genus] [organism.species]</i>';

    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function propertyDefinitions(FieldStorageDefinitionInterface $field_definition) {

    $properties['value'] = ChadoOrganismDataDefinition::create('chado_organism')
      ->setSettings([
        'searchable' => TRUE,
        'operations' => ['eq', 'ne', 'contains', 'starts'],
        'sortable' => TRUE
      ])
      ->setReadOnly(FALSE)
      ->setRequired(TRUE);

    $properties['record_id'] = DataDefinition::create('integer')
      ->setReadOnly(TRUE)
      ->setRequired(TRUE);

    $properties['linker_field'] = ChadoLinkerDataDefinition::create('chado_linker')
      ->setComputed(TRUE)
      ->setReadOnly(TRUE)
      ->setRequired(TRUE);

    return $properties;
  }

  /**
   * {@inheritdoc}
   */
  public static function schema(FieldStorageDefinitionInterface $field) {
    $schema = [
      'columns' => [
        'record_id' => [
          'type' => 'int',
        ]
      ]
    ];

    return $schema;
  }

  /**
   * {@inheritdoc}
   */
  public function isEmpty() {
    $value = $this->get('value')->getValue();
    return $value === NULL || $value === '';
  }

  /**
   * {@inheritdoc}
   */
  public function fieldSettingsForm(array $form, FormStateInterface $form_state) {

    $element = parent::fieldSettingsForm($form, $form_state);
    $settings = $this->getSettings();


    $element['instructions'] = [
      '#type' => 'item',
      '#markup' => 'You may rewrite the way this field is presented to the end-user.
        This field allows you to use tokens to indicate how the
        value should be displayed.  Tokens will be substituted with appriorate
        data from the database.  See the Available tokens list for the
        tokens you may use.',
    ];

    $element['field_display_string'] = [
      '#type' => 'textfield',
      '#title' => 'Rewrite Value',
      '#description' => t('Provide a mixture of text and/or tokens for the format.
          For example: [organism.genus] [organism.species].  When displayed,
          the tokens will be replaced with the actual value.'),
      '#default_value' => $settings['field_display_string'],
    ];

    $element['tokens'] = [
      '#type' => 'fieldset',
      '#collapsed' => TRUE,
      '#collapsible' => TRUE,
      '#title' => 'Available Tokens',
    ];
    $headers = ['Token', 'Description'];
    $rows = [];

    // Here we use the chado_get_tokens rather than the
    // tripal_get_entity_tokens because we can't gurantee that all organisms
    // have entities.
    // @TODO: the chado_get_tokens isn't yet implmeented.
    /*
    $tokens = chado_get_tokens('organism');
    foreach ($tokens as $token) {
      $rows[] = [
        $token['token'],
        $token['description'],
      ];
    }*/

    $element['tokens']['list'] = [
      '#type' => 'table',
      '#headers' => $headers,
      '#rows' => $rows,
      '#empty' => 'There are no tokens'
    ];
    return $element;
  }

}
