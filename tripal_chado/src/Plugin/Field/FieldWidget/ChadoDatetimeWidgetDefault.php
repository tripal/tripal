<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldWidget;
use Drupal\tripal\Plugin\Field\FieldWidget\TripalDatetimeTypeWidget;
use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;

/**
 * Plugin implementation of the default Chado datetime type widget.
 */
#[TripalFieldWidget(
  id: 'chado_datetime_type_widget',
  label: new TranslatableMarkup('Chado Datetime Widget'),
  description: new TranslatableMarkup('The default datetime type widget for Chado.'),
  field_types: [
    'chado_datetime_type_default',
  ],
)]
class ChadoDatetimeWidgetDefault extends TripalDatetimeTypeWidget {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {
    $item_vals = $items[$delta]->getValue();
    $element = parent::formElement($items, $delta, $element, $form, $form_state);
    $element['record_id'] = [
      '#type' => 'value',
      '#default_value' => $item_vals['record_id'] ?? 0,
    ];

    $settings = $this->fieldDefinition->getSetting('storage_plugin_settings');
    $base_table = $settings['base_table'] ?? '';
    $base_column = $settings['base_column'] ?? '';
    if ($base_table && $base_column) {
      $table_def = ChadoFieldItemBase::getChadoTableDef($base_table);
      if (!empty($table_def['fields'][$base_column]['not null'])) {
        $element['value']['#element_validate'][] = [$this, 'validateNotNullConstraint'];
      }
    }

    return $element;
  }

  /**
   * Validation: prevents clearing a NOT NULL datetime column on existing rows.
   *
   * Only attached when the backing Chado column has a NOT NULL constraint.
   * Empty values are accepted for new records (record_id == 0) because Chado
   * will supply its DEFAULT on INSERT; they are rejected for existing records
   * because the omitted UPDATE would leave Drupal and Chado out of sync.
   */
  public function validateNotNullConstraint(array &$element, FormStateInterface $form_state): void {
    $value = trim($element['#value'] ?? '');
    if ($value !== '') {
      return;
    }

    // Resolve the sibling record_id from the same delta in form state.
    $parents = $element['#parents'];   // e.g. ['field_foo', 0, 'value']
    array_pop($parents);               // → ['field_foo', 0]
    $parents[] = 'record_id';         // → ['field_foo', 0, 'record_id']
    $record_id = (int) $form_state->getValue($parents, 0);

    if ($record_id > 0) {
      $form_state->setError(
        $element,
        $this->t('This date/time field cannot be empty, please enter a valid date/time.')
      );
    }
  }

}
