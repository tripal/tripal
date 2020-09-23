<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Component\Utility\Html;
use Drupal\Core\Field\FieldItemInterface;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Field\FormatterBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of the 'obi__organism_default_formatter' formatter.
 *
 * @FieldFormatter(
 *   id = "obi__organism_default_formatter",
 *   label = @Translation("Organism: Simple String"),
 *   field_types = {
 *     "obi__organism"
 *   }
 * )
 */
class OBIOrganismDefaultFormatter extends FormatterBase {

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    return [
      // Implement default settings.
    ] + parent::defaultSettings();
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {
    return [
      // Implement settings form.
    ] + parent::settingsForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function settingsSummary() {
    $summary = [];
    // Implement settings summary.

    return $summary;
  }

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];

    foreach ($items as $delta => $item) {
      $elements[$delta] = ['#markup' => $this->viewValue($item)];
    }

    return $elements;
  }

  /**
   * Retrieve a specific value from the items list.
   *
   * @todo move this into a ChadoWidgetBase class.
   *
   * @param array $item
   *   The values for the OBIOrganismItem field on this entity.
   * @param string $property_name
   *   The name of the value or property you would like to pull out. Supported
   *   values include record_id, chado_schema, etc.
   */
  public function getChadoValue($item, $property_name) {

    if ($property_name == 'record_id') {
      return $item->get('record_id')->getValue();
    }
    elseif ($property_name == 'chado_schema') {
      return $item->get('chado_schema')->getValue();
    }
    else {
      $values = unserialize($item->getValue());
      if (isset($values[$property_name])) {
        return $values[$property_name];
      }
    }
    return NULL;
  }

  /**
   * Generate the output appropriate for one field item.
   *
   * @param \Drupal\Core\Field\FieldItemInterface $item
   *   One field item.
   *
   * @return string
   *   The textual output generated.
   */
  protected function viewValue(FieldItemInterface $item) {

    $genus = $this->getChadoValue($item, 'genus');
    $species = $this->getChadoValue($item, 'species');
    $common_name = $this->getChadoValue($item, 'common_name');

    $output = $genus . ' ' . $species;

    return nl2br(Html::escape($output));
  }

}
