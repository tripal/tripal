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
  public function fieldSettingsForm(array $form, FormStateInterface $form_state) {
    $elements = [];

    $elements["vocabulary_term"] = [
      "#type" => "item",
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

    foreach (get_called_class()::tripalTypes($field_definition->getTargetEntityTypeId()) as $type) {
      if ($type instanceof IntStoragePropertyType) {
        $properties[$type->getFieldKey()] = DataDefinition::create("integer");
      }
      else if ($type instanceof VarCharStoragePropertyType) {
        $properties[$type->getFieldKey()] = DataDefinition::create("string");
      }
      else {
        throw new RuntimeException("Unknown Tripal Property Type class.");
      }
    }

    if (empty(properties)) {
      throw new RuntimeException("Cannot return empty array.");
    }

    return $properties;
  }

  /**
   * {@inheritdoc}
   */
  public static function schema(FieldStorageDefinitionInterface $field_definition) {
    $schema = [];

    foreach (get_called_class()::tripalTypes($field_definition->getTargetEntityTypeId()) as $type) {
      if ($type instanceof IntStoragePropertyType) {
        $column = [
          "type" => "int"
        ];
        $schema["columns"][$type->getFieldKey()] = $column;
      }
      else if ($type instanceof VarCharStoragePropertyType) {
        $column = [
          "type" => "varchar"
          ,"size" => $type->getMaxCharacterSize()
        ];
        $schema["columns"][$type->getFieldKey()] = $column;
      }
      else {
        throw new RuntimeException("Unknown Tripal Property Type class.");
      }
    }

    if (empty(properties)) {
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
      "#type" => "item",
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
