<?php

namespace Drupal\tripal\Plugin\Field\FieldWidget;

use Drupal\Core\Field\Attribute\FieldWidget;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\TripalWidgetBase;

/**
 * Plugin implementation of default Tripal text type widget.
 */
#[TripalFieldWidget(
  id: 'default_tripal_text_type_widget',
  label: new TranslatableMarkup('Tripal Text Widget'),
  description: new TranslatableMarkup('The default text type widget.'),
  field_types: [
    'tripal_text_type',
  ],
)]
class TripalTextTypeWidget extends TripalWidgetBase {


  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    $element['value'] = $element + [
      '#base_type' => 'textarea',
      '#type' => 'text_format',
      '#format' => $this->getSetting('filter_format'),
      '#default_value' => $items[$delta]->value ?? '',
      '#rows' => $this->getSetting('num_rows'),
      '#placeholder' => $this->getSetting('placeholder'),
      '#attributes' => ['class' => ['js-text-full', 'text-full']],
    ];

    return $element;
  }

  /**
   * {@inheritdoc}
   */
  public static function afterBuild(array $element, FormStateInterface $form_state) {
    parent::afterBuild($element, $form_state);

    // Alter the format drop down so that it is hidden.
    // We do this because any changes here are not actually saved and thus
    // having it enabled is misleading.
    // Note: We couldn't disable it because the text format element would stop working ;-)
    foreach(\Drupal\Core\Render\Element::children($element) as $key) {
      $element[$key]['value']['format']['#attributes']['class'][] = 'hidden';
    }

    return $element;
  }

  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {

    // The text_format element returns an item consisting of both a value and a
    // format. We only want to keep the format.
    foreach ($values as $key => $item) {
      $values[$key]['value'] = $item['value']['value'];
    }
    return $values;
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    return [
      'filter_format' => 'basic_html',
      'num_rows' => 5,
    ] + parent::defaultSettings();
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {

    // Get all the filter formats available for the current site.
    $options = [];
    foreach (filter_formats() as $name => $object) {
      $options[$name] = $object->get('name');
    }

    $element['num_rows'] = [
      '#type' => 'number',
      '#title' => $this->t('Number of Rows'),
      '#description' => $this->t('Indicate the number of lines to display in the widget by default. A larger number will make for a longer textarea.'),
      '#required' => TRUE,
      '#default_value' => $this->getSetting('num_rows'),
      '#min' => 1,
      '#max' => 100,
    ];

    $element['filter_format'] = [
      '#type' => 'select',
      '#title' => $this->t('Text Filter Format'),
      '#options' => $options,
      '#description' => $this->t("Select the text filter format you want applied
        to this field. Everyone will use the same format. If a user does not have
        permission to the format chosen for this field then they won't be able to
        edit it. Please keep in mind there are security concerns with choosing
        'full_html' and thus this should only be your choice if you have
        restricted all people able to edit this field to those you trust."),
      '#default_value' => $this->getSetting('filter_format'),
      '#required' => TRUE,
    ];

    return $element;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsSummary() {
    $summary = [];

    $format = $this->getSetting('filter_format');
    $all_formats = filter_formats();
    $format_label = $all_formats[$format]->get('name');

    $num_rows = $this->getSetting('num_rows');

    $summary[] = $this->t("Text Format: @format", ['@format' => $format_label]);
    $summary[] = $this->t("Number of Rows: @rows", ['@rows' => $num_rows]);

    return $summary;
  }
}
