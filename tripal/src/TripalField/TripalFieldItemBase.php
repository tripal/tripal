<?php

namespace Drupal\tripal\TripalField;

use Drupal\Core\Field\FieldItemBase;
use Drupal\tripal\TripalField\TripalFieldItemInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Field\FieldStorageDefinitionInterface;
use Drupal\tripal\TripalStorage\IntStoragePropertyType;
use Drupal\Core\TypedData\DataDefinition;

/**
 * Defines the Tripal field item base class.
 */
class TripalFieldItemBase implements TripalFieldItemInterface {

  /**
   * {@inheritdoc}
   */
  public function fieldSettingsForm(array $form, FormStateInterface $form_state) {
    $elements = [];

    $elements["vocabulary_term"] = [
      "#type" => "string",
      "#title" => $this->t("Vocabulary Term"),
      "#required" => TRUE,
      "#description" => $this->t("The vocabulary term.")
    ];

    return $elements + parent::fieldSettingsForm($form,$form_state);
  }

  /**
   * {@inheritdoc}
   */
  public static function propertyDefinitions(FieldStorageDefinitionInterface $field_definition) {
    $properties = [];

    foreach ($this->tripalTypes() as $type) {
      if ($type instanceof IntStoragePropertyType) {
        $properties[$type->getFieldKey()] = DataDefinition::create("integer");
      }
    }

    return $properties;
  }

  /**
   * {@inheritdoc}
   */
  public static function schema(FieldStorageDefinitionInterface $field_definition) {
    $schema = [];

    foreach ($this->tripalTypes() as $type) {
      if ($type instanceof IntStoragePropertyType) {
        $column = [
          "type" => "int"
        ];
        $schema["columns"][$type->getFieldKey()] = $column;
      }
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
      "#type" => "string",
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
