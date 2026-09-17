<?php

namespace Drupal\tripal\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldWidget;
use Drupal\tripal\TripalField\TripalWidgetBase;

/**
 * Plugin implementation of default Tripal string type widget.
 */
#[TripalFieldWidget(
  id: 'default_tripal_string_type_widget',
  label: new TranslatableMarkup('Tripal String Widget'),
  description: new TranslatableMarkup('The default string type widget.'),
  field_types: [
    'tripal_string_type',
  ],
)]
class TripalStringTypeWidget extends TripalWidgetBase {


  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {
    $element['value'] = $element + [
      '#type' => 'textfield',
      '#default_value' => $items[$delta]->value ?? '',
      '#placeholder' => $this->getSetting('placeholder'),
      '#maxlength' => $this->getFieldSetting('max_length'),
      '#attributes' => ['class' => ['js-text-full', 'text-full']],
    ];
    return $element;
  }

  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {
    foreach ($values as $key => $item) {
      if (array_key_exists('value', $item) && $item['value'] === '' && $this->getSetting('null_if_empty')) {
        $values[$key]['value'] = NULL;
      }
    }
    return $values;
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $defaultSettings = parent::defaultSettings();
    $defaultSettings['null_if_empty'] = FALSE;
    return $defaultSettings;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {
    $elements = parent::settingsForm($form, $form_state);
    $elements['null_if_empty'] = [
      '#type' => 'checkbox',
      '#title' => $this->t('Store NULL for empty string'),
      '#description' => $this->t('If the form element contains an empty string, store a NULL in the database table instead of the empty string.'),
      '#default_value' => $this->getSetting('null_if_empty') ?? 0,
    ];
    return $elements;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsSummary() {
    $summary = parent::settingsSummary();
    $summary[] = $this->t('Store NULL if empty: @setting',
      ['@setting' => $this->getSetting('null_if_empty') ? $this->t('Yes') : $this->t('No')]);
    return $summary;
  }

}
