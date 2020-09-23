<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Field\WidgetBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of the 'obi__organism_default_widget' widget.
 *
 * @FieldWidget(
 *   id = "obi__organism_default_widget",
 *   module = "tripal_chado",
 *   label = @Translation("Organism: Select List"),
 *   field_types = {
 *     "obi__organism"
 *   }
 * )
 */
class OBIOrganismDefaultWidget extends WidgetBase {

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    return [
      'placeholder' => '- Select -',
    ] + parent::defaultSettings();
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {
    $elements = [];

    $elements['placeholder'] = [
      '#type' => 'textfield',
      '#title' => t('Placeholder'),
      '#default_value' => $this->getSetting('placeholder'),
      '#description' => t('Text that will be shown inside the field until a value is entered. This hint is usually a sample value or a brief description of the expected format.'),
    ];

    return $elements;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsSummary() {
    $summary = [];

    if (!empty($this->getSetting('placeholder'))) {
      $summary[] = t('Placeholder: @placeholder', ['@placeholder' => $this->getSetting('placeholder')]);
    }

    return $summary;
  }

  /**
   * Retrieve a specific value from the items list.
   *
   * @todo move this into a ChadoWidgetBase class.
   *
   * @param array $items
   *   An array of default value items for the OBIOrganismItem field.
   * @param int $delta
   *   The index of the current item.
   * @param string $property_name
   *   The name of the value or property you would like to pull out. Supported
   *   values include record_id, chado_schema, etc.
   */
  public function getChadoValue($items, $delta, $property_name) {

    if ($property_name == 'record_id') {
      $record_id = $items[$delta]->get('record_id')->getValue();

      // Get a default value if it's not set?
      if (!$record_id) {
        $record_id = $this->getDefaultRecordID();
      }

      return $record_id;
    }
    elseif ($property_name == 'chado_schema') {
      return $items[$delta]->get('chado_schema')->getValue();
    }
    else {
      $values = unserialize($items[$delta]->getValue());
      if (isset($values[$property_name])) {
        return $values[$property_name];
      }
    }
    return NULL;
  }

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    // Use the value to store the previous values for this field.
    $element['value'] = $element + [
      '#tree' => TRUE,
    ];

    $element['value']['genus'] = [
      '#type' => 'hidden',
      '#default_value' => $this->getChadoValue($items, $delta, 'genus'),
    ];

    $element['value']['species'] = [
      '#type' => 'hidden',
      '#default_value' => $this->getChadoValue($items, $delta, 'species'),
    ];

    $element['value']['common_name'] = [
      '#type' => 'hidden',
      '#default_value' => $this->getChadoValue($items, $delta, 'common_name'),
    ];

    $element['value']['abbreviation'] = [
      '#type' => 'hidden',
      '#default_value' => $this->getChadoValue($items, $delta, 'abbreviation'),
    ];

    // Now store the new organism in the record_id.
    // -- Retrieve the organism options fro, the database (all not just published).
    $options = chado_get_organism_select_options(FALSE);
    // -- Remove the old "Select an organism" so that admin can configure it as the placeholder.
    unset($options[0]);
    // -- Finally, the form element that makes the magic happen!
    $element['record_id'] = [
      '#type' => 'select',
      '#title' => $element['#title'],
      '#description' => $element['#description'],
      '#options' => $options,
      '#default_value' => $this->getChadoValue($items, $delta, 'record_id'),
      '#empty_option' => $this->getSetting('placeholder'),
      '#required' => $element['#required'],
      '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
      '#delta' => $delta,
    ];

    $element['chado_schema'] = [
      '#type' => 'textfield',
      '#title' => 'Chado Schema',
      '#description' => 'The name of the chado schema this record is stored in.',
      '#default_value' => $this->getChadoValue($items, $delta, 'chado_schema'),
    ];

    return $element;
  }

  /**
   * Extract the default record_id for the field.
   *
   * @return integer
   *   The record ID represented by the default value set in the field settings.
   */
  public function getDefaultRecordID() {

    // We can access the values array stored in the field definition.
    // This is a serialized value so we need to unserialize it and
    // extract the unique keys in order to lookup the record ID.
    $default_serialized = $this->fieldDefinition->getDefaultValueLiteral();
    if ($default_serialized) {
      $defaultvals = unserialize($default_serialized[0]);
      if (isset($defaultvals['genus']) && isset($defaultvals['species'])) {
        $record_id = chado_query(
          'SELECT organism_id FROM {organism} WHERE genus=:g AND species=:sp',
          [':g' => $defaultvals['genus'], ':sp' => $defaultvals['species']])
          ->fetchField();
        return $record_id;
      }
    }
  }
}
