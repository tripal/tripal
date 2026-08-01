<?php

namespace Drupal\tripal\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldFormatter;
use Drupal\tripal\TripalField\TripalFormatterBase;

/**
 * Plugin implementation of default Tripal real type formatter.
 */
#[TripalFieldFormatter(
  id: 'default_tripal_real_type_formatter',
  label: new TranslatableMarkup('Default Real Type Formatter'),
  description: new TranslatableMarkup('The default real type formatter.'),
  field_types: [
    'tripal_real_type',
  ],
)]
class DefaultTripalRealTypeFormatter extends TripalFormatterBase {

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $settings = parent::defaultSettings();
    $settings['field_prefix'] = '';
    $settings['field_suffix'] = '';
    $settings['thousand_separator'] = '';
    $settings['decimal_separator'] = '.';
    $settings['decimal_places'] = '';
    $settings['hide_condition'] = 'never';
    $settings['hide_value'] = '';
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];
    $field_prefix = $this->getSetting('field_prefix');
    $field_suffix = $this->getSetting('field_suffix');
    $thousand_separator = $this->getSetting('thousand_separator');
    $decimal_separator = $this->getSetting('decimal_separator') ?? '.';
    $decimal_places = $this->getSetting('decimal_places') ?? '';
    $hide_condition = $this->getSetting('hide_condition') ?? 'never';
    $hide_value = $this->getSetting('hide_value') ?? '';
#dpm($this->getSettings(), 'CPF1 settings');
    foreach($items as $delta => $item) {
      $value = $item->get("value")->getValue() ?? '';
      $hide = ((($hide_condition == '') and !$value)
           or (($hide_condition == 'if_value') and ($value == $hide_value)));
#print "\nCP3 hide_condition \"$hide_condition\" hide_value ";var_dump($hide_value);
#print "CP4 value ";var_dump($value);
#print "CP5 hide ";var_dump($hide);
      if (!$hide) {
        if (strlen($value) && (strlen($thousand_separator) || strlen($decimal_places))) {
          // If the decimal places setting is not specified (i.e. default),
          // then we need a value for number_format(), so find the actual
          // number of decimal places in the current value.
          if (!$decimal_places) {
            $decimal_places = 0;
            if (preg_match('/' . preg_quote($decial_separator, '/') . '(.*)$/', $value, $matches)) {
              $decimal_places = strlen($matches[1]);
            }
#dpm($decimal_places, "CPF2 DP from \"$value\""); //;;;
          }
#dpm($value, "CPF3 value, decimal_places=$decimal_places");
          $value = number_format(floatval($value), $decimal_places, $decimal_separator, $thousand_separator);
        }
        $elements[$delta] = [
          "#markup" => $field_prefix . $value . $field_suffix,
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

    $form['field_prefix'] = [
      '#title' => $this->t('Text to display before the field value'),
      '#description' => $this->t('Enter text here that will be displayed before the field value, or leave blank for no additional text'),
      '#type' => 'textfield',
      '#default_value' => $this->getSetting('field_prefix'),
      '#required' => FALSE,
    ];
    $form['field_suffix'] = [
      '#title' => $this->t('Text to display after the field value'),
      '#description' => $this->t('Enter text here that will be displayed after the field value, or leave blank for no additional text'),
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
    $form['decimal_separator'] = [
      '#title' => $this->t('Decimal Separator'),
      '#description' => $this->t('Character to use for the decimal point'),
      '#type' => 'textfield',
      '#default_value' => $this->getSetting('decimal_separator'),
      '#required' => FALSE,
    ];
    $form['decimal_places'] = [
      '#title' => $this->t('Decimal Places'),
      '#description' => $this->t('Number of decimal places to display'),
      '#type' => 'textfield',
      '#default_value' => $this->getSetting('decimal_places'),
      '#required' => FALSE,
    ];
    $form['hide_condition'] = [
      '#title' => $this->t('You may provide a condition when the field is not displayed'),
      '#type' => 'radios',
      '#options' => [
        '' => $this->t('Hide if zero'),
        'if_value' => $this->t('Hide if equal to a specific value'),
        'never_hide' => $this->t('Never hide'),
      ],
      '#default_value' => $this->getSetting('hide_condition') ?? '',
    ];
    $form['hide_value'] = [
      '#title' => $this->t('Specific value to be hidden'),
      '#description' => $this->t('A value that you do not want displayed, e.g. "0" or "-1"'),
      '#type' => 'textfield',
      '#default_value' => $this->getSetting('hide_value') ?? '',
      '#required' => FALSE,
    ];

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsSummary() {
    $summary = parent::settingsSummary();
    $decimal_places = $this->getSetting('decimal_places');
    if (!strlen($decimal_places)) {
      $decimal_places = $this->t('not specified');
    }
    $summary[] = $this->t('Places: @decimal_places', ['@decimal_places' => $decimal_places]);
    return $summary;
  }

}
