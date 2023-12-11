<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\tripal\TripalField\TripalFormatterBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of Default Tripal field formatter for sequence data
 *
 * @FieldFormatter(
 *   id = "chado_sequence_formatter_default",
 *   label = @Translation("Chado Sequence Residues Display"),
 *   description = @Translation("Displays chado sequence residues from the feature table on the page."),
 *   field_types = {
 *     "chado_sequence_type_default"
 *   }
 * )
 */
class ChadoSequenceFormatterDefault extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $settings = parent::defaultSettings();
    $settings['wrap_setting'] = 50;
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];
    $elements['#attached']['library'][] = 'tripal_chado/tripal_chado.field.ChadoSequenceFormatterDefault';
    $wrap_setting = $this->getSetting('wrap_setting');
    if (!$wrap_setting or ($wrap_setting < 1)) {
      $wrap_setting = 50;
    }

    foreach($items as $delta => $item) {
      $residues = $item->get('residues')->getString();
      if (!empty($residues)) {
        $elements[$delta] = [
          "#markup" => "<pre id='tripal-chado-sequence-format'>"
              . wordwrap($residues, $wrap_setting, '<br>', TRUE) . "</pre>",
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

    $form['wrap_setting'] = [
      '#title' => $this->t('Line length'),
      '#description' => $this->t('Insert a line break after this many sequence residues. Default is 50.'),
      '#type' => 'textfield',
      '#default_value' => $this->getSetting('wrap_setting'),
      '#required' => FALSE,
      '#element_validate' => [[static::class, 'settingsFormValidateLineLength']],
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

  /**
   * Form element validation handler for line wrap setting
   *
   * @param array $form
   *   The form where the settings form is being included in.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state of the (entire) form.
   */
  public static function settingsFormValidateLineLength(array $form, FormStateInterface $form_state) {
    // This form state contains settings for all of the fields for the
    // current content type, we only validate our own field.
    $field_values = $form_state->getValue('fields');
    foreach ($field_values as $field => $field_settings) {
      if (($field_settings['type'] == 'chado_sequence_formatter_default')
          and (array_key_exists('settings_edit_form', $field_settings))) {
        $wrap_setting = $field_settings['settings_edit_form']['settings']['wrap_setting'];

        // Return validation error to the user with offending field highlighted
        if ($wrap_setting and preg_match('/\D/', $wrap_setting)) {
          $form_state->setErrorByName('fields][' . $field . '][settings_edit_form][settings][wrap_setting',
              'Line length must be a non-negative integer');
        }
      }
    }
  }

}
