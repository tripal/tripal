<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of Default Tripal field formatter for sequence data 
 *
 * @FieldFormatter(
 *   id = "chado_sequence_checksum_formatter_default",
 *   label = @Translation("Chado Sequence checksum Formatter"),
 *   description = @Translation("A chado sequence checksum formatter"),
 *   field_types = {
 *     "chado_sequence_checksum_type_default"
 *   }
 * )
 */
class ChadoSequenceChecksumFormatterDefault extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $settings = parent::defaultSettings();
    $settings['case_setting'] = '0';
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];
    $case_setting = $this->getSetting('case_setting');
    foreach($items as $delta => $item) {
      $seqlen_val = $item->get('seqlen')->getString();
      if ( intval($seqlen_val) > 0 ) {
        $value = $item->get('md5checksum')->getString();
        if ($case_setting == 'u') {
          $value = strtoupper($value);
        }
        elseif ($case_setting == 'l') {
          $value = strtolower($value);
        }
        $elements[$delta] = [
          "#markup" => $value,
        ];
      } 
    }

    return $elements;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {
    $form = parent::settingsForm($form, $form_state);

    $form['case_setting'] = [
      '#title' => $this->t('Convert to upper or lower case'),
      '#description' => $this->t('Select preferred case for the checksum'),
      '#type' => 'select',
      '#options' => ['0' => 'As stored', 'u' => 'Upper case', 'l' => 'Lower case'],
      '#default_value' => $this->getSetting('case_setting'),
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
