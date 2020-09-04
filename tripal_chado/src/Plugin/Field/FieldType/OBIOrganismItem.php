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
    $settings = [
      // The short name for the vocabulary (e.g. shcema, SO, GO, PATO, etc.).
      'term_vocabulary' => 'OBI',
      // The name of the term.
      'term_name' => 'organism',
      // The unique ID (i.e. accession) of the term.
      'term_accession' => '0100026',
      // Set to TRUE if the site admin is allowed to change the term
      // type. This will create form elements when editing the field instance
      // to allow the site admin to change the term settings above.
      'term_fixed' => FALSE,
      // The format for display of the organism.
      'field_display_string' => '<i>[organism.genus] [organism.species]</i>',

      // @TODO: these are not hardocded in Tripal v3
      // The table in Chado that the instance maps to.
      'chado_table' => 'organism',
      // The column of the table in Chado where the value of the field comes from.
      'chado_column' => 'organism_id',
      // The base table.
      'base_table' => 'organism',
    ] + parent::defaultFieldSettings();
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function schema(FieldStorageDefinitionInterface $field) {
    return array(
      'columns' => array(
        'source_description' => array(
          'type' => 'varchar',
          'length' => 256,
          'not null' => FALSE,
        ),
        'source_code' => array(
          'type' => 'text',
          'size' => 'big',
          'not null' => FALSE,
        ),
        'source_lang' => array(
          'type' => 'varchar',
          'length' => 256,
          'not null' => FALSE,
        ),
      ),
    );
  }

  /**
   * {@inheritdoc}
   */
  public function isEmpty() {
    $value = $this->get('source_code')->getValue();
    return $value === NULL || $value === '';
  }

  /**
   * {@inheritdoc}
   */
  public static function propertyDefinitions(FieldStorageDefinitionInterface $field_definition) {
    $properties['source_description'] = DataDefinition::create('string')
      ->setLabel(t('Snippet description'));

    $properties['source_code'] = DataDefinition::create('string')
      ->setLabel(t('Snippet code'));

    $properties['source_lang'] = DataDefinition::create('string')
      ->setLabel(t('Programming Language'))
      ->setDescription(t('Snippet code language'));

    return $properties;
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