<?php

namespace Drupal\tripal_image\Plugin\Field\FieldWidget;

use Drupal\Core\Field\Attribute\FieldWidget;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

/**
 * Plugin implementation of the default Chado image widget.
 */
#[FieldWidget(
  id: 'chado_eimage_widget_default',
  label: new TranslatableMarkup('Chado Image Widget'),
  description: new TranslatableMarkup('Edits images store in the chado eimage table'),
  field_types: [
    'chado_eimage_type_default',
  ],
)]
class ChadoEimageWidgetDefault extends ChadoWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    // Get the field settings.
    $field_definition = $items[$delta]->getFieldDefinition();
    $storage_settings = $field_definition->getSetting('storage_plugin_settings');
    $linker_fkey_column = $storage_settings['linker_fkey_column']
      ?? $storage_settings['base_column'] ?? 'image_id';
    $property_definitions = $items[$delta]->getFieldDefinition()->getFieldStorageDefinition()->getPropertyDefinitions();
    $field_name = $items->getFieldDefinition()->get('field_name');

    $item_vals = $items[$delta]->getValue();
    $record_id = $item_vals['record_id'] ?? 0;
    $linker_id = $item_vals['linker_id'] ?? 0;
    $link = $item_vals['link'] ?? 0;
    $image_id = $item_vals[$linker_fkey_column] ?? 0;

    $elements = [];
    $elements['record_id'] = [
      '#type' => 'value',
      '#default_value' => $record_id,
    ];
    $elements['linker_id'] = [
      '#type' => 'value',
      '#default_value' => $linker_id,
    ];
    $elements['link'] = [
      '#type' => 'value',
      '#default_value' => $link,
    ];
    // Pass the foreign key name through the form for massageFormValues().
    $elements['linker_fkey_column'] = [
      '#type' => 'value',
      '#default_value' => $linker_fkey_column,
    ];
    // Pass the field machine name through the form for massageFormValues().
    $elements['field_name'] = [
      '#type' => 'value',
      '#default_value' => $field_name,
    ];





    // If there is a type_id and the value is not already set, then we want to
    // use the cvterm of the field as the default.
    if (array_key_exists('linker_type_id', $property_definitions)) {

      if (empty($item['linker_type_id'])) {
        $termIdSpace = $this->getFieldSetting('termIdSpace');
        $termAccession = $this->getFieldSetting('termAccession');

        $idSpace_manager = \Drupal::service('tripal.collection_plugin_manager.idspace');
        $idSpace = $idSpace_manager->loadCollection($termIdSpace);
        $term = $idSpace->getTerm($termAccession);

        $item['linker_type_id'] = $term->getInternalId();
      }

      $elements['linker_type_id'] = [
        '#type' => 'value',
        '#default_value' => $item['linker_type_id'],
      ];
    }

    // If there is a rank and it is not already set,
    // then we want to use 0 as the default.
    if (array_key_exists('linker_rank', $property_definitions)) {
      $default_value = $item_vals['linker_rank'] ?? 0;
      $elements['linker_rank'] = [
        '#type' => 'value',
        '#default_value' => $default_value,
      ];
    }

    // Save some initial values to allow later handling of the "Remove" button.
    $this->saveInitialValues($delta, $field_name, $linker_id, $form_state);

    return $elements;
  }

  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {
    $values = $this->genericSelectMassageFormValues('license_id', $values);
    return $this->massageLinkingFormValues('license_id', $values, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    return self::defaultSelectSettings() + parent::defaultSettings();
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {
    return $this->selectSettingsForm($form, $form_state) + parent::settingsForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function settingsSummary() {
    return $this->selectSettingsSummary() + parent::settingsSummary();
  }

}
