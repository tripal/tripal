<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Component\Utility\Html;
use Drupal\Core\Field\FieldItemInterface;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\tripal_chado\Plugin\Field\ChadoFormatterBase;
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
class OBIOrganismDefaultFormatter extends ChadoFormatterBase {

  /**
   * @param array
   *   The options available for format.
   *   These map to the keys of the field property definition.
   */
  public static $format_options = [
    'scientific_name' => 'Scientific Name',
    'common_name' => 'Common Name',
    'abbreviation' => 'Abbreviation',
  ];

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    return [
      'format' => 'scientific_name',
    ] + parent::defaultSettings();
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {
    $element = [];

    $element['format'] = [
      '#type' => 'select',
      '#title' => t('Format'),
      '#description' => t('The format to display the organism using.'),
      '#options' => $this::$format_options,
      '#default_value' => $this->getSetting('format'),
    ];

    $element += parent::settingsForm($form, $form_state);
    return $element;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsSummary() {

    $format = $this->getSetting('format');
    $options = $this::$format_options;
    $summary = [
      '#markup' => 'Format: ' . $options[$format],
    ];
    return $summary;
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

    $format = $this->getSetting('format');
    $output = $this->getChadoValue($item, $format);
    return nl2br(Html::escape($output));
  }

}
