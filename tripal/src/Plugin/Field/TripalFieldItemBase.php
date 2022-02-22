<?php

namespace Drupal\tripal\Plugin\Field;

use Drupal\Core\Field\FieldItemBase;
use Drupal\tripal\TripalStorage\TripalFieldItemInterface;

class TripalFieldItemBase extends FieldItemBase extends TripalFieldItemInterface {

  /**
   * {@inheritdoc}
   */
  public function storageSettingsForm(array &$form, FormStateInterface $form_state, $has_data) {
    $elements = [];

    $elements["vocabulary_term"] = [
      "#type" => "string",
      "#title" => $this->t("Vocabulary Term"),
      "#required" => TRUE,
      "#description" => $this->t("The vocabulary term."),
      "#disabled" => $has_data
    ];

    $elements["vocabulary_term"] = [
      "#type" => "string",
      "#title" => $this->t("Vocabulary Term"),
      "#required" => TRUE,
      "#description" => $this->t("The vocabulary term."),
      "#disabled" => $has_data
    ];

    return $elements + parent::storageSettingsForm($form,$form_state,$has_data);
  }
}
