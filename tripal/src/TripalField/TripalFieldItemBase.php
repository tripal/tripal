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

    $elements['field_term_fs'] = [
      '#type' => 'details',
      '#title' => $this->t("Controlled Vocbulary Term"),
      '#description' => $this->t("All fields attached to a Tripal-based content " .
          "type must be associated with a controlled vocabulary term. " .
          "Use caution when changing the term. It should accurately represent " .
          "the type of data stored in this field.  Using terms that are developed ".
          "by the community (e.g. Sequence Ontology, etc.) ensures that the ".
          "data on your site is discoverable and interoperable."),
      '#open' => False,
    ];
    // Construct a table for the vocabulary information.
    $headers = ['Term Property', 'Value'];
    $rows = [];
    $rows[] = [
      [
        'data' => 'Vocabulary Name',
        'header' => TRUE,
        'width' => '20%',
      ],
      $vocabulary->getName(),
    ];
    $rows[] = [
      [
        'data' => 'Vocabulary Description',
        'header' => TRUE,
        'width' => '20%',
      ],
      $vocabulary->getLabel(),
    ];
    $rows[] = [
      [
        'data' => 'Term ID Space',
        'header' => TRUE,
        'width' => '20%',
      ],
      $vocabulary->getName(),
    ];
    $rows[] = [
      [
        'data' => 'Term ID Space Description',
        'header' => TRUE,
        'width' => '20%',
      ],
      $idSpace->getDescription(),
    ];
    $rows[] = [
      [
        'data' => 'Term Accession',
        'header' => TRUE,
        'width' => '20%',
      ],
      $termAccession,
    ];
    $rows[] = [
      [
        'data' => 'Term ID',
        'header' => TRUE,
        'width' => '20%',
      ],
      $termIdSpace . ':' . $termAccession,
    ];
    $rows[] = [
      [
        'data' => 'Term Name',
        'header' => TRUE,
        'width' => '20%',
      ],
      $term->getName(),
    ];
    $rows[] = [
      [
        'data' => 'Term Definition',
        'header' => TRUE,
        'width' => '20%',
      ],
      $term->getDefinition(),
    ];
    $elements['field_term_fs']['table_label'] = [
      '#type' => 'item',
      '#title' => $this->t('The Current Term'),
      '#description' => $this->t("Terms belong to a vocabulary (e.g. Sequence "  .
         "Ontology) and are identified with a unique accession which is often  " .
         "numeric but may not be (e.g. gene accession is 0000704 in the Sequence " .
         "Ontology). Term IDs are prefixed with an ID Space (e.g. SO). The " .
         "ID Space and the accession will uniquely identify a term (e.g. SO:0000704).")
    ];
    $elements['field_term_fs']['field_term'] = [
      '#type' => 'table',
      '#header'=> $headers,
      '#rows' => $rows,
      '#empty' => $this->t('There is no term associated with this field.'),
      '#sticky' => False
    ];

    $elements['field_term_fs']["vocabulary_term"] = [
      "#type" => "textfield",
      "#title" => $this->t("Change the Term"),
      "#required" => TRUE,
      "#description" => $this->t("Enter a vocabulary term name. A set of matching candidates will be provided to choose from."),
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
          "type" => "varchar",
          "length" => $type->getMaxCharacterSize()
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
