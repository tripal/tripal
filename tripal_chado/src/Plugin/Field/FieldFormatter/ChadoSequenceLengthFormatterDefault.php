<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of Default Tripal field formatter for sequence data 
 *
 * @FieldFormatter(
 *   id = "chado_sequence_length_formatter_default",
 *   label = @Translation("Chado Sequence Length Formatter"),
 *   description = @Translation("A chado sequence length formatter"),
 *   field_types = {
 *     "chado_sequence_length_type_default"
 *   }
 * )
 */
class ChadoSequenceLengthFormatterDefault extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $settings = parent::defaultSettings();
    $settings['field_prefix'] = '';
    $settings['field_suffix'] = '';
    $settings['thousand_separator'] = '';
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];
    $elements['#attached']['library'][] = 'tripal_chado/tripal_chado.field.ChadoSequenceLengthFormatterDefault';
    $field_prefix = $this->getSetting('field_prefix');
    $field_suffix = $this->getSetting('field_suffix');
    $thousand_separator = $this->getSetting('thousand_separator');

    foreach($items as $delta => $item) {
      $value = $item->get('seqlen')->getString();
      if (strlen($thousand_separator)) {
        // For an integer we can hardcode the unused decimal setting
        $value = number_format(floatval($value), 0, '.', $thousand_separator);
      }
      $elements[$delta] = [
        "#markup" => $field_prefix . $value . $field_suffix,
      ];
    }

    return $elements;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {
    $form = parent::settingsForm($form, $form_state);

    $form['field_prefix'] = [
      '#title' => $this->t('Text to display before the sequence length'),
      '#description' => $this->t('Enter text here that will be displayed before the'
                     . ' sequence length value, or leave blank for no additional text'),
      '#type' => 'textfield',
      '#default_value' => $this->getSetting('field_prefix'),
      '#required' => FALSE,
    ];
    $form['field_suffix'] = [
      '#title' => $this->t('Text to display after the sequence length'),
      '#description' => $this->t('Enter text here that will be displayed after the'
                     . ' sequence length value, e.g. " b.p.", or leave blank for no additional text'),
      '#type' => 'textfield',
      '#default_value' => $this->getSetting('field_suffix'),
      '#required' => FALSE,
    ];
    $form['thousand_separator'] = [
      '#title' => $this->t('Thousand Separator'),
      '#description' => $this->t('Character to display every three digits'),
      '#type' => 'textfield',
      '#default_value' => $this->getSetting('thousand_separator'),
      '#required' => FALSE,
    ];

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsSummary() {
    $summary = parent::settingsSummary();
    $summary[] = $this->t('Set display format');
    return $summary;
  }

}
