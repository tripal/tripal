<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

/**
 * Plugin implementation of default Tripal linker property widget.
 *
 * @FieldWidget(
 *   id = "chado_property_widget_default",
 *   label = @Translation("Chado Property: Long Text"),
 *   description = @Translation("Provides a long text widget for Chado Properties using a formatted textarea."),
 *   field_types = {
 *     "chado_property_type_default"
 *   }
 * )
 */
class ChadoPropertyWidgetDefault extends ChadoWidgetBase {


  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    // Get the field settings.
    $field_definition = $items[$delta]->getFieldDefinition();
    $field_settings = $field_definition->getSettings();
    $field_name = $field_definition->get('field_name');
    $required = $field_definition->get('required');

    // Get the default values.
    $item_vals = $items[$delta]->getValue();
    $record_id = $item_vals['record_id'] ?? 0;
    $prop_id = $item_vals['prop_id'] ?? 0;
    $linker_id = $item_vals['linker_id'] ?? 0;
    $default_value = $item_vals['value'] ?? '';
    $term_id = NULL;
    if ($field_settings['termIdSpace'] and $field_settings['termAccession']) {
      $idSpace_manager = \Drupal::service('tripal.collection_plugin_manager.idspace');
      $idSpace = $idSpace_manager->loadCollection($field_settings['termIdSpace']);

      $term = $idSpace->getTerm($field_settings['termAccession']);
      $term_id = $term->getInternalId();
    }

    $elements = [];
    $elements['record_id'] = [
      '#type' => 'value',
      '#default_value' => $record_id,
    ];
    $elements['prop_id'] = [
      '#type' => 'value',
      '#default_value' => $prop_id,
    ];
    $elements['linker_id'] = [
      '#type' => 'value',
      '#value' => $linker_id,
    ];
    $elements['type_id'] = [
      '#type' => 'value',
      '#value' => $term_id,
    ];
    // pass the field machine name through the form for massageFormValues()
    $elements['field_name'] = [
      '#type' => 'value',
      '#default_value' => $field_name,
    ];
    $elements['value'] = $element + [
      '#base_type' => 'textarea',
      '#type' => 'text_format',
      '#format' => $this->getSetting('filter_format'),
      '#default_value' => $default_value,
      '#rows' => 5,
      '#required' => $required,
    ];
    $elements['rank'] = [
      '#type' => 'value',
      '#value' => $delta,
    ];

    // Save some initial values to allow later handling of the "Remove" button
    $this->saveInitialValues($delta, $field_name, $prop_id, $form_state);

    return $elements;
  }

  /**
   * {@inheritdoc}
   */
  public static function afterBuild(array $element, FormStateInterface $form_state) {
    parent::afterBuild($element, $form_state);

    // Alter the format drop down so that it is hidden.
    // We do this because any changes here are not actually saved and thus
    // having it enabled is misleading.
    // Note: We couldn't disable it or the text format element would stop working ;-)
    foreach (\Drupal\Core\Render\Element::children($element) as $key) {
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

    // Look up the rank term
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');
    $storage_settings = $this->getFieldSetting('storage_plugin_settings');
    $prop_table = $storage_settings['prop_table'];
    $rank_term = $this->sanitizeKey($mapping->getColumnTermId($prop_table, 'rank'));

    // Call parent massage helper function
    $values = $this->massagePropertyFormValues('value', $values, $form_state, $rank_term, 'prop_id');

    return $values;
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

    // Get all the filter formats available for the current site.
    $options = [];
    foreach (filter_formats() as $name => $object) {
      $options[$name] = $object->get('name');
    }

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

    $summary[] = $this->t("Text Format: @format", ['@format' => $format_label]);

    return $summary;
  }
}
