<?php

namespace Drupal\tripal\TripalField;

use Drupal\Core\Field\FieldItemBase;
use Drupal\tripal\TripalField\Interfaces\TripalFieldItemInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Field\FieldStorageDefinitionInterface;
use Drupal\tripal\TripalStorage\IntStoragePropertyType;
use Drupal\tripal\TripalStorage\VarCharStoragePropertyType;
use Drupal\Core\TypedData\DataDefinition;
use \RuntimeException;

/**
 * Defines the Tripal field item base class.
 */
abstract class TripalFieldItemBase extends FieldItemBase implements TripalFieldItemInterface {

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    return [
      'tripal_term' => ''
    ] + parent::defaultFieldSettings();
  }

  /**
   * {@inheritdoc}
   */
  public function fieldSettingsForm(array $form, FormStateInterface $form_state) {
    $elements = [];

    $tripal_term = $this->getSetting('tripal_term');
    list($idSpace_name, $accession) = explode(':', $tripal_term);

    $idSpace_manager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $idSpace = $idSpace_manager->loadCollection($idSpace_name);
    $term = $idSpace->getTerm($accession);
    $vocabulary = $term->getVocabularyObject();

    // Construct a table for the vocabulary information.
    $headers = [];
    $rows = [];
    $rows[] = [
      [
        'data' => 'Vocabulary',
        'header' => TRUE,
        'width' => '20%',
      ],
      $vocabulary->getName() . ' (' . $idSpace_name. ') ' . $idSpace->getDescription(),
    ];
    $rows[] = [
      [
        'data' => 'Term',
        'header' => TRUE,
        'width' => '20%',
      ],
      $idSpace_name . ':' . $accession,
    ];
    $rows[] = [
      [
        'data' => 'Name',
        'header' => TRUE,
        'width' => '20%',
      ],
      $term->getName(),
    ];
    $rows[] = [
      [
        'data' => 'Definition',
        'header' => TRUE,
        'width' => '20%',
      ],
      $term->getDefinition(),
    ];
//     $table = [
//       'header' => $headers,
//       'rows' => $rows,
//       'attributes' => [],
//       'sticky' => FALSE,
//       'caption' => '',
//       'colgroups' => [],
//       'empty' => '',
//     ];

    $description = t('All fields attached to a Tripal-based content type must
        be associated with a controlled vocabulary term.  Please use caution
        when changing the term for this field as other sites may expect this term
        when querying web services.');
//     if (array_key_exists('term_fixed', $instance['settings']) and $instance['settings']['term_fixed']) {
//       $description = t('All fields attached to a Tripal-based content type must
//         be associated with a controlled vocabulary term. This field mapping is
//         required and cannot be changed');
//     }
//     $element['term_vocabulary'] = [
//       '#type' => 'value',
//       '#value' => $vocabulary,
//     ];
//     $element['term_name'] = [
//       '#type' => 'value',
//       '#value' => $term_name,
//     ];
//     $element['term_accession'] = [
//       '#type' => 'value',
//       '#value' => $accession,
//     ];
    $elements['field_term'] = [
      '#type' => 'table',
      '#header'=> $headers,
      '#rows' => $rows,
      '#empty' => t('There is no term associated with this field.'),
      '#title' => 'Controlled Vocabulary Term',
      '#description' => $description,
//      '#prefix' => '<div id = "tripal-field-term-fieldset">',
//      '#suffix' => '</div>',
    ];
//     $element['field_term']['details'] = [
//       '#type' => 'item',
//       '#title' => 'Current Term',
//       '#markup' => theme_table($table),
//     ];

    $elements["vocabulary_term"] = [
      "#type" => "textfield",
      "#title" => $this->t("Vocabulary Term"),
      "#required" => TRUE,
      "#description" => $this->t("The vocabulary term."),
      '#default_value' => $term->getName()
    ];

    return $elements + parent::fieldSettingsForm($form,$form_state);
  }

  /**
   * {@inheritdoc}
   */
  public static function propertyDefinitions(FieldStorageDefinitionInterface $field_definition) {
    $properties = [];

    foreach (get_called_class()::tripalTypes($field_definition) as $type) {
      if ($type instanceof IntStoragePropertyType) {
        $properties[$type->getKey()] = DataDefinition::create("integer");
      }
      else if ($type instanceof VarCharStoragePropertyType) {
        $properties[$type->getKey()] = DataDefinition::create("string");
      }
      else {
        throw new RuntimeException("Unknown Tripal Property Type class.");
      }
    }

    if (empty($properties)) {
      throw new RuntimeException("Cannot return empty array.");
    }

    return $properties;
  }

  /**
   * {@inheritdoc}
   */
  public static function schema(FieldStorageDefinitionInterface $field_definition) {
    $schema = [];
    foreach (get_called_class()::tripalTypes($field_definition) as $type) {
      if ($type instanceof IntStoragePropertyType) {
        $column = [
          "type" => "int"
        ];
        $schema["columns"][$type->getKey()] = $column;
      }
      else if ($type instanceof VarCharStoragePropertyType) {
        $column = [
          "type" => "varchar"
          ,"length" => $type->getMaxCharacterSize()
        ];
        $schema["columns"][$type->getKey()] = $column;
      }
      else {
        throw new RuntimeException("Unknown Tripal Property Type class.");
      }
    }

    if (empty($schema)) {
      throw new RuntimeException("Cannot return empty array.");
    }

    return $schema;
  }

  /**
   * {@inheritdoc}
   */
  public function storageSettingsForm(array &$form, FormStateInterface $form_state, $has_data) {
    $elements = [];

    // turn into selection
    $elements["storage_plugin_id"] = [
      "#type" => "textfield",
      "#title" => $this->t("Tripal Storage Plugin ID."),
      "#required" => TRUE,
      "#description" => $this->t(""),
      "#disabled" => $has_data
    ];

    return $elements + parent::storageSettingsForm($form,$form_state,$has_data);
  }

  /**
   * {@inheritdoc}
   */
  public function tripalStorageId() {
    return $this->getSetting("storage_plugin_id");
  }
}
