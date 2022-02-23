<?php

namespace Drupal\tripal\Plugin\Field;

use Drupal\Core\Field\FieldItemBase;
use Drupal\tripal\TripalStorage\TripalFieldItemInterface;

class TripalFieldItemBase extends FieldItemBase extends TripalFieldItemInterface {
  #schema
  #propertyDefinitions

  /**
   * {@inheritdoc}
   */
  public function storageSettingsForm(array &$form, FormStateInterface $form_state, $has_data) {
    $elements = [];

    # move this to fieldSettingsForm method
    $elements["vocabulary_term"] = [
      "#type" => "string",
      "#title" => $this->t("Vocabulary Term"),
      "#required" => TRUE,
      "#description" => $this->t("The vocabulary term."),
      "#disabled" => $has_data
    ];

    # turn into selection
    $elements["storage_plugin_id"] = [
      "#type" => "string",
      "#title" => $this->t("Tripal Storage Plugin ID."),
      "#required" => TRUE,
      "#description" => $this->t(""),
      "#disabled" => $has_data
    ];

    return $elements + parent::storageSettingsForm($form,$form_state,$has_data);
  }

  public function tripalStorageId() {
    return getSetting("storage_plugin_id");
  }
  
  #propertyDefinitions - use tripalTypes to autogenerate typeddata for drupal
}

#First field to implement:
#rdfs_type
#https://github.com/tripal/tripal/tree/7.x-3.x/tripal/includes/TripalFields/rdfs__type
