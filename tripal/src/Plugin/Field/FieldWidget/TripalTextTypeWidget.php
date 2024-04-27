<?php

namespace Drupal\tripal\Plugin\Field\FieldWidget;

use Drupal\tripal\TripalField\TripalWidgetBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of default Tripal text type widget.
 *
 * @FieldWidget(
 *   id = "default_tripal_text_type_widget",
 *   label = @Translation("Tripal Text Widget"),
 *   description = @Translation("The default text type widget."),
 *   field_types = {
 *     "tripal_text_type"
 *   }
 * )
 */
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
    // Note: We couldn't disable it for the text format element would stop working ;-)
    foreach(\Drupal\Core\Render\Element::children($element) as $key) {
      $element[$key]['value']['format']['#attributes']['class'][] = 'hidden';
    }

    return $element;
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    return [
      'filter_format' => 'basic_html',
    ] + parent::defaultSettings();
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {

    $options = [
      'plain_text' => 'Plain Text',
      'basic_html' => 'Basic HTML',
      'filtered_html' => 'Filtered HTML',
      'full_html' => 'Full HTML',
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
    $options = [
      'plain_text' => 'Plain Text',
      'basic_html' => 'Basic HTML',
      'filtered_html' => 'Filtered HTML',
      'full_html' => 'Full HTML',
    ];

    $summary[] = $this->t("Text Format: @format", ['@format' => $options[$format]]);

    return $summary;
  }
}
