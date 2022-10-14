<?php

namespace Drupal\tripal\TripalField;

use Drupal\Core\Field\FieldItemBase;
use Drupal\tripal\TripalField\Interfaces\TripalFieldItemInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Field\FieldStorageDefinitionInterface;
use Drupal\tripal\TripalStorage\IntStoragePropertyType;
use Drupal\tripal\TripalStorage\VarCharStoragePropertyType;
use Drupal\tripal\TripalStorage\TextStoragePropertyType;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
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
    $settings = [
      'termIdSpace' => '',
      'termAccession' => ''
    ];
    return $settings + parent::defaultFieldSettings();
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $settings = [
      'storage_plugin_id' => '',
      'storage_plugin_settings' => [],
    ];
    return $settings + parent::defaultStorageSettings();
  }


  /**
   * {@inheritdoc}
   */
  public static function defaultTripalTypes($entity_type_id, $field_type) {
    return [
      // The record Id can be used by the Tripal storage plugin to
      // assocaite the values this field provides with a record in the
      // data store.
      new IntStoragePropertyType($entity_type_id, $field_type, "record_id"),
    ];
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultTripalValuesTemplate($entity_type_id, $field_type, $entity_id) {
    return [
      new StoragePropertyValue($entity_type_id, $field_type, "record_id", $entity_id),
    ];
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
      else if ($type instanceof TextStoragePropertyType) {
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
      else if ($type instanceof TextStoragePropertyType) {
        $column = [
          "type" => "text",
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
      '#default_value' => $this->getSetting('storage_plugin_id'),
      "#required" => TRUE,
      "#description" => $this->t("The plugin ID of the storage backend."),
      "#disabled" => TRUE
    ];


    // Construct a table for the vocabulary information.
    $headers = ['Storage Property', 'Value'];
    $settings = $this->getSetting('storage_plugin_settings');
    $rows = [];
    foreach ($settings as $setting_name => $setting_value) {
      $rows[] = [
        [
          'data' => $setting_name,
          'header' => TRUE,
          'width' => '20%',
        ],
        json_encode($setting_value),
      ];
    }
    $elements['settings_fs'] = [
      '#type' => 'details',
      '#title' => $this->t("Storage Settings"),
      '#description' => $this->t("The following storage settings apply for this field."),
      '#open' => False,
    ];
    $elements['settings_fs']['table_label'] = [
      '#type' => 'item',
      '#title' => $this->t('Current Settings'),
    ];
    $elements['settings_fs']['settings_table'] = [
      '#type' => 'table',
      '#header'=> $headers,
      '#rows' => $rows,
      '#empty' => $this->t('There are no settings.'),
      '#sticky' => False
    ];
    return $elements + parent::storageSettingsForm($form,$form_state,$has_data);
  }

  /**
   * {@inheritdoc}
   */
  public function tripalStorageId() {
    return $this->getSetting("storage_plugin_id");
  }

  /**
   * {@inheritdoc}
   */
  public function tripalSave($field_item, $field_name, $properties, $entity) {
    $delta = $field_item->getName();
    foreach ($properties as $property) {
      $prop_key = $property->getKey();
      $value = $entity->get($field_name)->get($delta)->get($prop_key)->getValue();
      $property->setValue($value);
    }
  }

  /**
   * {@inheritdoc}
   */
  public function tripalLoad($field_item, $field_name, $properties, $entity) {
    $delta = $field_item->getName();
    foreach ($properties as $property) {
      $prop_key = $property->getKey();
      $entity->get($field_name)->get($delta)->get($prop_key)->setValue($property->getValue(), False);
    }
  }


  /**
   * {@inheritdoc}
   */
  public function tripalClear($field_item, $field_name, $properties, $entity) {
    $delta = $field_item->getName();
    foreach ($properties as $property) {
      $prop_key = $property->getKey();
      // Never clear out the record_id we need this to map to Chado records.
      if ($prop_key == 'record_id') {
        continue;
      }
      $entity->get($field_name)->get($delta)->get($prop_key)->setValue('', False);
    }
  }
}
