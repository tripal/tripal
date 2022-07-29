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
      'termIdSpace' => '',
      'termAccession' => ''
    ] + parent::defaultFieldSettings();
  }

  /**
   * {@inheritdoc}
   */
  public function fieldSettingsForm(array $form, FormStateInterface $form_state) {
    $elements = [];

    $termIdSpace = $this->getSetting('termIdSpace');
    $termAccession = $this->getSetting('termAccession');

    $idSpace_manager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $idSpace = $idSpace_manager->loadCollection($termIdSpace);
    $term = $idSpace->getTerm($termAccession);
    $vocabulary = $term->getVocabularyObject();

    $elements["vocabulary_term"] = [
      "#type" => "textfield",
      "#title" => $this->t("Vocabulary Term"),
      "#required" => TRUE,
      "#description" => $this->t("The vocabulary term."),
      '#default_value' => $term->getName()
    ];

    // Construct a table for the vocabulary information.
    $headers = [];
    $rows = [];
    $rows[] = [
      [
        'data' => 'Vocabulary',
        'header' => TRUE,
        'width' => '20%',
      ],
      $vocabulary->getName() . ' (' . $termIdSpace. ') ' . $idSpace->getDescription(),
    ];
    $rows[] = [
      [
        'data' => 'Term',
        'header' => TRUE,
        'width' => '20%',
      ],
      $termIdSpace . ':' . $termAccession,
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
    $elements['field_term'] = [
      '#type' => 'table',
      '#header'=> $headers,
      '#rows' => $rows,
      '#empty' => $this->t('There is no term associated with this field.'),
      '#caption' => $this->t('The currently selected term'),
      '#sticky' => False
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
