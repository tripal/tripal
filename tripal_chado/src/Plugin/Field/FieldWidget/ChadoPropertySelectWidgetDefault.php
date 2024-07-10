<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

/**
 * Plugin implementation of default Tripal linker property widget.
 *
 * @FieldWidget(
 *   id = "chado_property_select_widget_default",
 *   label = @Translation("Chado Property: Select Drop-down"),
 *   description = @Translation("Provides a configurable drop-down widget for Chado Properties."),
 *   field_types = {
 *     "chado_property_type_default"
 *   }
 * )
 */
class ChadoPropertySelectWidgetDefault extends ChadoWidgetBase {


  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    // Get the field settings.
    $field_definition = $items[$delta]->getFieldDefinition();
    $field_settings = $field_definition->getSettings();

    // Get the default values.
    $item_vals = $items[$delta]->getValue();
    $record_id = $item_vals['record_id'] ?? 0;
    $prop_id = $item_vals['prop_id'] ?? 0;
    $linker_id = $item_vals['linker_id'] ?? 0;
    $default_value = $item_vals['value'] ?? NULL;
    $term_id = NULL;
    if ($field_settings['termIdSpace'] and $field_settings['termAccession']) {
      $idSpace_manager = \Drupal::service('tripal.collection_plugin_manager.idspace');
      $idSpace = $idSpace_manager->loadCollection($field_settings['termIdSpace']);

      $term = $idSpace->getTerm($field_settings['termAccession']);
      $term_id = $term->getInternalId();
    }

    // Process the options from the configuration.
    $raw_options = $this->getSetting('options');
    $options = [];
    foreach( explode("\n", $raw_options) as $item) {
      $item = trim($item);
      $options[$item] = $item;
    }

    // Check that the default value is one of the available options.
    if (isset($default_value) and !array_key_exists($default_value, $options)) {
      // Do not display the warning when saving
      if (!$form_state->getUserInput()) {
        $field_label = $field_definition->label();
        $this->messenger()->addWarning($this->t("The value saved for the '@field_label' does not match any of the available options. As such it has been set to 'None' and the original value ('@default_value') will be overwritten when you save this page. Please contact your administrator to get this added as an option before making any changes to this page.",
            ['@field_label' => $field_label, '@default_value' => $default_value]));
      }
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
    $elements['value'] = $element + [
      '#type' => 'select',
      '#default_value' => $default_value,
      '#empty_option' => '- None -',
      '#options' => $options,
      '#required' => FALSE,
    ];
    $elements['rank'] = [
      '#type' => 'value',
      '#value' => $delta,
    ];
    return $elements;
  }

  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');

    $storage_settings = $this->getFieldSetting('storage_plugin_settings');
    $prop_table = $storage_settings['prop_table'];
    $rank_term = $this->sanitizeKey($mapping->getColumnTermId($prop_table, 'rank'));

    // Remove any empty values that aren't mapped to a record id.
    foreach ($values as $val_key => $value) {
      if ($value['value'] == '' and $value['record_id'] == 0) {
        unset($values[$val_key]);
      }
    }

    // Reset the weights
    $i = 0;
    foreach ($values as $val_key => $value) {
      if ($value['value'] == '') {
        continue;
      }
      $values[$val_key]['_weight'] = $i;
      $values[$val_key][$rank_term] = $i;
      $i++;
    }

    return $values;
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    return [
      'options' => '',
    ] + parent::defaultSettings();
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {
    $element['options'] = [
      '#type' => 'textarea',
      '#title' => $this->t('Widget Options'),
      '#description' => $this->t("Enter the options you want to be available in the widget drop down with each option on its own line."),
      '#default_value' => $this->getSetting('options'),
      '#required' => TRUE,
    ];

    return $element;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsSummary() {
    $summary = [];

    $raw_options = $this->getSetting('options');
    if (!empty($raw_options)) {
      $count = sizeof(explode("\n", $raw_options));
      $summary[] = $this->t("There are @count options configured.", ['@count' => $count]);
    }
    else {
      $summary[] = $this->t("There are no options configured yet.");
    }

    return $summary;
  }

}
